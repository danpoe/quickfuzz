#include "hash_set.h"

#include <assert.h>
#include <stdlib.h>

void hs_init(hs_t *hs, ht_config_t c)
{
  ht_init(hs, c);
}

int hs_insert(hs_t *hs, void *el)
{
  int ret;
  ret = ht_insert(hs, el, NULL);
  assert(ret == 0 || ret == -1 || ret == -2);
  return ret;
}

// Check if element is in set
int hs_test(hs_t *hs, void *el)
{
  ht_entry_t *e;
  int ret;
  ret = ht_find(hs, el, &e);
  assert(ret == 0 || ret == -1);
  return ret + 1;
}