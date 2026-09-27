//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#if defined(__MVS__)
// As part of monotonic clock support on z/OS we need macro _LARGE_TIME_API
// to be defined before any system header to include definition of struct timespec64.
#  define _LARGE_TIME_API
#endif

#include <__system_error/throw_system_error.h>
#include <cerrno> // errno
#include <chrono>

#if defined(__MVS__)
#  include <__support/ibm/gettod_zos.h> // gettimeofdayMonotonic
#endif

#include "include/apple_availability.h"
#include <time.h> // clock_gettime and CLOCK_{MONOTONIC,REALTIME,MONOTONIC_RAW}

#if __has_include(<unistd.h>)
#  include <unistd.h> // _POSIX_TIMERS
#endif

#if __has_include(<sys/time.h>)
#  include <sys/time.h> // for gettimeofday and timeval
#endif

#if defined(__LLVM_LIBC__)
#  define _LIBCPP_HAS_TIMESPEC_GET
#endif

// OpenBSD and GPU do not have a fully conformant suite of POSIX timers, but
// it does have clock_gettime and CLOCK_MONOTONIC which is all we need.
#if defined(__APPLE__) || defined(__gnu_hurd__) || defined(__OpenBSD__) || defined(__AMDGPU__) ||                      \
    defined(__NVPTX__) || (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0)
#  define _LIBCPP_HAS_CLOCK_GETTIME
#endif

#if defined(_LIBCPP_WIN32API)
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRA_LEAN
#  include <windows.h>
#  if _WIN32_WINNT >= _WIN32_WINNT_WIN8
#    include <winapifamily.h>
#  endif
#endif // defined(_LIBCPP_WIN32API)

#if defined(__Fuchsia__)
#  include <zircon/syscalls.h>
#endif

#if defined(__sgi)
#  include <cstdint>
#  include <fcntl.h>
#  include <mutex>
#  include <sys/mman.h>
#  include <sys/syssgi.h>
#  include <sys/times.h>
#endif

#if __has_include(<mach/mach_time.h>)
#  include <mach/mach_time.h>
#endif

#if defined(__ELF__) && defined(_LIBCPP_LINK_RT_LIB)
#  pragma comment(lib, "rt")
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

namespace chrono {

//
// system_clock
//

#if defined(_LIBCPP_WIN32API)

#  if _WIN32_WINNT < _WIN32_WINNT_WIN8

namespace {

typedef void(WINAPI* GetSystemTimeAsFileTimePtr)(LPFILETIME);

class GetSystemTimeInit {
public:
  GetSystemTimeInit() {
    fp = (GetSystemTimeAsFileTimePtr)(void*)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "GetSystemTimePreciseAsFileTime");
    if (fp == nullptr)
      fp = GetSystemTimeAsFileTime;
  }
  GetSystemTimeAsFileTimePtr fp;
};

// Pretend we're inside a system header so the compiler doesn't flag the use of the init_priority
// attribute with a value that's reserved for the implementation (we're the implementation).
#    include "chrono_system_time_init.h"
} // namespace

#  endif

static system_clock::time_point __libcpp_system_clock_now() {
  // FILETIME is in 100ns units
  using filetime_duration =
      std::chrono::duration<__int64, std::ratio_multiply<std::ratio<100, 1>, nanoseconds::period>>;

  // The Windows epoch is Jan 1 1601, the Unix epoch Jan 1 1970.
  static constexpr const seconds nt_to_unix_epoch{11644473600};

  FILETIME ft;
#  if (_WIN32_WINNT >= _WIN32_WINNT_WIN8 && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)) ||                      \
      (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
  GetSystemTimePreciseAsFileTime(&ft);
#  elif !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
  GetSystemTimeAsFileTime(&ft);
#  else
  GetSystemTimeAsFileTimeFunc.fp(&ft);
#  endif

  filetime_duration d{(static_cast<__int64>(ft.dwHighDateTime) << 32) | static_cast<__int64>(ft.dwLowDateTime)};
  return system_clock::time_point(duration_cast<system_clock::duration>(d - nt_to_unix_epoch));
}

#elif defined(_LIBCPP_HAS_TIMESPEC_GET)

static system_clock::time_point __libcpp_system_clock_now() {
  struct timespec ts;
  if (timespec_get(&ts, TIME_UTC) != TIME_UTC)
    std::__throw_system_error(errno, "timespec_get(TIME_UTC) failed");
  return system_clock::time_point(seconds(ts.tv_sec) + microseconds(ts.tv_nsec / 1000));
}

#elif defined(_LIBCPP_HAS_CLOCK_GETTIME)

static system_clock::time_point __libcpp_system_clock_now() {
  struct timespec tp;
  if (0 != clock_gettime(CLOCK_REALTIME, &tp))
    std::__throw_system_error(errno, "clock_gettime(CLOCK_REALTIME) failed");
  return system_clock::time_point(seconds(tp.tv_sec) + microseconds(tp.tv_nsec / 1000));
}

#else

static system_clock::time_point __libcpp_system_clock_now() {
  timeval tv;
  gettimeofday(&tv, 0);
  return system_clock::time_point(seconds(tv.tv_sec) + microseconds(tv.tv_usec));
}

#endif

_LIBCPP_DIAGNOSTIC_PUSH
_LIBCPP_CLANG_DIAGNOSTIC_IGNORED("-Wdeprecated")
const bool system_clock::is_steady;
_LIBCPP_DIAGNOSTIC_POP

system_clock::time_point system_clock::now() noexcept { return __libcpp_system_clock_now(); }

time_t system_clock::to_time_t(const time_point& t) noexcept {
  return time_t(duration_cast<seconds>(t.time_since_epoch()).count());
}

system_clock::time_point system_clock::from_time_t(time_t t) noexcept { return system_clock::time_point(seconds(t)); }

//
// steady_clock
//
// Warning:  If this is not truly steady, then it is non-conforming.  It is
//  better for it to not exist and have the rest of libc++ use system_clock
//  instead.
//

#if _LIBCPP_HAS_MONOTONIC_CLOCK

#  if defined(__APPLE__)

// On Apple platforms, only CLOCK_UPTIME_RAW, CLOCK_MONOTONIC_RAW or
// mach_absolute_time are able to time functions in the nanosecond range.
// Furthermore, only CLOCK_MONOTONIC_RAW is truly monotonic, because it
// also counts cycles when the system is asleep. Thus, it is the only
// acceptable implementation of steady_clock.
static steady_clock::time_point __libcpp_steady_clock_now() {
  struct timespec tp;
  if (0 != clock_gettime(CLOCK_MONOTONIC_RAW, &tp))
    std::__throw_system_error(errno, "clock_gettime(CLOCK_MONOTONIC_RAW) failed");
  return steady_clock::time_point(seconds(tp.tv_sec) + nanoseconds(tp.tv_nsec));
}

#  elif defined(_LIBCPP_WIN32API)

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms644905(v=vs.85).aspx says:
//    If the function fails, the return value is zero. <snip>
//    On systems that run Windows XP or later, the function will always succeed
//      and will thus never return zero.

static LARGE_INTEGER __QueryPerformanceFrequency() {
  LARGE_INTEGER val;
  (void)QueryPerformanceFrequency(&val);
  return val;
}

static steady_clock::time_point __libcpp_steady_clock_now() {
  static const LARGE_INTEGER freq = __QueryPerformanceFrequency();

  LARGE_INTEGER counter;
  (void)QueryPerformanceCounter(&counter);
  auto seconds   = counter.QuadPart / freq.QuadPart;
  auto fractions = counter.QuadPart % freq.QuadPart;
  auto dur       = seconds * nano::den + fractions * nano::den / freq.QuadPart;
  return steady_clock::time_point(steady_clock::duration(dur));
}

#  elif defined(__MVS__)

static steady_clock::time_point __libcpp_steady_clock_now() {
  struct timespec64 ts;
  if (0 != gettimeofdayMonotonic(&ts))
    std::__throw_system_error(errno, "failed to obtain time of day");

  return steady_clock::time_point(seconds(ts.tv_sec) + nanoseconds(ts.tv_nsec));
}

#  elif defined(__Fuchsia__)

static steady_clock::time_point __libcpp_steady_clock_now() noexcept {
  // Implicitly link against the vDSO system call ABI without
  // requiring the final link to specify -lzircon explicitly when
  // statically linking libc++.
#    pragma comment(lib, "zircon")

  return steady_clock::time_point(nanoseconds(_zx_clock_get_monotonic()));
}

#  elif defined(__sgi)

// IRIX has no CLOCK_MONOTONIC. What it has that never goes back is the
// machine's free-running counter, which CLOCK_SGI_CYCLE reads by mapping it
// from /dev/mmem -- but leaves its wraps to the caller, and the counter is
// 32 bits on many machines (an Indigo2's wraps every 44 seconds); and
// clock_gettime rounds its period down to whole nanoseconds (10.256 ns to 10
// there). So map the counter here, keep its exact period, in picoseconds,
// and count the wraps with times(), whose clock ticks since boot are coarse
// but last over a year at 32 bits: of the whole wraps the counter may have
// made since the last reading, the one nearest what times() says has
// passed. Each reading is the reference for the next, so the two clocks,
// which run off different oscillators, only need to agree to half a wrap
// between readings, not over the life of the process. Without the counter,
// times() alone serves.
namespace {
class __irix_steady_clock {
  volatile uint32_t* __addr32_ = nullptr;
  volatile uint64_t* __addr64_ = nullptr;
  uint64_t __period_ps_        = 0;
  uint64_t __tick_ns_          = 0;
  uint64_t __boot_ns_          = 0; // times() at the first reading

  // The last reading of a 32-bit counter.
  mutex __mutex_;
  uint32_t __last_count_ = 0;
  clock_t __last_ticks_  = 0;
  uint64_t __counts_     = 0; // since the first reading

  // Counts to nanoseconds, without overflow for centuries of counts.
  uint64_t __ns(uint64_t __n) const { return __n * (__period_ps_ / 1000) + __n * (__period_ps_ % 1000) / 1000; }

public:
  __irix_steady_clock() {
    long __hz  = sysconf(_SC_CLK_TCK);
    __tick_ns_ = 1000000000 / (__hz > 0 ? __hz : 100);
    struct tms __tms;
    __last_ticks_ = times(&__tms);
    __boot_ns_    = static_cast<uint64_t>(static_cast<uint32_t>(__last_ticks_)) * __tick_ns_;

    unsigned __period = 0;
    ptrdiff_t __bits  = syssgi(SGI_CYCLECNTR_SIZE);
    ptrdiff_t __phys  = syssgi(SGI_QUERY_CYCLECNTR, &__period);
    if ((__bits != 32 && __bits != 64) || __phys == -1 || __period == 0)
      return;
    ptrdiff_t __page = getpagesize();
    int __fd         = open("/dev/mmem", O_RDONLY);
    if (__fd < 0)
      return;
    void* __map = mmap(nullptr, __page, PROT_READ, MAP_PRIVATE, __fd, __phys & ~(__page - 1));
    close(__fd);
    if (__map == MAP_FAILED)
      return;
    auto* __at   = static_cast<volatile char*>(__map) + (__phys & (__page - 1));
    __period_ps_ = __period;
    if (__bits == 64)
      __addr64_ = reinterpret_cast<volatile uint64_t*>(__at);
    else {
      __addr32_     = reinterpret_cast<volatile uint32_t*>(__at);
      __last_count_ = *__addr32_;
    }
  }

  nanoseconds __now() {
    if (__addr64_)
      return nanoseconds(__ns(*__addr64_));
    struct tms __tms;
    lock_guard<mutex> __lock(__mutex_);
    clock_t __ticks = times(&__tms);
    uint64_t __since = static_cast<uint64_t>(static_cast<uint32_t>(__ticks - __last_ticks_)) * __tick_ns_;
    __last_ticks_    = __ticks;
    if (!__addr32_) {
      __counts_ += __since; // in nanoseconds, then
      return nanoseconds(__boot_ns_ + __counts_);
    }
    uint32_t __count   = *__addr32_;
    uint64_t __fine    = static_cast<uint32_t>(__count - __last_count_);
    __last_count_      = __count;
    uint64_t __wrap_ns = __ns(uint64_t(1) << 32);
    uint64_t __fine_ns = __ns(__fine);
    uint64_t __wraps   = __since + __wrap_ns / 2 > __fine_ns ? (__since + __wrap_ns / 2 - __fine_ns) / __wrap_ns : 0;
    __counts_ += __fine + (__wraps << 32);
    return nanoseconds(__boot_ns_ + __ns(__counts_));
  }
};
} // namespace

static steady_clock::time_point __libcpp_steady_clock_now() {
  static __irix_steady_clock __clock;
  return steady_clock::time_point(__clock.__now());
}

#  elif defined(_LIBCPP_HAS_TIMESPEC_GET)

static steady_clock::time_point __libcpp_steady_clock_now() {
  struct timespec ts;
  if (timespec_get(&ts, TIME_MONOTONIC) != TIME_MONOTONIC)
    std::__throw_system_error(errno, "timespec_get(TIME_MONOTONIC) failed");
  return steady_clock::time_point(seconds(ts.tv_sec) + microseconds(ts.tv_nsec / 1000));
}

#  elif defined(_LIBCPP_HAS_CLOCK_GETTIME)

static steady_clock::time_point __libcpp_steady_clock_now() {
  struct timespec tp;
  if (0 != clock_gettime(CLOCK_MONOTONIC, &tp))
    std::__throw_system_error(errno, "clock_gettime(CLOCK_MONOTONIC) failed");
  return steady_clock::time_point(seconds(tp.tv_sec) + nanoseconds(tp.tv_nsec));
}

#  else
#    error "Monotonic clock not implemented on this platform"
#  endif

_LIBCPP_DIAGNOSTIC_PUSH
_LIBCPP_CLANG_DIAGNOSTIC_IGNORED("-Wdeprecated")
const bool steady_clock::is_steady;
_LIBCPP_DIAGNOSTIC_POP

steady_clock::time_point steady_clock::now() noexcept { return __libcpp_steady_clock_now(); }

#endif // _LIBCPP_HAS_MONOTONIC_CLOCK

} // namespace chrono

_LIBCPP_END_NAMESPACE_STD
