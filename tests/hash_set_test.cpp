#include <catch2/catch.hpp>

extern "C"
{
#include "../hash_set.h"
}

TEST_CASE("Hash set")
{
  hs_init(0, ht_config_t());
}