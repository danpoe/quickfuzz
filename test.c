#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include "quickfuzz.h"

// -----------------------------------------------------------------------------
// Basic test

void *thread(void *arg)
{
  qf_let_map();

  qf_print("First");
  qf_print("Second");

  qf_print_c(0, "One");
  qf_print_c(1, "Two");

  qf_print_k(NAME1, "Hey");
  qf_print_k(NAME2, "Hola");

  qf_print_kc(NAME1, 1, "Yo");
  qf_print_kc(NAME1, 0, "Hihi");
  qf_print_kc(NAME2, 1, "Yum");
  qf_print_kc(NAME2, 0, "No");

  qf_code(printf("X\n"));
  qf_code_k(NAME1, printf("Y\n"));

  return NULL;
}

// -----------------------------------------------------------------------------
// Signal/Wait test

void *thread_ws_1(void *arg)
{
  qf_let_map();

  qf_wait(A);
  printf("a\n");

  qf_signal(B);
  qf_wait(C);
  printf("c\n");

  qf_signal(D);

  return NULL;
}

void *thread_ws_2(void *arg)
{
  qf_let_map();

  qf_wait(B);
  printf("b\n");
  
  qf_signal(C);
  qf_wait(D);
  printf("d\n");

  return NULL;
}

// -----------------------------------------------------------------------------

void *thread_ws(void *arg)
{
  qf_let_map();

  printf("a\n");

  qf_signal_k(TH1, A);
  qf_wait_k(TH1, B);

  qf_signal_k(TH2, B);
  qf_wait_k(TH2, A);

  printf("b\n");

  return NULL;
}

// -----------------------------------------------------------------------------

int main()
{
  pthread_t t1, t2;

  printf("----------------\n");
  printf("BASIC TEST\n");
  printf("----------------\n");

  qf_init(INT_MAX);
  
  qf_prepare_map();
  pthread_create(&t1, NULL, thread, NULL);
  qf_make_map(NAME1, t1);
  
  usleep(1000);
  printf("----\n");

  qf_prepare_map();
  pthread_create(&t2, NULL, thread, NULL);
  qf_clear_map(t2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("------------------\n");
  printf("SIGNAL/WAIT TEST 1\n");
  printf("------------------\n");

  qf_init(INT_MAX);

  qf_prepare_map();
  pthread_create(&t1, NULL, thread_ws_1, NULL);
  qf_make_map(TH1, t1);

  qf_prepare_map();
  pthread_create(&t2, NULL, thread_ws_2, NULL);
  qf_make_map(TH2, t2);

  qf_signal(A);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("------------------\n");
  printf("SIGNAL/WAIT TEST 2\n");
  printf("------------------\n");

  qf_init(INT_MAX);

  qf_prepare_map();
  pthread_create(&t1, NULL, thread_ws, NULL);
  qf_make_map(TH1, t1);

  qf_prepare_map();
  pthread_create(&t2, NULL, thread_ws, NULL);
  qf_make_map(TH2, t2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("---------------\n");
  printf("ASSERTION TESTS\n");
  printf("---------------\n");

  //qf_assert(0);
  qf_precon(0);

  return 0;
}

