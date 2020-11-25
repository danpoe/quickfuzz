#include "hash_table.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ht_t *ht_allocate(size_t capacity)
{
  ht_t *p = (ht_t *)malloc(sizeof(ht_t));
  p->capacity = capacity;
  p->size = 0;
  p->equal_el = NULL;
  p->hash_el = NULL;
  p->ht = (ht_t *)malloc(capacity * sizeof(ht_entry_t));
  return p;
}

void ht_destroy(ht_t *ht)
{
  free(ht->ht);
  free(ht);
}

void ht_init(ht_t *ht, ht_config_t c)
{
  ht->hash_el = c.hash_el;
  ht->equal_el = c.equal_el;
  ht->size = 0;
  memset(ht->ht, 0, ht->capacity * sizeof(ht_entry_t));
}

void ht_clear(ht_t *ht)
{
  memset(ht->ht, 0, ht->capacity * sizeof(ht_entry_t));
  ht->size = 0;
}

size_t ht_get_size(ht_t *ht)
{
  return ht->size;
}

int ht_insert(ht_t *ht, void *key, void *val)
{
  if (ht->size >= ht->capacity) {
    return -2; // No space (and key possibly exists)
  }

  int pos = ht->hash_el(key) % ht->capacity;
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
      ht->size += 1;
      return 0; // Inserted
    }
    pos = (pos + 1) % ht->capacity;
  }
}

int ht_find(ht_t *ht, void *key, ht_entry_t **hte)
{
  int pos = ht->hash_el(key) % ht->capacity;
  assert(pos >= 0);

  int i;
  for (i = 0; i < ht->capacity; i++) {
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
    pos = (pos + 1) % ht->capacity;
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
    ht->size -= 1;
    return 0; // Deleted
  } else {
    assert(ret == -1);
    return -1; // Not found
  } 
}
