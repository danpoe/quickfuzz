// Internal header (only include in quickfuzz.c)

#ifndef __QUICKFUZZ_INT_H__
#define __QUICKFUZZ_INT_H__

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

#endif // __QUICKFUZZ_INT_H__
