#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

// Fixed size of hash table
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

void ht_init(ht_t *ht, ht_config_t c);
void ht_clear(ht_t *ht);
int ht_get_elems(ht_t *ht);
int ht_insert(ht_t *ht, void *key, void *val);
int ht_find(ht_t *ht, void *key, ht_entry_t **hte);
int ht_delete(ht_t *ht, void *key);

#endif // __HASH_TABLE_H__