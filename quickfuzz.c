#include "quickfuzz.h"
#include "quickfuzz-int.h"

// -----------------------------------------------------------------------------
// Hash table

static void ht_init(ht_t *ht, ht_config_t c)
{
  ht->hash_el = c.hash_el;
  ht->equal_el = c.equal_el;
  ht->elems = 0;
  memset(ht->ht, 0, sizeof(ht->ht));
}

static void ht_clear(ht_t *ht)
{
  memset(ht->ht, 0, sizeof(ht->ht));
  ht->elems = 0;
}

static int ht_get_elems(ht_t *ht)
{
  return ht->elems;
}

static int ht_insert(ht_t *ht, void *key, void *val)
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

static int ht_find(ht_t *ht, void *key, ht_entry_t **hte)
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

static int ht_delete(ht_t *ht, void *key)
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
  if (elems >= HT_SZ) {
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
  return HT_SZ;
}

// -----------------------------------------------------------------------------
// Hash set (for storing signalled symbolic names)

static ht_t hs;

static void hs_init(ht_config_t c)
{
  ht_init(&hs, c);
}

static int hs_insert(void *el)
{
  int ret;
  ret = ht_insert(&hs, el, NULL);
  assert(ret == 0 || ret == -1 || ret == -2);
  return ret;
}

// Check if element is in set
static int hs_test(void *el)
{
  ht_entry_t *e;
  int ret;
  ret = ht_find(&hs, el, &e);
  assert(ret == 0 || ret == -1);
  return ret + 1;
}

// -----------------------------------------------------------------------------
// Hash and equality functions

unsigned long hash_tid(void *v)
{
  return (long)v;
}

int equal_tid(void *v1, void *v2)
{
  return v1 == v2;
}

// DJB2 variant
unsigned long hash_str(void *v)
{
  char *str = (char *)v;
  assert(str != NULL);

  long hash = 5381;
  char c;

  while ((c = *str)) {
    hash = ((hash << 5) + hash) + c;
    str++;
  }

  return hash;  
}

int equal_str(void *v1, void *v2)
{
  char *s1 = (char *)v1;
  char *s2 = (char *)v2;

  return strcmp(s1, s2) == 0;
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

  hs_init(c);
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
    c = hs_test(sym);
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
  ret = hs_insert(sym);
  assert(ret == 0 || ret == -1 || ret == -2);
  qfi_unlock();
}

