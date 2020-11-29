#include <catch2/catch.hpp>

extern "C"
{
#include "../hash_table.h"
}

int equal_int(const void *el1, const void *el2)
{
  return *((int *)el1) == *((int *)el2);
}

unsigned long hash_int(const void *el)
{
  return *((int *)el);
}

TEST_CASE("Hash table")
{
  int key1 = 0;
  int key2 = 1;
  int key3 = 2;
  int value1 = 3;
  int value2 = 4;

  ht_config_t config;
  config.equal_el = equal_int;
  config.hash_el = hash_int;

  ht_entry *entry;
  int r;

  SECTION("Empty table")
  {
    ht_t *ht = ht_allocate(10);
    ht_init(ht, config);
    REQUIRE(ht_get_size(ht) == 0);

    r = ht_find(ht, &key1, &entry);
    REQUIRE(r == -1);

    r = ht_delete(ht, &key1);
    REQUIRE(r == -1);

    ht_clear(ht);
    ht_destroy(ht);
  }

  SECTION("Fill map")
  {
    ht_t *ht = ht_allocate(10);
    ht_init(ht, config);

    r = ht_insert(ht, &key1, &value1);
    REQUIRE(r == 0);
    REQUIRE(ht_get_size(ht) == 1);

    r = ht_insert(ht, &key2, &value2);
    REQUIRE(r == 0);
    REQUIRE(ht_get_size(ht) == 2);

    SECTION("Attempt to insert existing element")
    {
      r = ht_insert(ht, &key1, &value1);
      REQUIRE(r == -1);
      REQUIRE(ht_get_size(ht) == 2);
    }

    SECTION("Find elements")
    {
      r = ht_find(ht, &key1, &entry);
      REQUIRE(r == 0);
      REQUIRE(entry->key == &key1);
      REQUIRE(entry->val == &value1);

      r = ht_find(ht, &key3, &entry);
      REQUIRE(r == -1);
    }

    SECTION("Delete elements")
    {
      r = ht_delete(ht, &key1);
      REQUIRE(r == 0);
      REQUIRE(ht_get_size(ht) == 1);

      r = ht_delete(ht, &key1);
      REQUIRE(r == -1);
      REQUIRE(ht_get_size(ht) == 1);

      ht_clear(ht);
      REQUIRE(ht_get_size(ht) == 0);
    }
  }
}