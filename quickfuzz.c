#include "quickfuzz.h"

#include "hash_set.h"
#include "hash_table.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include <unistd.h>
#include <pthread.h>

static pthread_mutex_t qfi_mutex = PTHREAD_MUTEX_INITIALIZER;

#define qfi_lock() pthread_mutex_lock(&qfi_mutex)
#define qfi_unlock() pthread_mutex_unlock(&qfi_mutex)

// Check for internal error or usage error
#define qfi_chk(cond) assert(cond)

static hs_t hs;

// -----------------------------------------------------------------------------
// Two-way map (between symbolic thread names and thread IDs)

// Hash tables that make up two-way map
static ht_t ht1;
static ht_t ht2;
static int elems = 0;

static void init(ht_config_t c1, ht_config_t c2)
{
  ht_init(&ht1, c1);
  ht_init(&ht2, c2);
  elems = 0;
}

static int map(void *el1, void *el2)
{
  if (elems >= ht_get_size(&ht1)) {
    return -2; // No space (and a component may exist already)
  }

  // Trial inserts
  int ret1;
  int ret2;
  ret1 = ht_insert(&ht1, el1, el2);
  ret2 = ht_insert(&ht2, el2, el1);
  assert(ret1 != -2 && ret2 != -2);
  
  int ret;

  if (ret1 == 0 && ret2 == -1) {
    ret = ht_delete(&ht1, el1);
    assert(ret == 0);
    return -1; // Component exists
  }

  if (ret1 == -1 && ret2 == 0) {
    ret = ht_delete(&ht2, el2);
    assert(ret == 0);
    return -1; // Component exists
  }

  elems++;

  return 0; // Inserted
}

static int find_el1(void *el1, void **el2)
{
  ht_entry_t *e;
  int ret;
  ret = ht_find(&ht1, el1, &e);
  if (ret == 0) {
    assert(el2 != NULL);
    *el2 = e->val;
  }
  return ret;
}

static int find_el2(void *el2, void **el1)
{
  ht_entry_t *e;
  int ret;
  ret = ht_find(&ht2, el2, &e);
  if (ret == 0) {
    assert(el1 != NULL);
    *el1 = e->val;
  }
  return ret;
}

static void delete_el1(void *el1)
{
  void *el2;
  int ret;
  ret = find_el1(el1, &el2);
  if (ret == 0) {
    ret = ht_delete(&ht2, el2);
    assert(ret == 0);
    ret = ht_delete(&ht1, el1);
    assert(ret == 0);
    elems--;
  }
}

static void delete_el2(void *el2)
{
  void *el1;
  int ret;
  ret = find_el2(el2, &el1);
  if (ret == 0) {
    ret = ht_delete(&ht1, el1);
    assert(ret == 0);
    ret = ht_delete(&ht2, el2);
    assert(ret == 0);
    elems--;
  }
}

static void clear()
{
  ht_clear(&ht1);
  ht_clear(&ht2);
  elems = 0;
}

static int get_elems()
{
  return elems;
}

static int get_capacity()
{
  return ht_get_size(&ht1);
}

// -----------------------------------------------------------------------------
// Hash and equality functions

unsigned long hash_tid(const void *v)
{
  return (long)v;
}

int equal_tid(const void *v1, const void *v2)
{
  return v1 == v2;
}

// -----------------------------------------------------------------------------
// Symbolic thread IDs

// Prepare for a call to qfi_make_map() or qfi_clear_map()
void qfi_prepare_map()
{
  qfi_lock();
}

// Mapping for key must not exist. Old mapping for tid is overwritten.
void qfi_make_map(char *key, pthread_t tid)
{
  int ret;
  void *out;

  ret = find_el1(key, &out);
  qfi_chk(ret == -1);

  delete_el2((void *)tid);
  ret = map(key, (void *)tid);
  qfi_chk(ret == 0);

  qfi_unlock();
}

// Remove mapping for tid that might exist.
void qfi_clear_map(pthread_t tid)
{
  delete_el2((void *)tid);
  qfi_unlock();
}

void qfi_let_map()
{
  qfi_lock();
  qfi_unlock();
}

int qfi_chk_sym(char *key)
{
  int ret;
  void *out;
  pthread_t tid;
  qfi_lock();
  ret = find_el1(key, &out);
  qfi_unlock();
  if (ret == -1) {
    return 0;
  }
  tid = pthread_self();
  if ((void *)tid == out) {
    return 1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// Other

// Busy wait check interval in microseconds
static const int qfi_yield = 1000;

// Time (in microseconds) after which wait continues
static int qfi_wait_time = INT_MAX;

// Can be called more than once during an execution
void qfi_init(int t)
{
  qfi_chk(t >= 0);
  qfi_chk(sizeof(pthread_t) == sizeof(void *));

  qfi_wait_time = t;

  ht_config_t c1;
  c1.hash_el = hash_str;
  c1.equal_el = equal_str;
  ht_config_t c2;
  c2.hash_el = hash_tid;
  c2.equal_el = equal_tid;

  init(c1, c2);

  ht_config_t c;
  c.hash_el = hash_str;
  c.equal_el = equal_str;

  hs_init(&hs, c);
}

void qfi_print(char *str)
{
  printf("%s\n", str);
}

// -----------------------------------------------------------------------------
// Wait/Signal

void qfi_wait(char *sym)
{
  int c;
  int i;
  for (i = 0; i * qfi_yield < qfi_wait_time; i++) {
    qfi_lock();
    c = hs_test(&hs, sym);
    qfi_unlock();
    if (c) {
      break;
    }
    usleep(qfi_yield); 
  }
}

void qfi_signal(char *sym)
{
  int ret;
  qfi_lock();
  ret = hs_insert(&hs, sym);
  assert(ret == 0 || ret == -1 || ret == -2);
  qfi_unlock();
}
