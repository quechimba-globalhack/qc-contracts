#pragma once

#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/string.hpp>
#include <eosio/time.hpp>

namespace quechimba {
using Date = eosio::time_point;

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
struct [[eosio::table, eosio::contract( "qccontract" )]] globalconfig {
  uint32_t auctime      = 120;
  fibrange actfbrange   = {};
  uint64_t subactnprice = 10;
};
typedef eosio::singleton<"globalconfig"_n, globalconfig> config_t;

// TODO: Table to store superpowers allowed

// Bakan table
struct [[eosio::table, eosio::contract( "qccontract" )]] usr {
  eosio::name   usrname = {};
  eosio::string name    = "";
  eosio::string surname = "";

  auto primary_key() const { return usrname.value; }
};
typedef eosio::multi_index<"usr"_n, usr> usr_t;

// Experience table
struct [[eosio::table, eosio::contract( "qccontract" )]] exp {
  uint64_t           exp_id;
  eosio::name        bkn_id;
  eosio::checksum256 content;
  Date               start_date;
  uint32_t           places;
  uint64_t           base_val;
  Date               maxactntdate;
  uint16_t           engagement;
  uint64_t           pub_price;
  bool               sealed = false;

  uint64_t primary_key() const { return exp_id; }
  uint64_t by_bkn() const { return bkn_id.value; }  // secondary index
};
typedef eosio::multi_index<
    "exp"_n, exp,
    eosio::indexed_by<"bybkn"_n, eosio::const_mem_fun<exp, uint64_t, &exp::by_bkn>>>
    exp_t;

// Experience subscriber
struct [[eosio::table, eosio::contract( "qccontract" )]] exp_subs {
  id                     exp_id;
  id                     bkn_id;
  eosio::block_timestamp created_at;

  auto primary_key() const { return exp_id; }
  // Secondary index by exp
  auto by_bkn() const { return bkn_id; }  // secondary index
};
typedef eosio::multi_index<
    "expsubs"_n, exp_subs,
    eosio::indexed_by<
        "bybkn"_n, eosio::const_mem_fun<exp_subs, id, &exp_subs::by_bkn>>>
    exp_subs_t;

// Auction table
struct [[eosio::table, eosio::contract( "qccontract" )]] actn {
  id                     actn_id;
  id                     exp_id;
  Date                   actn_start;
  uint16_t               actn_time;  // how long the auction will last
  uint64_t               highest_bid;
  eosio::block_timestamp created_at;
  uint64_t               age;

  auto primary_key() const { return actn_id; }
  // Secondary index by exp
  auto by_exp() const { return exp_id; }  // secondary index
};

typedef eosio::multi_index<
    "actn"_n, actn,
    eosio::indexed_by<"byexp"_n, eosio::const_mem_fun<actn, id, &actn::by_exp>>>
    actn_t;

// Table to store exp by bkn (experience that have been assigned)
struct [[eosio::table, eosio::contract( "qccontract" )]] bkn_exp {
  id       bkn_id;
  id       exp_id;
  uint64_t final_price;  // Price which the experience was acquired

  auto primary_key() const { return bkn_id; }
  auto by_exp() const { return exp_id; }
};
typedef eosio::multi_index<
    "bknexp"_n, bkn_exp,
    eosio::indexed_by<
        "byexp"_n, eosio::const_mem_fun<bkn_exp, id, &bkn_exp::by_exp>>>
    bkn_exp_t;

struct [[eosio::table, eosio::contract( "qcontract" )]] bid_record {
  id                     bid_id;
  id                     bkn_id;
  id                     exp_id;
  uint64_t               price;
  uint16_t               current_fib;  // Current fibonacci number of the bakan
  eosio::block_timestamp created_at;
  // TODO: superpower optional
  auto primary_key() const { return bid_id; }
  auto by_exp() const { return exp_id; }
  auto by_bkn() const { return bkn_id; }
};

// https://eosio-cpp.readme.io/v1.1.0/docs/using-multi-index-tables
typedef eosio::multi_index<
    "bidrecord"_n, bid_record,
    eosio::indexed_by<
        "byexp"_n, eosio::const_mem_fun<bid_record, id, &bid_record::by_exp>>,
    eosio::indexed_by<
        "bybkn"_n, eosio::const_mem_fun<bid_record, id, &bid_record::by_bkn>>>
    bid_record_t;

}  // namespace quechimba
