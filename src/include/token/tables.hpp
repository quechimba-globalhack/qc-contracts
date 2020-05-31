#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

namespace eosiosystem {
class system_contract;
}

namespace qctoken {

using eosio::asset;
using eosio::name;

struct [[eosio::table, eosio::contract("qctoken")]] account {
  asset balance;

  uint64_t primary_key() const { return balance.symbol.code().raw(); }
};

struct [[eosio::table, eosio::contract("qctoken")]] currency_stats {
  asset supply;
  asset max_supply;
  name  issuer;

  uint64_t primary_key() const { return supply.symbol.code().raw(); }
};

typedef eosio::multi_index<"accounts"_n, account>    accounts;
typedef eosio::multi_index<"stat"_n, currency_stats> stats;
}  // namespace qctoken
