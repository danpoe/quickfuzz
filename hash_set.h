#include "hash_table.h"

typedef ht_t hs_t;
typedef ht_config_t hs_config_t;

hs_t *hs_allocate(size_t capacity);
void hs_destroy(hs_t *hs);
void hs_init(hs_t *hs, hs_config_t c);
int hs_insert(hs_t *hs, void *el);
int hs_test(hs_t *hs, void *el);