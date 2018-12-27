#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define __test_cat_name1(a, b) a##b
#define __test_cat_name(a, b) __test_cat_name1(a, b)
#define __test_gen_name() \
  __test_cat_name (__test_case_, __COUNTER__)

#ifndef NDEBUG
typedef struct __test_result
{
  int value;
  int expected;
  int skipped;
  char *code;
  struct __test_result *prev;
  struct __test_result *next;
} __test_result_t;

__test_result_t *__test_results = NULL;
__test_result_t *__test_results_head = NULL;
__test_result_t *__test_result_pending = NULL;


#define __test_body(expr, expectation, skip)        \
  {                                                 \
    __test_result_t *__test_new                     \
      = malloc (sizeof (__test_result_t));          \
    __test_new->code = malloc (strlen (#expr) + 1); \
    strcpy (__test_new->code, #expr);               \
    __test_new->expected = expectation;             \
    __test_new->skipped = skip;                     \
    __test_new->prev = __test_results;              \
    if (__test_results)                             \
      __test_results->next = __test_new;            \
    else                                            \
      __test_results_head = __test_new;             \
    __test_results = __test_new;                    \
    __test_result_pending = __test_new;             \
    __test_new->value = (expr);                     \
    __test_result_pending = NULL;                   \
    printf (".");                                   \
  }                                                 \

__attribute__ ((weak)) void setup (void);
__attribute__ ((weak)) void teardown (void);

sig_atomic_t __test_segv_flag = 0;

void
__test_handler (int signum)
{
  __test_segv_flag = 1;
  exit (signum);
}

__attribute__ ((__constructor__)) void
__test_constructor (void)
{
  if (setup)
    setup ();
  signal (SIGSEGV, __test_handler);
  printf ("running");
}

__attribute__ ((__destructor__)) void
__test_destructor (void)
{
  size_t count = 0;
  size_t successed = 0;
  size_t skipped = 0;
  size_t failed = 0;
  int result;
  __test_result_t *prev = NULL;
  __test_result_t *tmp = __test_results_head;
  
  if (__test_result_pending)
    printf (" aborted:\n");
  else
    printf (" done:\n");
  while (tmp)
    {
      result = tmp->value == tmp->expected;
      count++;
      if (tmp == __test_result_pending)
        {
          failed++;
          printf (
            "  \x1b[31m✗ \"%s\" segmentation fault. aborted.\x1b[0m\n",
            __test_result_pending->code);
          __test_segv_flag = 0;
        }      
      else if (tmp->skipped)
        {
          skipped++;
          printf (
            "  \x1b[36m- \"%s\" skipped.\x1b[0m\n",
            tmp->code);
        }
      else if (result)
        {
          successed++;
          printf (
            "  \x1b[32m✓ \"%s\" expected to be true.\x1b[0m\n",
            tmp->code);
        }
      else
        {
          failed++;
          printf (
            "  \x1b[31m✗ \"%s\" expected to be false.\x1b[0m\n",
            tmp->code);
        }
      prev = tmp;
      tmp = tmp->next;
      free (prev->code);
      free (prev);
    }
  if (__test_segv_flag)
    {
      printf (
        "  \x1b[31m✗ segmentation fault at unknown location."
        " aborted.\x1b[0m\n");
    }
  printf ("summary:\n");
  if (count)
    {
      if (failed)
        {
          printf (
            "  \x1b[31m✗ %d of %d tests failed.\x1b[0m\n",
            failed, count);
          if (skipped)
            printf (
              "  \x1b[36m- %d tests skipped.\x1b[0m\n",
              skipped);
          if (successed)
            printf (
              "  \x1b[32m✓ %d tests passed.\x1b[0m\n",
              successed);
        }
      else if (skipped)
        {
          printf (
            "  \x1b[36m- %d of %d tests skipped.\x1b[0m\n",
            skipped, count);
          if (successed)
            printf (
            "  \x1b[32m✓ %d tests passed.\x1b[0m\n",
            successed);
        }
      else
        printf ("  \x1b[32m✓ all tests passed.\x1b[0m\n");
    }
  else
    printf ("  no tests conducted.\n");
  if (teardown)
    teardown ();
  if (__test_segv_flag)
    exit (SIGSEGV);
  else
    exit (failed);
}

/* APIs */
#define ok(expr) __test_body (expr, 1, 0);
#define ng(expr) __test_body (expr, 0, 0);
#define skip(expr) __test_body (expr, 1, 1);
#define unittest __attribute__ ((__constructor__)) void __test_gen_name () (void)

#else

#define ok(expr)
#define ng(expr)
#define skip(expr)
#define unittest void __test_gen_name () (void)

#endif
