// RUN: %clang_builtins %s %librt -lpthread -o %t && %run %t
// REQUIRES: target={{.*-irix.*}}
//===-- atfile_threads_test.c - irix/atfile.c against threads -------------===//
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
// prints PASS or FAIL and exits 0 only if nothing went wrong. It writes under
// /var/tmp, or the directory given as an argument.
//
// The openat family against threads (compiler-rt irix/atfile.c).
//
// IRIX has no openat, fstatat, unlinkat, mkdirat or fdopendir; this runtime
// emulates them, and the process's working directory is shared by all
// threads. An emulation that gets a directory's path by changing the working
// directory (fchdir to the descriptor, getcwd, fchdir back) lets any other
// thread that resolves a relative path in that moment see the wrong directory;
// one that moves a descriptor with dup2 can close another thread's file.
//
// Workers loop openat, fstatat, mkdirat, unlinkat and fdopendir on relative
// names in their own directory descriptors, and check every result against
// what is on disk. Alongside them a cwd thread keeps resolving relative paths
// in the working directory: open and stat of "here" (only the working
// directory has one), and create and unlink of a file there. It also checks
// its own descriptors stay valid (EBADF).
//
// Failure kinds, counted and printed:
//   wrongdir   a worker's result named another directory's file, or a file it
//              made is not where it should be
//   cwdseen    the cwd thread did not see the working directory ("here" gone,
//              or its new file landed in a worker's directory)
//   ebadf      a descriptor a thread was holding stopped being valid
//   other      any other unexpected error
//
// usage: atfile_threads_test [-n ITERATIONS] [-w WORKERS] [DIR]
// Prints PASS or FAIL lines and exits 0 only if nothing went wrong.
//
//===----------------------------------------------------------------------===//
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXW 16

static char root[256];
static int iterations = 400, nworkers = 4;
static volatile int stop;         /* set when enough has gone wrong */
static volatile int workers_done; /* workers finished */

static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
static long n_wrongdir, n_cwdseen, n_ebadf, n_other, shown;

static void bad(long *counter, const char *who, const char *what, int err)
{
    pthread_mutex_lock(&mu);
    (*counter)++;
    if (shown++ < 12)
        printf("  %s: %s (errno %d: %s)\n", who, what, err, err ? strerror(err) : "-");
    if (n_wrongdir + n_cwdseen + n_ebadf + n_other >= 40)
        stop = 1; /* no point in running a broken build for minutes */
    pthread_mutex_unlock(&mu);
}

static void abs_path(char *out, size_t n, const char *dir, const char *name)
{
    snprintf(out, n, "%s/%s/%s", root, dir, name);
}

struct worker {
    int id;
    int dfd;
    ino_t want_f; /* inode of "f" in this worker's directory */
    long ops;
};

static void *worker_main(void *arg)
{
    struct worker *w = arg;
    char who[32], dir[16], p[512];
    struct stat st, st2;
    int i, fd, keep;
    sprintf(who, "worker %d", w->id);
    sprintf(dir, "d%d", w->id);
    keep = openat(w->dfd, "keep", O_RDWR | O_CREAT, 0644);
    if (keep < 0) {
        bad(&n_other, who, "openat keep", errno);
        keep = -1;
    }
    for (i = 0; i < iterations && !stop; i++) {
        DIR *d;
        int dfd2, seen;
        struct dirent *e;

        /* a descriptor this thread holds must stay valid */
        if (keep >= 0 && fstat(keep, &st) < 0)
            bad(errno == EBADF ? &n_ebadf : &n_other, who, "fstat of own open file", errno);

        if (fstatat(w->dfd, "f", &st, 0) < 0)
            bad(errno == ENOENT ? &n_wrongdir : &n_other, who, "fstatat f", errno);
        else if (st.st_ino != w->want_f)
            bad(&n_wrongdir, who, "fstatat f: another directory's file", 0);

        fd = openat(w->dfd, "f", O_RDONLY);
        if (fd < 0)
            bad(errno == ENOENT ? &n_wrongdir : &n_other, who, "openat f", errno);
        else {
            if (fstat(fd, &st2) < 0)
                bad(errno == EBADF ? &n_ebadf : &n_other, who, "fstat of the file just opened", errno);
            else if (st2.st_ino != w->want_f)
                bad(&n_wrongdir, who, "openat f: another directory's file", 0);
            close(fd);
        }

        /* made where it should be, and gone from there when removed */
        if (mkdirat(w->dfd, "sub", 0755) < 0)
            bad(&n_other, who, "mkdirat sub", errno);
        else {
            abs_path(p, sizeof p, dir, "sub");
            if (stat(p, &st) < 0 || !S_ISDIR(st.st_mode))
                bad(&n_wrongdir, who, "mkdirat sub: not in the directory it was asked for", errno);
            if (fstatat(w->dfd, "sub", &st, AT_SYMLINK_NOFOLLOW) < 0)
                bad(&n_other, who, "fstatat sub", errno);
            if (unlinkat(w->dfd, "sub", AT_REMOVEDIR) < 0)
                bad(errno == ENOENT ? &n_wrongdir : &n_other, who, "unlinkat sub", errno);
            else if (stat(p, &st) == 0)
                bad(&n_wrongdir, who, "unlinkat sub: still there", 0);
        }
        fd = openat(w->dfd, "tmp", O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (fd < 0)
            bad(&n_other, who, "openat O_CREAT tmp", errno);
        else {
            close(fd);
            abs_path(p, sizeof p, dir, "tmp");
            if (stat(p, &st) < 0)
                bad(&n_wrongdir, who, "openat O_CREAT tmp: not in the directory it was asked for", errno);
            if (unlinkat(w->dfd, "tmp", 0) < 0)
                bad(errno == ENOENT ? &n_wrongdir : &n_other, who, "unlinkat tmp", errno);
        }

        /* a DIR over a fresh descriptor for this directory; it must list "f" and
         * close cleanly (closedir EBADF = the descriptor had been taken over) */
        dfd2 = openat(w->dfd, ".", O_RDONLY | O_DIRECTORY);
        if (dfd2 < 0)
            bad(&n_other, who, "openat .", errno);
        else if ((d = fdopendir(dfd2)) == NULL) {
            bad(&n_other, who, "fdopendir", errno);
            close(dfd2);
        } else {
            seen = 0;
            while ((e = readdir(d)) != NULL)
                if (strcmp(e->d_name, "f") == 0)
                    seen = 1;
            if (!seen)
                bad(&n_wrongdir, who, "fdopendir listing has no f", 0);
            if (closedir(d) < 0)
                bad(errno == EBADF ? &n_ebadf : &n_other, who, "closedir", errno);
        }
        w->ops++;
    }
    if (keep >= 0)
        close(keep);
    pthread_mutex_lock(&mu);
    workers_done++;
    pthread_mutex_unlock(&mu);
    return NULL;
}

static ino_t here_ino;

static void *cwd_main(void *arg)
{
    char p[512];
    struct stat st;
    int fd, k = 0;
    long loops = 0;
    (void)arg;
    snprintf(p, sizeof p, "%s/cwd/cwdtmp", root);
    while (!stop && workers_done < nworkers) {
        fd = open("here", O_RDONLY);
        if (fd < 0)
            bad(errno == ENOENT ? &n_cwdseen : &n_other, "cwd thread", "open here", errno);
        else {
            if (fstat(fd, &st) < 0)
                bad(errno == EBADF ? &n_ebadf : &n_other, "cwd thread", "fstat of the file just opened", errno);
            else if (st.st_ino != here_ino)
                bad(&n_cwdseen, "cwd thread", "open here: another file", 0);
            close(fd);
        }
        if (stat("here", &st) < 0)
            bad(errno == ENOENT ? &n_cwdseen : &n_other, "cwd thread", "stat here", errno);
        if ((k++ & 3) == 0) {
            fd = open("cwdtmp", O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (fd < 0)
                bad(&n_other, "cwd thread", "create cwdtmp", errno);
            else {
                close(fd);
                if (stat(p, &st) < 0)
                    bad(&n_cwdseen, "cwd thread", "cwdtmp did not land in the working directory", errno);
                if (unlink("cwdtmp") < 0)
                    bad(errno == ENOENT ? &n_cwdseen : &n_other, "cwd thread", "unlink cwdtmp", errno);
            }
        }
        loops++;
    }
    printf("# cwd thread: %ld rounds\n", loops);
    return NULL;
}

static void cleanup(void)
{
    char cmd[400];
    sprintf(cmd, "rm -rf %s", root);
    system(cmd);
}

int main(int argc, char **argv)
{
    pthread_t tw[MAXW], tc;
    struct worker w[MAXW];
    char p[512], dir[16];
    struct stat st;
    long ops = 0, total;
    int i, c;

    sprintf(root, "/var/tmp/atfile-threads.%ld", (long)getpid());
    while ((c = getopt(argc, argv, "n:w:")) != -1) {
        if (c == 'n') iterations = atoi(optarg);
        else if (c == 'w') nworkers = atoi(optarg);
        else return 2;
    }
    if (optind < argc)
        snprintf(root, sizeof root, "%s/atfile-threads.%ld", argv[optind], (long)getpid());
    if (nworkers < 1 || nworkers > MAXW) nworkers = 4;

    cleanup();
    if (mkdir(root, 0755) < 0) { perror(root); return 2; }
    for (i = 0; i < nworkers; i++) {
        FILE *f;
        sprintf(dir, "d%d", i);
        snprintf(p, sizeof p, "%s/%s", root, dir);
        if (mkdir(p, 0755) < 0) { perror(p); return 2; }
        abs_path(p, sizeof p, dir, "f");
        if ((f = fopen(p, "w")) == NULL) { perror(p); return 2; }
        fputs("x\n", f); fclose(f);
        if (stat(p, &st) < 0) { perror(p); return 2; }
        w[i].want_f = st.st_ino;
        w[i].id = i;
        w[i].ops = 0;
    }
    snprintf(p, sizeof p, "%s/cwd", root);
    if (mkdir(p, 0755) < 0) { perror(p); return 2; }
    snprintf(p, sizeof p, "%s/cwd/here", root);
    {
        FILE *f = fopen(p, "w");
        if (f == NULL) { perror(p); return 2; }
        fputs("here\n", f); fclose(f);
    }
    if (stat(p, &st) < 0) { perror(p); return 2; }
    here_ino = st.st_ino;

    /* Directory descriptors by plain open(), which the emulation did not see
     * (so its first use of each reads the path), then used once so that what
     * follows is the steady state. */
    for (i = 0; i < nworkers; i++) {
        snprintf(p, sizeof p, "%s/d%d", root, i);
        if (i & 1) {
            w[i].dfd = open(p, O_RDONLY);
        } else {
            /* the other half through openat, which the emulation records */
            int rfd = open(root, O_RDONLY);
            sprintf(dir, "d%d", i);
            w[i].dfd = rfd < 0 ? -1 : openat(rfd, dir, O_RDONLY | O_DIRECTORY);
            if (rfd >= 0) close(rfd);
        }
        if (w[i].dfd < 0) { perror(p); return 2; }
        if (fstatat(w[i].dfd, "f", &st, 0) < 0) { perror("warm-up fstatat"); return 2; }
    }
    snprintf(p, sizeof p, "%s/cwd", root);
    if (chdir(p) < 0) { perror(p); return 2; }

    printf("# %d workers x %d iterations, a cwd thread beside them\n", nworkers, iterations);
    pthread_create(&tc, NULL, cwd_main, NULL);
    for (i = 0; i < nworkers; i++)
        pthread_create(&tw[i], NULL, worker_main, &w[i]);
    for (i = 0; i < nworkers; i++) {
        pthread_join(tw[i], NULL);
        ops += w[i].ops;
    }
    pthread_join(tc, NULL);
    for (i = 0; i < nworkers; i++) close(w[i].dfd);
    chdir("/");
    cleanup();

    total = n_wrongdir + n_cwdseen + n_ebadf + n_other;
    printf("# %ld worker iterations done; wrongdir=%ld cwdseen=%ld ebadf=%ld other=%ld\n",
           ops, n_wrongdir, n_cwdseen, n_ebadf, n_other);
    if (total == 0 && ops == (long)nworkers * iterations)
        puts("PASS atfile-threads: no wrong directory, no EBADF");
    else
        printf("FAIL atfile-threads: %ld problems\n", total);
    return total != 0 || ops != (long)nworkers * iterations;
}
