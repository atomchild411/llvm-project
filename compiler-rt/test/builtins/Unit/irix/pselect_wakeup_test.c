// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: target={{.*-irix.*}}
//===-- pselect_wakeup_test.c - irix/pselect.c against lost signals -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// IRIX only: it is unsupported anywhere else, and needs %run to start an
// IRIX binary (an emulator or a remote IRIX host). To run it by hand, build it
// with the RUN line above, copy the binary to an IRIX machine and run it. It
// prints PASS or FAIL lines and exits 0 only if all passed.
//
// pselect against a signal that is already pending, or that arrives while it waits
// (compiler-rt irix/pselect.c).
//
// IRIX has no pselect. An emulation that does sigprocmask(SIG_SETMASK, mask) and then
// select() delivers a signal that was pending while blocked inside the sigprocmask, before
// select starts, so select waits for something that has already happened. GNU make hangs
// on exactly that (it waits for SIGCHLD only through pselect). The other cases
// are what a replacement must keep right.
//
//   pending      SIGCHLD pending while blocked; pselect with an empty mask must return
//                -1/EINTR with the handler run, at once. (The case that hangs.)
//   during       SIGUSR1 sent 200 ms into a wait with no timeout: EINTR within a second
//   timeout      no signal, 300 ms timeout: returns 0 after at least that, not much more
//   ready        a descriptor becomes ready after 150 ms: returns 1 with it set
//   mask         the signal mask after each call is the one before it
//   race         60 rounds with the signal sent at 0..29 ms into a 3 s wait: each must end
//                by EINTR in under a second
//
// A 6 s alarm ends a hang: it is reported, the test does not block for ever.
// Prints PASS or FAIL lines; exit status 0 only if all passed.
//
//===----------------------------------------------------------------------===//
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t chld_seen, usr1_seen, alrm_seen;
static void on_chld(int s) { (void)s; chld_seen++; }
static void on_usr1(int s) { (void)s; usr1_seen++; }
static void on_alrm(int s) { (void)s; alrm_seen = 1; }

/* SA_RESTART, which signal() handlers may have by default:
 * the kernel then restarts select() after a handler, which a real pselect never does.
 * The watchdog must not restart, or a lost wake-up would hang the test. */
static void handle(int sig, void (*fn)(int), int flags)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = fn;
    sa.sa_flags = flags;
    sigemptyset(&sa.sa_mask);
    sigaction(sig, &sa, 0);
}

static int fails;

static void report(int ok, const char *name, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));
static void report(int ok, const char *name, const char *fmt, ...)
{
    va_list ap;
    printf("%s %s: ", ok ? "PASS" : "FAIL", name);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    putchar('\n');
    fflush(stdout);
    if (!ok) fails++;
}

static double now(void)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

static int same_mask(const sigset_t *a, const sigset_t *b)
{
    int s;
    for (s = 1; s < NSIG; s++)
        if (sigismember(a, s) != sigismember(b, s)) return 0;
    return 1;
}

/* A child that signals the parent after MS milliseconds and exits. */
static pid_t kicker(int sig, int ms)
{
    pid_t c = fork();
    if (c == 0) {
        struct timeval tv;
        tv.tv_sec = ms / 1000; tv.tv_usec = (ms % 1000) * 1000;
        if (ms) select(0, 0, 0, 0, &tv);
        kill(getppid(), sig);
        _exit(0);
    }
    return c;
}

int main(void)
{
    sigset_t block, empty, before, after;
    int fds[2], rc, e, i, bad, ok;
    fd_set rfds;
    double t0, dt;
    struct timespec ts;
    pid_t c;

    setvbuf(stdout, 0, _IOLBF, 0);
    sigemptyset(&empty);
    sigemptyset(&block);
    sigaddset(&block, SIGCHLD);
    sigaddset(&block, SIGUSR1);
    handle(SIGCHLD, on_chld, SA_RESTART);
    handle(SIGUSR1, on_usr1, SA_RESTART);
    handle(SIGALRM, on_alrm, 0);
    sigprocmask(SIG_BLOCK, &block, 0);
    pipe(fds);

    /* pending: what make -j does. A job exits at once; the parent sleeps so SIGCHLD is
     * pending, then waits on the jobserver pipe, which nothing ever writes to. */
    if (fork() == 0) _exit(0);
    sleep(1);
    sigprocmask(SIG_SETMASK, 0, &before);
    chld_seen = 0; alrm_seen = 0;
    alarm(6);
    FD_ZERO(&rfds); FD_SET(fds[0], &rfds);
    t0 = now();
    rc = pselect(fds[0] + 1, &rfds, 0, 0, 0, &empty);
    e = errno; dt = now() - t0;
    alarm(0);
    wait(0);
    ok = rc == -1 && e == EINTR && chld_seen && !alrm_seen && dt < 2.0;
    report(ok, "pending", "returned %d (%s) after %.2f s; SIGCHLD handler ran: %s; watchdog fired: %s%s",
           rc, rc < 0 ? strerror(e) : "ready", dt, chld_seen ? "yes" : "no", alrm_seen ? "yes" : "no",
           alrm_seen && chld_seen ? " => LOST WAKE-UP: the pending signal was consumed before the wait began" : "");
    sigprocmask(SIG_SETMASK, 0, &after);
    report(same_mask(&before, &after), "mask", "after a pending-signal wait the mask is the one before");

    /* the remaining cases do not want SIGCHLD from the helper children */
    signal(SIGCHLD, SIG_DFL);

    /* during: a signal sent while the wait is on */
    usr1_seen = 0; alrm_seen = 0;
    alarm(6);
    c = kicker(SIGUSR1, 200);
    FD_ZERO(&rfds); FD_SET(fds[0], &rfds);
    t0 = now();
    rc = pselect(fds[0] + 1, &rfds, 0, 0, 0, &empty);
    e = errno; dt = now() - t0;
    alarm(0);
    waitpid(c, 0, 0);
    ok = rc == -1 && e == EINTR && usr1_seen && !alrm_seen && dt >= 0.15 && dt < 1.0;
    report(ok, "during", "returned %d (%s) after %.2f s; SIGUSR1 handler ran: %s; watchdog fired: %s",
           rc, rc < 0 ? strerror(e) : "ready", dt, usr1_seen ? "yes" : "no", alrm_seen ? "yes" : "no");

    /* timeout: not short, not long */
    ts.tv_sec = 0; ts.tv_nsec = 300000000L;
    FD_ZERO(&rfds); FD_SET(fds[0], &rfds);
    t0 = now();
    rc = pselect(fds[0] + 1, &rfds, 0, 0, &ts, &empty);
    e = errno; dt = now() - t0;
    ok = rc == 0 && dt >= 0.3 && dt < 0.6 && !FD_ISSET(fds[0], &rfds);
    report(ok, "timeout", "returned %d after %.3f s (wanted 0.300 to 0.600)", rc, dt);

    /* ready: a descriptor wakes it, and only that descriptor is set */
    c = fork();
    if (c == 0) {
        struct timeval tv = {0, 150000};
        select(0, 0, 0, 0, &tv);
        write(fds[1], "x", 1);
        _exit(0);
    }
    ts.tv_sec = 5; ts.tv_nsec = 0;
    FD_ZERO(&rfds); FD_SET(fds[0], &rfds);
    t0 = now();
    rc = pselect(fds[0] + 1, &rfds, 0, 0, &ts, &empty);
    e = errno; dt = now() - t0;
    waitpid(c, 0, 0);
    ok = rc == 1 && FD_ISSET(fds[0], &rfds) && dt >= 0.1 && dt < 1.0;
    report(ok, "ready", "returned %d after %.2f s", rc, dt);
    { char b; read(fds[0], &b, 1); }

    sigprocmask(SIG_SETMASK, 0, &after);
    report(same_mask(&before, &after), "mask", "after the other waits the mask is still the one before");

    /* race: the signal lands anywhere from before the wait to well into it */
    bad = 0;
    srand(7);
    for (i = 0; i < 60; i++) {
        usr1_seen = 0;
        c = kicker(SIGUSR1, rand() % 30);
        ts.tv_sec = 3; ts.tv_nsec = 0;
        FD_ZERO(&rfds); FD_SET(fds[0], &rfds);
        t0 = now();
        rc = pselect(fds[0] + 1, &rfds, 0, 0, &ts, &empty);
        e = errno; dt = now() - t0;
        waitpid(c, 0, 0);
        if (!(rc == -1 && e == EINTR && usr1_seen && dt < 1.0)) {
            bad++;
            if (bad <= 5)
                printf("  round %d: returned %d (%s) after %.2f s, handler ran: %s\n", i, rc,
                       rc < 0 ? strerror(e) : "ok", dt, usr1_seen ? "yes" : "no");
            sigprocmask(SIG_UNBLOCK, &block, 0); /* take a pending one, keep going */
            sigprocmask(SIG_BLOCK, &block, 0);
        }
    }
    report(bad == 0, "race", "%d of 60 rounds lost the signal", bad);

    printf("# %s\n", fails ? "some checks failed" : "all checks passed");
    return fails != 0;
}
