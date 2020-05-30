#pragma once

#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/singleton.hpp>
#include <eosio/string.hpp>

namespace quechimba
{
  using Date      = eosio::time_point;

  struct id {
    eosio::name author = {};
    uint32_t    time   = {};
    uint32_t    seq    = {};
  };

  struct fibrange {
    uint16_t min;
    uint16_t max;
  };

  // Config table to store global configuration for the contract
  struct [[eosio::table, eosio::contract("qccontract")]] config {
    uint32_t auctime       = 120;
    fibrange actfbrange    = {};
    uint64_t subauctprice  = 10;
  };
  typedef eosio::singleton<"globalconfig"_n, config> config_t;

  // Bakan table
  struct [[eosio::table, eosio::contract("qccontract")]] bkn
  {
    id            bkn_id   = {};
    eosio::string name     = "";
    eosio::string surname  = "";

    auto primary_key() const { return bkn_id; }
  };
  typedef eosio::multi_index<"bkn"_n, bkn> bkn_t;

  // Experience table
  struct [[eosio::table, eosio::contract("qccontract")]] exp
  {
    id exp_id = {};
    eosio::checksum256 content;
    Date start_date;
    uint32_t places;
    uint64_t base_val;
    Date maxaucttdate; 
    uint16_t engagement;
    uint64_t pub_price;
    bool sealed = false;

    auto primary_key() const { return exp_id; }
  };
  typedef eosio::multi_index<"exp"_n, exp> exp_t;

  struct [[eosio::table, eosio::contract("qccontract")]] exp_subs {
    id exp_id;
    id bkn_id;
    eosio::block_timestamp created_at;
  };

} // namespace
