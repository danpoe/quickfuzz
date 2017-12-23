// Internal header (only include in quickfuzz.c)

#ifndef __QUICKFUZZ_INT_H__
#define __QUICKFUZZ_INT_H__

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include <unistd.h>
#include <pthread.h>

// -----------------------------------------------------------------------------
// Hash table

// Fixed size of map
#define HT_SZ 1000

typedef struct ht_entry {
  // key/val
  void *key;
  void *val;
  // flags
  int taken;
  int deleted;
} ht_entry_t;

typedef struct ht {
  ht_entry_t ht[HT_SZ];
  int elems;
  unsigned long (*hash_el)(void *);
  int (*equal_el)(void *, void *);
} ht_t;

typedef struct ht_config {
  unsigned long (*hash_el)(void *);
  int (*equal_el)(void *, void *);
} ht_config_t;

// -----------------------------------------------------------------------------

static pthread_mutex_t qfi_mutex = PTHREAD_MUTEX_INITIALIZER;

#define qfi_lock() pthread_mutex_lock(&qfi_mutex)
#define qfi_unlock() pthread_mutex_unlock(&qfi_mutex)

// Check for internal error or usage error
#define qfi_chk(cond) assert(cond)

#endif // __QUICKFUZZ_INT_H__

