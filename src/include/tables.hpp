#pragma once

#include <eosio/eosio.hpp>

namespace
{
  struct [[eosio::table]] bkn
  {
    eosio::name username = ""_n;
    eosio::string name;
    eosio::string surname;

    uint64_t primary_key() const { return username.value; }
  }
  typedef eosio::multi_index<"bkn"_n, bkn> bkn_t;

  struct [[eosio::table]] exp
  {

  }

} // namespace
