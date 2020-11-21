#include "hash_table.h"

typedef ht_t hs_t;

void hs_init(hs_t *hs, ht_config_t c);
int hs_insert(hs_t *hs, void *el);
int hs_test(hs_t *hs, void *el);