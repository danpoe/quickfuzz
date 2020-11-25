#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

unsigned long hash_str(const void *v);
int equal_str(const void *v1, const void *v2);

typedef struct ht_entry {
  // key/val
  const void *key;
  void *val;
  // flags
  int taken;
  int deleted;
} ht_entry_t;

typedef struct ht {
  ht_entry_t *ht;
  size_t size;
  size_t capacity;
  unsigned long (*hash_el)(const void *);
  int (*equal_el)(const void *, const void *);
} ht_t;

typedef struct ht_config {
  unsigned long (*hash_el)(const void *);
  int (*equal_el)(const void *, const void *);
} ht_config_t;

ht_t *ht_allocate(size_t capacity);
void ht_destroy(ht_t *ht);
void ht_init(ht_t *ht, ht_config_t c);
void ht_clear(ht_t *ht);
size_t ht_get_size(ht_t *ht);
int ht_insert(ht_t *ht, const void *key, void *val);
int ht_find(ht_t *ht, const void *key, ht_entry_t **hte);
int ht_delete(ht_t *ht, const void *key);


#endif // __HASH_TABLE_H__
