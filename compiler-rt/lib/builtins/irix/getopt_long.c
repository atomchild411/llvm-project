//===-- irix/getopt_long.c - getopt_long for IRIX -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// getopt_long and getopt_long_only, which IRIX lacks; clang's IRIX
// <getopt.h> declares them. They behave as GNU's and the BSDs' do:
//
//  - Non-options are moved after the options (argv is permuted), unless the
//    option string starts with '+' or POSIXLY_CORRECT is set, which stops at
//    the first non-option, or with '-', which returns each as option 1 with
//    optarg pointing at it.
//  - A ':' after that suppresses the messages and reports a missing argument
//    as ':' rather than '?'.
//  - "--name=value" and "--name value", unique prefixes of long names, and
//    optional arguments ("x::", optional_argument) only in the same word.
//  - Setting optind to 0, or optreset to 1, starts over.
//
// They share libc's optind, optarg, opterr and optopt with IRIX's getopt,
// but keep their own place inside grouped short options ("-abc"), so a
// program should not interleave the two on one argument vector.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Weak: a program that brings its own copy (a compat/ directory) keeps it,
// while still getting the rest of this file.
#pragma weak getopt_long
#pragma weak getopt_long_only


int optreset;

static const char *nextchar; // the rest of a group of short options
static int first_nonopt, last_nonopt; // the non-options skipped so far
static int initialized;

enum ordering { PERMUTE, REQUIRE_ORDER, RETURN_IN_ORDER };

// Move the skipped non-options [first_nonopt, last_nonopt) after the options
// seen since [last_nonopt, optind), keeping each block's order.
static void exchange(char **argv) {
  int bottom = first_nonopt, middle = last_nonopt, top = optind;
  while (top > middle && middle > bottom) {
    if (top - middle > middle - bottom) {
      int len = middle - bottom, i;
      for (i = 0; i < len; i++) {
        char *t = argv[bottom + i];
        argv[bottom + i] = argv[top - (middle - bottom) + i];
        argv[top - (middle - bottom) + i] = t;
      }
      top -= len;
    } else {
      int len = top - middle, i;
      for (i = 0; i < len; i++) {
        char *t = argv[bottom + i];
        argv[bottom + i] = argv[middle + i];
        argv[middle + i] = t;
      }
      bottom += len;
    }
  }
  first_nonopt += optind - last_nonopt;
  last_nonopt = optind;
}

static int is_nonoption(const char *arg) {
  return arg[0] != '-' || arg[1] == '\0';
}

// Returns the option's value, '?' or ':' on an error, or -2 when
// getopt_long_only should try a short option instead.
static int long_option(int argc, char *const argv[], const char *optstring,
                       const struct option *longopts, int *longindex,
                       int long_only, int colon, int dashes) {
  const char *name = nextchar;
  size_t namelen = strcspn(name, "=");
  const struct option *p, *found = 0;
  int index = -1, ambiguous = 0, i;

  for (p = longopts, i = 0; p->name; p++, i++) {
    if (strncmp(p->name, name, namelen) != 0)
      continue;
    if (strlen(p->name) == namelen) { // an exact match wins
      found = p;
      index = i;
      ambiguous = 0;
      break;
    }
    if (!found) {
      found = p;
      index = i;
    } else if (found->has_arg != p->has_arg || found->flag != p->flag ||
               found->val != p->val) {
      ambiguous = 1;
    }
  }

  if (ambiguous) {
    if (opterr && !colon)
      fprintf(stderr, "%s: option '%s%.*s' is ambiguous\n", argv[0],
              dashes ? "--" : "-", (int)namelen, name);
    nextchar = 0;
    optind++;
    optopt = 0;
    return '?';
  }
  if (!found) {
    if (long_only && !dashes && strchr(optstring, *name))
      return -2;
    if (opterr && !colon)
      fprintf(stderr, "%s: unrecognized option '%s%.*s'\n", argv[0],
              dashes ? "--" : "-", (int)namelen, name);
    nextchar = 0;
    optind++;
    optopt = 0;
    return '?';
  }

  optind++;
  nextchar = 0;
  if (name[namelen] == '=') {
    if (found->has_arg == no_argument) {
      if (opterr && !colon)
        fprintf(stderr, "%s: option '%s%s' doesn't allow an argument\n",
                argv[0], dashes ? "--" : "-", found->name);
      optopt = found->flag ? 0 : found->val;
      return '?';
    }
    optarg = (char *)name + namelen + 1;
  } else if (found->has_arg == required_argument) {
    if (optind >= argc) {
      if (opterr && !colon)
        fprintf(stderr, "%s: option '%s%s' requires an argument\n", argv[0],
                dashes ? "--" : "-", found->name);
      optopt = found->flag ? 0 : found->val;
      return colon ? ':' : '?';
    }
    optarg = argv[optind++];
  } else {
    optarg = 0;
  }
  if (longindex)
    *longindex = index;
  if (found->flag) {
    *found->flag = found->val;
    return 0;
  }
  return found->val;
}

static int getopt_internal(int argc, char *const argv[], const char *optstring,
                           const struct option *longopts, int *longindex,
                           int long_only) {
  enum ordering ordering = PERMUTE;
  int colon, c;
  const char *spec;

  if (optind == 0 || optreset || !initialized) {
    if (optind == 0)
      optind = 1;
    nextchar = 0;
    first_nonopt = last_nonopt = optind;
    optreset = 0;
    initialized = 1;
  }

  if (*optstring == '-') {
    ordering = RETURN_IN_ORDER;
    optstring++;
  } else if (*optstring == '+') {
    ordering = REQUIRE_ORDER;
    optstring++;
  } else if (getenv("POSIXLY_CORRECT")) {
    ordering = REQUIRE_ORDER;
  }
  colon = *optstring == ':';

  optarg = 0;
  if (!nextchar || !*nextchar) {
    // A new argument. optind may have been moved by the caller.
    if (last_nonopt > optind)
      last_nonopt = optind;
    if (first_nonopt > optind)
      first_nonopt = optind;

    if (ordering == PERMUTE) {
      if (first_nonopt != last_nonopt && last_nonopt != optind)
        exchange((char **)argv);
      else if (last_nonopt != optind)
        first_nonopt = optind;
      while (optind < argc && is_nonoption(argv[optind]))
        optind++;
      last_nonopt = optind;
    }

    // "--" ends the options; what follows is non-options.
    if (optind != argc && strcmp(argv[optind], "--") == 0) {
      optind++;
      if (first_nonopt != last_nonopt && last_nonopt != optind)
        exchange((char **)argv);
      else if (first_nonopt == last_nonopt)
        first_nonopt = optind;
      last_nonopt = argc;
      optind = argc;
    }

    if (optind == argc) {
      // Leave optind at the first non-option.
      if (first_nonopt != last_nonopt)
        optind = first_nonopt;
      return -1;
    }

    if (is_nonoption(argv[optind])) {
      if (ordering == REQUIRE_ORDER)
        return -1;
      optarg = argv[optind++];
      return 1;
    }

    if (longopts) {
      if (argv[optind][1] == '-') {
        nextchar = argv[optind] + 2;
        return long_option(argc, argv, optstring, longopts, longindex,
                           long_only, colon, 1);
      }
      // getopt_long_only: "-x" for a short option x is that option, not a
      // prefix of a long one.
      if (long_only &&
          (argv[optind][2] || !strchr(optstring, argv[optind][1]))) {
        nextchar = argv[optind] + 1;
        c = long_option(argc, argv, optstring, longopts, longindex, long_only,
                        colon, 0);
        if (c != -2)
          return c;
      }
    }
    nextchar = argv[optind] + 1;
  }

  // A short option.
  c = (unsigned char)*nextchar++;
  spec = c == ':' ? 0 : strchr(optstring, c);
  if (!*nextchar)
    optind++;
  if (!spec) {
    if (opterr && !colon)
      fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], c);
    optopt = c;
    return '?';
  }
  if (spec[1] == ':') {
    if (spec[2] == ':') { // optional: only in the same word
      if (*nextchar) {
        optarg = (char *)nextchar;
        optind++;
      }
    } else if (*nextchar) {
      optarg = (char *)nextchar;
      optind++;
    } else if (optind >= argc) {
      if (opterr && !colon)
        fprintf(stderr, "%s: option requires an argument -- '%c'\n", argv[0],
                c);
      optopt = c;
      c = colon ? ':' : '?';
    } else {
      optarg = argv[optind++];
    }
    nextchar = 0;
  }
  return c;
}

int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex) {
  return getopt_internal(argc, argv, optstring, longopts, longindex, 0);
}

int getopt_long_only(int argc, char *const argv[], const char *optstring,
                     const struct option *longopts, int *longindex) {
  return getopt_internal(argc, argv, optstring, longopts, longindex, 1);
}

#endif // defined(__sgi)
