//===-- crtbegin.c - Start of constructors and destructors ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <stddef.h>

#ifdef __sgi
#include <pthread.h>
#endif

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#if __has_feature(ptrauth_init_fini)
#include <ptrauth.h>
#endif

__attribute__((visibility("hidden"))) void *__dso_handle = &__dso_handle;

#ifdef EH_USE_FRAME_REGISTRY
__extension__ static void *__EH_FRAME_LIST__[]
    __attribute__((section(".eh_frame"), aligned(sizeof(void *)))) = {};

extern void __register_frame_info(const void *, void *) __attribute__((weak));
extern void *__deregister_frame_info(const void *) __attribute__((weak));
#endif

#ifndef CRT_HAS_INITFINI_ARRAY
typedef void (*fp)(void);

static fp __CTOR_LIST__[]
    __attribute__((section(".ctors"), aligned(sizeof(fp)))) = {(fp)-1};
extern fp __CTOR_LIST_END__[];
#endif

#ifndef __sgi
extern void __cxa_finalize(void *) __attribute__((weak));
#endif

static void __attribute__((used)) __do_init(void) {
  static _Bool __initialized;
  if (__builtin_expect(__initialized, 0))
    return;
  __initialized = 1;

#ifdef EH_USE_FRAME_REGISTRY
  static struct { void *p[8]; } __object;
  if (__register_frame_info)
    __register_frame_info(__EH_FRAME_LIST__, &__object);
#endif
#ifndef CRT_HAS_INITFINI_ARRAY
  const size_t n = __CTOR_LIST_END__ - __CTOR_LIST__ - 1;
  for (size_t i = n; i >= 1; i--) __CTOR_LIST__[i]();
#endif
}

#ifdef CRT_HAS_INITFINI_ARRAY
# if __has_feature(ptrauth_init_fini)
// TODO: use __ptrauth-qualified pointers when they are supported on clang side
#  if __has_feature(ptrauth_init_fini_address_discrimination)
__attribute__((section(".init_array"), used)) static void *__init =
    ptrauth_sign_constant(&__do_init, ptrauth_key_init_fini_pointer,
                          ptrauth_blend_discriminator(
                              &__init, __ptrauth_init_fini_discriminator));
#  else
__attribute__((section(".init_array"), used)) static void *__init =
    ptrauth_sign_constant(&__do_init, ptrauth_key_init_fini_pointer,
                          __ptrauth_init_fini_discriminator);
#  endif
# elif __has_feature(ptrauth_calls)
#  ifdef __aarch64__
// If ptrauth_init_fini feature is not present, compiler emits raw unsigned
// pointers in .init_array. Use inline assembly to avoid implicit signing of
// __do_init function pointer with ptrauth_calls enabled.
__asm__(".pushsection .init_array,\"aw\",@init_array\n\t"
        ".xword __do_init\n\t"
        ".popsection");
#  else
#   error "ptrauth_calls is only supported for AArch64"
#  endif
# else
__attribute__((section(".init_array"),
               used)) static void (*__init)(void) = __do_init;
# endif
#elif defined(__sgi)
// IRIX: rld calls DT_INIT and DT_FINI -- the linker points them at _init
// and _fini -- for executables and shared objects alike. crt1.o's .init
// (the __istart it builds) runs for executables only. Hidden, so one
// object's _init cannot interpose on another's.
__attribute__((visibility("hidden"), used)) void _init(void) { __do_init(); }
#elif defined(__i386__) || defined(__x86_64__)
__asm__(".pushsection .init,\"ax\",@progbits\n\t"
        "call __do_init\n\t"
        ".popsection");
#elif defined(__riscv)
__asm__(".pushsection .init,\"ax\",%progbits\n\t"
        "call __do_init\n\t"
        ".popsection");
#elif defined(__arm__) || defined(__aarch64__)
__asm__(".pushsection .init,\"ax\",%progbits\n\t"
        "bl __do_init\n\t"
        ".popsection");
#elif defined(__mips__)
__asm__(".pushsection .init,\"ax\",@progbits\n\t"
        "jal __do_init\n\t"
        ".popsection");
#elif defined(__powerpc__) || defined(__powerpc64__)
__asm__(".pushsection .init,\"ax\",@progbits\n\t"
        "bl __do_init\n\t"
        "nop\n\t"
        ".popsection");
#elif defined(__sparc__)
__asm__(".pushsection .init,\"ax\",@progbits\n\t"
        "call __do_init\n\t"
        ".popsection");
#else
#error "crtbegin without .init_fini array unimplemented for this architecture"
#endif // CRT_HAS_INITFINI_ARRAY

#ifndef CRT_HAS_INITFINI_ARRAY
static fp __DTOR_LIST__[]
    __attribute__((section(".dtors"), aligned(sizeof(fp)))) = {(fp)-1};
extern fp __DTOR_LIST_END__[];
#endif

#ifdef __sgi
// IRIX's libc has no __cxa_atexit (C++ registers static destructors with it)
// and its atexit holds only 32 entries. This one keeps a list in chunks,
// shared by every object: rld binds all of them to the first definition.
typedef void (*__cxa_atexit_fn)(void *);

#define __CXA_ATEXIT_CHUNK 256
struct __cxa_atexit_chunk {
  int count;
  struct __cxa_atexit_chunk *next;
  __cxa_atexit_fn funs[__CXA_ATEXIT_CHUNK];
  void *args[__CXA_ATEXIT_CHUNK];
  void *dsos[__CXA_ATEXIT_CHUNK];
};

// Weak: a program need not link malloc or pthreads for this to work.
void *malloc(size_t) __attribute__((weak));
int pthread_mutex_lock(pthread_mutex_t *) __attribute__((weak));
int pthread_mutex_unlock(pthread_mutex_t *) __attribute__((weak));

static struct __cxa_atexit_chunk __cxa_atexit_first;
static struct __cxa_atexit_chunk *__cxa_atexit_list = &__cxa_atexit_first;
static pthread_mutex_t __cxa_atexit_mutex = PTHREAD_MUTEX_INITIALIZER;

static void __cxa_atexit_lock(void) {
  if (pthread_mutex_lock)
    pthread_mutex_lock(&__cxa_atexit_mutex);
}

static void __cxa_atexit_unlock(void) {
  if (pthread_mutex_unlock)
    pthread_mutex_unlock(&__cxa_atexit_mutex);
}

int __cxa_atexit(__cxa_atexit_fn func, void *arg, void *dso) {
  __cxa_atexit_lock();
  if (__cxa_atexit_list->count == __CXA_ATEXIT_CHUNK) {
    struct __cxa_atexit_chunk *chunk =
        malloc ? (struct __cxa_atexit_chunk *)malloc(sizeof *chunk) : NULL;
    if (!chunk) {
      __cxa_atexit_unlock();
      return -1;
    }
    chunk->count = 0;
    chunk->next = __cxa_atexit_list;
    __cxa_atexit_list = chunk;
  }
  int i = __cxa_atexit_list->count++;
  __cxa_atexit_list->funs[i] = func;
  __cxa_atexit_list->args[i] = arg;
  __cxa_atexit_list->dsos[i] = dso;
  __cxa_atexit_unlock();
  return 0;
}

// Runs, newest first, the handlers registered for `dso` (all of them if it
// is NULL), each once.
void __cxa_finalize(void *dso) {
  __cxa_atexit_lock();
  for (struct __cxa_atexit_chunk *c = __cxa_atexit_list; c; c = c->next)
    for (int i = c->count - 1; i >= 0; i--) {
      if (!c->funs[i] || (dso && c->dsos[i] != dso))
        continue;
      __cxa_atexit_fn f = c->funs[i];
      c->funs[i] = NULL;
      __cxa_atexit_unlock();
      f(c->args[i]);
      __cxa_atexit_lock();
    }
  __cxa_atexit_unlock();
}
#endif

static void __attribute__((used)) __do_fini(void) {
  static _Bool __finalized;
  if (__builtin_expect(__finalized, 0))
    return;
  __finalized = 1;

#ifdef __sgi
  __cxa_finalize(__dso_handle); // defined above, never absent
#else
  if (__cxa_finalize)
    __cxa_finalize(__dso_handle);
#endif

#ifndef CRT_HAS_INITFINI_ARRAY
  const size_t n = __DTOR_LIST_END__ - __DTOR_LIST__ - 1;
  for (size_t i = 1; i <= n; i++) __DTOR_LIST__[i]();
#endif
#ifdef EH_USE_FRAME_REGISTRY
  if (__deregister_frame_info)
    __deregister_frame_info(__EH_FRAME_LIST__);
#endif
}

#ifdef CRT_HAS_INITFINI_ARRAY
# if __has_feature(ptrauth_init_fini)
// TODO: use __ptrauth-qualified pointers when they are supported on clang side
#  if __has_feature(ptrauth_init_fini_address_discrimination)
__attribute__((section(".fini_array"), used)) static void *__fini =
    ptrauth_sign_constant(&__do_fini, ptrauth_key_init_fini_pointer,
                          ptrauth_blend_discriminator(
                              &__fini, __ptrauth_init_fini_discriminator));
#  else
__attribute__((section(".fini_array"), used)) static void *__fini =
    ptrauth_sign_constant(&__do_fini, ptrauth_key_init_fini_pointer,
                          __ptrauth_init_fini_discriminator);
#  endif
# elif __has_feature(ptrauth_calls)
#  ifdef __aarch64__
// If ptrauth_init_fini feature is not present, compiler emits raw unsigned
// pointers in .fini_array. Use inline assembly to avoid implicit signing of
// __do_fini function pointer with ptrauth_calls enabled.
__asm__(".pushsection .fini_array,\"aw\",@fini_array\n\t"
        ".xword __do_fini\n\t"
        ".popsection");
#  else
#   error "ptrauth_calls is only supported for AArch64"
#  endif
# else
__attribute__((section(".fini_array"),
               used)) static void (*__fini)(void) = __do_fini;
# endif
#elif defined(__sgi)
__attribute__((visibility("hidden"), used)) void _fini(void) { __do_fini(); }
#elif defined(__i386__) || defined(__x86_64__)
__asm__(".pushsection .fini,\"ax\",@progbits\n\t"
        "call __do_fini\n\t"
        ".popsection");
#elif defined(__arm__) || defined(__aarch64__)
__asm__(".pushsection .fini,\"ax\",%progbits\n\t"
        "bl __do_fini\n\t"
        ".popsection");
#elif defined(__mips__)
__asm__(".pushsection .fini,\"ax\",@progbits\n\t"
        "jal __do_fini\n\t"
        ".popsection");
#elif defined(__powerpc__) || defined(__powerpc64__)
__asm__(".pushsection .fini,\"ax\",@progbits\n\t"
        "bl __do_fini\n\t"
        "nop\n\t"
        ".popsection");
#elif defined(__riscv)
__asm__(".pushsection .fini,\"ax\",@progbits\n\t"
        "call __do_fini\n\t"
        ".popsection");
#elif defined(__sparc__)
__asm__(".pushsection .fini,\"ax\",@progbits\n\t"
        "call __do_fini\n\t"
        ".popsection");
#else
#error "crtbegin without .init_fini array unimplemented for this architecture"
#endif  // CRT_HAS_INIT_FINI_ARRAY
