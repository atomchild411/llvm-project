/*===---- sys/queue.h - BSD list macros for IRIX -----------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * The BSDs' and glibc's intrusive list macros, which IRIX does not ship and
 * many programs include rather than bring their own:
 *
 *   SLIST    singly linked list
 *   LIST     doubly linked list
 *   SIMPLEQ  singly linked tail queue (NetBSD, OpenBSD)
 *   STAILQ   the same, under FreeBSD's name
 *   TAILQ    doubly linked tail queue
 *
 * with the *_FOREACH_SAFE forms. A tail queue's head and an element's entry
 * share a layout (first or next, then a pointer to the last or previous
 * link), which TAILQ_LAST and TAILQ_PREV depend on, as on the BSDs.
 */

#ifndef __CLANG_IRIX_SYS_QUEUE_H
#define __CLANG_IRIX_SYS_QUEUE_H

#include <stddef.h>

/* Singly linked list. */

#define SLIST_HEAD(name, type)                                                 \
  struct name {                                                                \
    struct type *slh_first;                                                    \
  }
#define SLIST_HEAD_INITIALIZER(head) {NULL}
#define SLIST_ENTRY(type)                                                      \
  struct {                                                                     \
    struct type *sle_next;                                                     \
  }

#define SLIST_FIRST(head) ((head)->slh_first)
#define SLIST_EMPTY(head) (SLIST_FIRST(head) == NULL)
#define SLIST_NEXT(elm, field) ((elm)->field.sle_next)

#define SLIST_FOREACH(var, head, field)                                        \
  for ((var) = SLIST_FIRST(head); (var) != NULL;                               \
       (var) = SLIST_NEXT(var, field))
#define SLIST_FOREACH_SAFE(var, head, field, tvar)                             \
  for ((var) = SLIST_FIRST(head);                                              \
       (var) != NULL && ((tvar) = SLIST_NEXT(var, field), 1); (var) = (tvar))

#define SLIST_INIT(head)                                                       \
  do {                                                                         \
    SLIST_FIRST(head) = NULL;                                                  \
  } while (0)
#define SLIST_INSERT_HEAD(head, elm, field)                                    \
  do {                                                                         \
    SLIST_NEXT(elm, field) = SLIST_FIRST(head);                                \
    SLIST_FIRST(head) = (elm);                                                 \
  } while (0)
#define SLIST_INSERT_AFTER(slistelm, elm, field)                               \
  do {                                                                         \
    SLIST_NEXT(elm, field) = SLIST_NEXT(slistelm, field);                      \
    SLIST_NEXT(slistelm, field) = (elm);                                       \
  } while (0)
#define SLIST_REMOVE_HEAD(head, field)                                         \
  do {                                                                         \
    SLIST_FIRST(head) = SLIST_NEXT(SLIST_FIRST(head), field);                  \
  } while (0)
#define SLIST_REMOVE_AFTER(elm, field)                                         \
  do {                                                                         \
    SLIST_NEXT(elm, field) = SLIST_NEXT(SLIST_NEXT(elm, field), field);        \
  } while (0)
#define SLIST_REMOVE(head, elm, type, field)                                   \
  do {                                                                         \
    if (SLIST_FIRST(head) == (elm)) {                                          \
      SLIST_REMOVE_HEAD(head, field);                                          \
    } else {                                                                   \
      struct type *__slist_cur = SLIST_FIRST(head);                            \
      while (SLIST_NEXT(__slist_cur, field) != (elm))                          \
        __slist_cur = SLIST_NEXT(__slist_cur, field);                          \
      SLIST_REMOVE_AFTER(__slist_cur, field);                                  \
    }                                                                          \
  } while (0)

/* Doubly linked list: le_prev points at the link that points at this
 * element (the head's lh_first or the previous element's le_next). */

#define LIST_HEAD(name, type)                                                  \
  struct name {                                                                \
    struct type *lh_first;                                                     \
  }
#define LIST_HEAD_INITIALIZER(head) {NULL}
#define LIST_ENTRY(type)                                                       \
  struct {                                                                     \
    struct type *le_next;                                                      \
    struct type **le_prev;                                                     \
  }

#define LIST_FIRST(head) ((head)->lh_first)
#define LIST_EMPTY(head) (LIST_FIRST(head) == NULL)
#define LIST_NEXT(elm, field) ((elm)->field.le_next)

#define LIST_FOREACH(var, head, field)                                         \
  for ((var) = LIST_FIRST(head); (var) != NULL; (var) = LIST_NEXT(var, field))
#define LIST_FOREACH_SAFE(var, head, field, tvar)                              \
  for ((var) = LIST_FIRST(head);                                               \
       (var) != NULL && ((tvar) = LIST_NEXT(var, field), 1); (var) = (tvar))

#define LIST_INIT(head)                                                        \
  do {                                                                         \
    LIST_FIRST(head) = NULL;                                                   \
  } while (0)
#define LIST_INSERT_HEAD(head, elm, field)                                     \
  do {                                                                         \
    if ((LIST_NEXT(elm, field) = LIST_FIRST(head)) != NULL)                    \
      LIST_FIRST(head)->field.le_prev = &LIST_NEXT(elm, field);                \
    LIST_FIRST(head) = (elm);                                                  \
    (elm)->field.le_prev = &LIST_FIRST(head);                                  \
  } while (0)
#define LIST_INSERT_AFTER(listelm, elm, field)                                 \
  do {                                                                         \
    if ((LIST_NEXT(elm, field) = LIST_NEXT(listelm, field)) != NULL)           \
      LIST_NEXT(listelm, field)->field.le_prev = &LIST_NEXT(elm, field);       \
    LIST_NEXT(listelm, field) = (elm);                                         \
    (elm)->field.le_prev = &LIST_NEXT(listelm, field);                         \
  } while (0)
#define LIST_INSERT_BEFORE(listelm, elm, field)                                \
  do {                                                                         \
    (elm)->field.le_prev = (listelm)->field.le_prev;                           \
    LIST_NEXT(elm, field) = (listelm);                                         \
    *(listelm)->field.le_prev = (elm);                                         \
    (listelm)->field.le_prev = &LIST_NEXT(elm, field);                         \
  } while (0)
#define LIST_REMOVE(elm, field)                                                \
  do {                                                                         \
    if (LIST_NEXT(elm, field) != NULL)                                         \
      LIST_NEXT(elm, field)->field.le_prev = (elm)->field.le_prev;             \
    *(elm)->field.le_prev = LIST_NEXT(elm, field);                             \
  } while (0)
#define LIST_REPLACE(elm, elm2, field)                                         \
  do {                                                                         \
    if ((LIST_NEXT(elm2, field) = LIST_NEXT(elm, field)) != NULL)              \
      LIST_NEXT(elm2, field)->field.le_prev = &LIST_NEXT(elm2, field);         \
    (elm2)->field.le_prev = (elm)->field.le_prev;                              \
    *(elm2)->field.le_prev = (elm2);                                           \
  } while (0)

/* Singly linked tail queue: stqh_last points at the last link (the head's
 * stqh_first when empty, else the last element's stqe_next). STAILQ is the
 * whole of it; SIMPLEQ is the same under the other BSDs' names. */

#define STAILQ_HEAD(name, type)                                                \
  struct name {                                                                \
    struct type *stqh_first;                                                   \
    struct type **stqh_last;                                                   \
  }
#define STAILQ_HEAD_INITIALIZER(head) {NULL, &(head).stqh_first}
#define STAILQ_ENTRY(type)                                                     \
  struct {                                                                     \
    struct type *stqe_next;                                                    \
  }

#define STAILQ_FIRST(head) ((head)->stqh_first)
#define STAILQ_EMPTY(head) (STAILQ_FIRST(head) == NULL)
#define STAILQ_NEXT(elm, field) ((elm)->field.stqe_next)
#define STAILQ_LAST(head, type, field)                                         \
  (STAILQ_EMPTY(head)                                                          \
       ? NULL                                                                  \
       : (struct type *)(void *)((char *)(head)->stqh_last -                   \
                                 offsetof(struct type, field)))

#define STAILQ_FOREACH(var, head, field)                                       \
  for ((var) = STAILQ_FIRST(head); (var) != NULL;                              \
       (var) = STAILQ_NEXT(var, field))
#define STAILQ_FOREACH_SAFE(var, head, field, tvar)                            \
  for ((var) = STAILQ_FIRST(head);                                             \
       (var) != NULL && ((tvar) = STAILQ_NEXT(var, field), 1); (var) = (tvar))

#define STAILQ_INIT(head)                                                      \
  do {                                                                         \
    STAILQ_FIRST(head) = NULL;                                                 \
    (head)->stqh_last = &STAILQ_FIRST(head);                                   \
  } while (0)
#define STAILQ_INSERT_HEAD(head, elm, field)                                   \
  do {                                                                         \
    if ((STAILQ_NEXT(elm, field) = STAILQ_FIRST(head)) == NULL)                \
      (head)->stqh_last = &STAILQ_NEXT(elm, field);                            \
    STAILQ_FIRST(head) = (elm);                                                \
  } while (0)
#define STAILQ_INSERT_TAIL(head, elm, field)                                   \
  do {                                                                         \
    STAILQ_NEXT(elm, field) = NULL;                                            \
    *(head)->stqh_last = (elm);                                                \
    (head)->stqh_last = &STAILQ_NEXT(elm, field);                              \
  } while (0)
#define STAILQ_INSERT_AFTER(head, listelm, elm, field)                         \
  do {                                                                         \
    if ((STAILQ_NEXT(elm, field) = STAILQ_NEXT(listelm, field)) == NULL)       \
      (head)->stqh_last = &STAILQ_NEXT(elm, field);                            \
    STAILQ_NEXT(listelm, field) = (elm);                                       \
  } while (0)
#define STAILQ_REMOVE_HEAD(head, field)                                        \
  do {                                                                         \
    if ((STAILQ_FIRST(head) = STAILQ_NEXT(STAILQ_FIRST(head), field)) == NULL) \
      (head)->stqh_last = &STAILQ_FIRST(head);                                 \
  } while (0)
#define STAILQ_REMOVE_AFTER(head, elm, field)                                  \
  do {                                                                         \
    if ((STAILQ_NEXT(elm, field) =                                             \
             STAILQ_NEXT(STAILQ_NEXT(elm, field), field)) == NULL)             \
      (head)->stqh_last = &STAILQ_NEXT(elm, field);                            \
  } while (0)
#define STAILQ_REMOVE(head, elm, type, field)                                  \
  do {                                                                         \
    if (STAILQ_FIRST(head) == (elm)) {                                         \
      STAILQ_REMOVE_HEAD(head, field);                                         \
    } else {                                                                   \
      struct type *__stailq_cur = STAILQ_FIRST(head);                          \
      while (STAILQ_NEXT(__stailq_cur, field) != (elm))                        \
        __stailq_cur = STAILQ_NEXT(__stailq_cur, field);                       \
      STAILQ_REMOVE_AFTER(head, __stailq_cur, field);                          \
    }                                                                          \
  } while (0)
#define STAILQ_CONCAT(head1, head2)                                            \
  do {                                                                         \
    if (!STAILQ_EMPTY(head2)) {                                                \
      *(head1)->stqh_last = STAILQ_FIRST(head2);                               \
      (head1)->stqh_last = (head2)->stqh_last;                                 \
      STAILQ_INIT(head2);                                                      \
    }                                                                          \
  } while (0)

#define SIMPLEQ_HEAD(name, type)                                               \
  struct name {                                                                \
    struct type *sqh_first;                                                    \
    struct type **sqh_last;                                                    \
  }
#define SIMPLEQ_HEAD_INITIALIZER(head) {NULL, &(head).sqh_first}
#define SIMPLEQ_ENTRY(type)                                                    \
  struct {                                                                     \
    struct type *sqe_next;                                                     \
  }

#define SIMPLEQ_FIRST(head) ((head)->sqh_first)
#define SIMPLEQ_EMPTY(head) (SIMPLEQ_FIRST(head) == NULL)
#define SIMPLEQ_NEXT(elm, field) ((elm)->field.sqe_next)
#define SIMPLEQ_LAST(head, type, field)                                        \
  (SIMPLEQ_EMPTY(head)                                                         \
       ? NULL                                                                  \
       : (struct type *)(void *)((char *)(head)->sqh_last -                    \
                                 offsetof(struct type, field)))

#define SIMPLEQ_FOREACH(var, head, field)                                      \
  for ((var) = SIMPLEQ_FIRST(head); (var) != NULL;                             \
       (var) = SIMPLEQ_NEXT(var, field))
#define SIMPLEQ_FOREACH_SAFE(var, head, field, tvar)                           \
  for ((var) = SIMPLEQ_FIRST(head);                                            \
       (var) != NULL && ((tvar) = SIMPLEQ_NEXT(var, field), 1); (var) = (tvar))

#define SIMPLEQ_INIT(head)                                                     \
  do {                                                                         \
    SIMPLEQ_FIRST(head) = NULL;                                                \
    (head)->sqh_last = &SIMPLEQ_FIRST(head);                                   \
  } while (0)
#define SIMPLEQ_INSERT_HEAD(head, elm, field)                                  \
  do {                                                                         \
    if ((SIMPLEQ_NEXT(elm, field) = SIMPLEQ_FIRST(head)) == NULL)              \
      (head)->sqh_last = &SIMPLEQ_NEXT(elm, field);                            \
    SIMPLEQ_FIRST(head) = (elm);                                               \
  } while (0)
#define SIMPLEQ_INSERT_TAIL(head, elm, field)                                  \
  do {                                                                         \
    SIMPLEQ_NEXT(elm, field) = NULL;                                           \
    *(head)->sqh_last = (elm);                                                 \
    (head)->sqh_last = &SIMPLEQ_NEXT(elm, field);                              \
  } while (0)
#define SIMPLEQ_INSERT_AFTER(head, listelm, elm, field)                        \
  do {                                                                         \
    if ((SIMPLEQ_NEXT(elm, field) = SIMPLEQ_NEXT(listelm, field)) == NULL)     \
      (head)->sqh_last = &SIMPLEQ_NEXT(elm, field);                            \
    SIMPLEQ_NEXT(listelm, field) = (elm);                                      \
  } while (0)
#define SIMPLEQ_REMOVE_HEAD(head, field)                                       \
  do {                                                                         \
    if ((SIMPLEQ_FIRST(head) = SIMPLEQ_NEXT(SIMPLEQ_FIRST(head), field)) ==    \
        NULL)                                                                  \
      (head)->sqh_last = &SIMPLEQ_FIRST(head);                                 \
  } while (0)
#define SIMPLEQ_REMOVE_AFTER(head, elm, field)                                 \
  do {                                                                         \
    if ((SIMPLEQ_NEXT(elm, field) =                                            \
             SIMPLEQ_NEXT(SIMPLEQ_NEXT(elm, field), field)) == NULL)           \
      (head)->sqh_last = &SIMPLEQ_NEXT(elm, field);                            \
  } while (0)
#define SIMPLEQ_REMOVE(head, elm, type, field)                                 \
  do {                                                                         \
    if (SIMPLEQ_FIRST(head) == (elm)) {                                        \
      SIMPLEQ_REMOVE_HEAD(head, field);                                        \
    } else {                                                                   \
      struct type *__simpleq_cur = SIMPLEQ_FIRST(head);                        \
      while (SIMPLEQ_NEXT(__simpleq_cur, field) != (elm))                      \
        __simpleq_cur = SIMPLEQ_NEXT(__simpleq_cur, field);                    \
      SIMPLEQ_REMOVE_AFTER(head, __simpleq_cur, field);                        \
    }                                                                          \
  } while (0)
#define SIMPLEQ_CONCAT(head1, head2)                                           \
  do {                                                                         \
    if (!SIMPLEQ_EMPTY(head2)) {                                               \
      *(head1)->sqh_last = SIMPLEQ_FIRST(head2);                               \
      (head1)->sqh_last = (head2)->sqh_last;                                   \
      SIMPLEQ_INIT(head2);                                                     \
    }                                                                          \
  } while (0)

/* Doubly linked tail queue: tqh_last points at the last link, tqe_prev at
 * the link that points at this element. */

#define TAILQ_HEAD(name, type)                                                 \
  struct name {                                                                \
    struct type *tqh_first;                                                    \
    struct type **tqh_last;                                                    \
  }
#define TAILQ_HEAD_INITIALIZER(head) {NULL, &(head).tqh_first}
#define TAILQ_ENTRY(type)                                                      \
  struct {                                                                     \
    struct type *tqe_next;                                                     \
    struct type **tqe_prev;                                                    \
  }

#define TAILQ_FIRST(head) ((head)->tqh_first)
#define TAILQ_EMPTY(head) (TAILQ_FIRST(head) == NULL)
#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)
/* The last link is the last element's tqe_next, inside an entry laid out as
 * a head: that "head"'s tqh_last is the element's tqe_prev, the link that
 * points at the last element. */
#define TAILQ_LAST(head, headname)                                             \
  (*(((struct headname *)(void *)((head)->tqh_last))->tqh_last))
#define TAILQ_PREV(elm, headname, field)                                       \
  (*(((struct headname *)(void *)((elm)->field.tqe_prev))->tqh_last))

#define TAILQ_FOREACH(var, head, field)                                        \
  for ((var) = TAILQ_FIRST(head); (var) != NULL;                               \
       (var) = TAILQ_NEXT(var, field))
#define TAILQ_FOREACH_SAFE(var, head, field, tvar)                             \
  for ((var) = TAILQ_FIRST(head);                                              \
       (var) != NULL && ((tvar) = TAILQ_NEXT(var, field), 1); (var) = (tvar))
#define TAILQ_FOREACH_REVERSE(var, head, headname, field)                      \
  for ((var) = TAILQ_LAST(head, headname); (var) != NULL;                      \
       (var) = TAILQ_PREV(var, headname, field))
#define TAILQ_FOREACH_REVERSE_SAFE(var, head, headname, field, tvar)           \
  for ((var) = TAILQ_LAST(head, headname);                                     \
       (var) != NULL && ((tvar) = TAILQ_PREV(var, headname, field), 1);        \
       (var) = (tvar))

#define TAILQ_INIT(head)                                                       \
  do {                                                                         \
    TAILQ_FIRST(head) = NULL;                                                  \
    (head)->tqh_last = &TAILQ_FIRST(head);                                     \
  } while (0)
#define TAILQ_INSERT_HEAD(head, elm, field)                                    \
  do {                                                                         \
    if ((TAILQ_NEXT(elm, field) = TAILQ_FIRST(head)) != NULL)                  \
      TAILQ_FIRST(head)->field.tqe_prev = &TAILQ_NEXT(elm, field);             \
    else                                                                       \
      (head)->tqh_last = &TAILQ_NEXT(elm, field);                              \
    TAILQ_FIRST(head) = (elm);                                                 \
    (elm)->field.tqe_prev = &TAILQ_FIRST(head);                                \
  } while (0)
#define TAILQ_INSERT_TAIL(head, elm, field)                                    \
  do {                                                                         \
    TAILQ_NEXT(elm, field) = NULL;                                             \
    (elm)->field.tqe_prev = (head)->tqh_last;                                  \
    *(head)->tqh_last = (elm);                                                 \
    (head)->tqh_last = &TAILQ_NEXT(elm, field);                                \
  } while (0)
#define TAILQ_INSERT_AFTER(head, listelm, elm, field)                          \
  do {                                                                         \
    if ((TAILQ_NEXT(elm, field) = TAILQ_NEXT(listelm, field)) != NULL)         \
      TAILQ_NEXT(elm, field)->field.tqe_prev = &TAILQ_NEXT(elm, field);        \
    else                                                                       \
      (head)->tqh_last = &TAILQ_NEXT(elm, field);                              \
    TAILQ_NEXT(listelm, field) = (elm);                                        \
    (elm)->field.tqe_prev = &TAILQ_NEXT(listelm, field);                       \
  } while (0)
#define TAILQ_INSERT_BEFORE(listelm, elm, field)                               \
  do {                                                                         \
    (elm)->field.tqe_prev = (listelm)->field.tqe_prev;                         \
    TAILQ_NEXT(elm, field) = (listelm);                                        \
    *(listelm)->field.tqe_prev = (elm);                                        \
    (listelm)->field.tqe_prev = &TAILQ_NEXT(elm, field);                       \
  } while (0)
#define TAILQ_REMOVE(head, elm, field)                                         \
  do {                                                                         \
    if (TAILQ_NEXT(elm, field) != NULL)                                        \
      TAILQ_NEXT(elm, field)->field.tqe_prev = (elm)->field.tqe_prev;          \
    else                                                                       \
      (head)->tqh_last = (elm)->field.tqe_prev;                                \
    *(elm)->field.tqe_prev = TAILQ_NEXT(elm, field);                           \
  } while (0)
#define TAILQ_REPLACE(head, elm, elm2, field)                                  \
  do {                                                                         \
    if ((TAILQ_NEXT(elm2, field) = TAILQ_NEXT(elm, field)) != NULL)            \
      TAILQ_NEXT(elm2, field)->field.tqe_prev = &TAILQ_NEXT(elm2, field);      \
    else                                                                       \
      (head)->tqh_last = &TAILQ_NEXT(elm2, field);                             \
    (elm2)->field.tqe_prev = (elm)->field.tqe_prev;                            \
    *(elm2)->field.tqe_prev = (elm2);                                          \
  } while (0)
#define TAILQ_CONCAT(head1, head2, field)                                      \
  do {                                                                         \
    if (!TAILQ_EMPTY(head2)) {                                                 \
      *(head1)->tqh_last = TAILQ_FIRST(head2);                                 \
      TAILQ_FIRST(head2)->field.tqe_prev = (head1)->tqh_last;                  \
      (head1)->tqh_last = (head2)->tqh_last;                                   \
      TAILQ_INIT(head2);                                                       \
    }                                                                          \
  } while (0)

#endif /* __CLANG_IRIX_SYS_QUEUE_H */
