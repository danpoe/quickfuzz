#include "hash_table.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void ht_init(ht_t *ht, ht_config_t c)
{
  ht->hash_el = c.hash_el;
  ht->equal_el = c.equal_el;
  ht->elems = 0;
  memset(ht->ht, 0, sizeof(ht->ht));
}

void ht_clear(ht_t *ht)
{
  memset(ht->ht, 0, sizeof(ht->ht));
  ht->elems = 0;
}

int ht_get_elems(ht_t *ht)
{
  return ht->elems;
}

int ht_insert(ht_t *ht, void *key, void *val)
{
  if (ht->elems >= HT_SZ) {
    return -2; // No space (and key possibly exists)
  }

  int pos = ht->hash_el(key) % HT_SZ;
  assert(pos >= 0);

  while (1) {
    ht_entry_t *e = ht->ht + pos;
    if (e->taken) {
      assert(!e->deleted);
      if (ht->equal_el(key, e->key)) {
        return -1; // Key exists
      }
    } else {
      e->key = key;
      e->val = val;
      e->taken = 1;
      e->deleted = 0;
      ht->elems += 1;
      return 0; // Inserted
    }
    pos = (pos + 1) % HT_SZ;
  }
}

int ht_find(ht_t *ht, void *key, ht_entry_t **hte)
{
  int pos = ht->hash_el(key) % HT_SZ;
  assert(pos >= 0);

  int i;
  for (i = 0; i < HT_SZ; i++) {
    ht_entry_t *e = ht->ht + pos;
    if (e->taken) {
      assert(!e->deleted);
      if (ht->equal_el(key, e->key)) {
        assert(hte != NULL);
        *hte = e;
        return 0;
      }
    } else if (!e->deleted) {
      // Not found
      return -1;
    }
    pos = (pos + 1) % HT_SZ;
  }

  // Not found
  return -1;
}

int ht_delete(ht_t *ht, void *key)
{
  ht_entry_t *e;
  int ret;
  ret = ht_find(ht, key, &e);
  if (ret == 0) {
    e->taken = 0;
    e->deleted = 1;
    ht->elems -= 1;
    return 0; // Deleted
  } else {
    assert(ret == -1);
    return -1; // Not found
  } 
}