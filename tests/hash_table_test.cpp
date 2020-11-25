#include <catch2/catch.hpp>

extern "C"
{
#include "../hash_table.h"
}

TEST_CASE("Hash table")
{
  ht_init(0, ht_config_t());
}