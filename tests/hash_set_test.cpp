#include <catch2/catch.hpp>

extern "C"
{
#include "../hash_set.h"
}

namespace
{

int equal_int(const void *el1, const void *el2)
{
  return *((int *)el1) == *((int *)el2);
}

unsigned long hash_int(const void *el)
{
  return *((int *)el);
}
} // anonymous namespace

TEST_CASE("Hash set")
{
  int val1 = 0;
  int val2 = 1;

  hs_config_t config;
  config.equal_el = equal_int;
  config.hash_el = hash_int;

  hs_t *hs = hs_allocate(10);
  hs_init(hs, config);
  REQUIRE(!hs_test(hs, &val1));

  hs_insert(hs, &val1);
  REQUIRE(hs_test(hs, &val1));
  REQUIRE(!hs_test(hs, &val2));

  hs_destroy(hs);
}
