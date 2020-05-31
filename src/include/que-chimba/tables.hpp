#pragma once

#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/string.hpp>
#include <eosio/time.hpp>

using eosio::const_mem_fun;
using eosio::name;

namespace quechimba {
// Semantic types of quechimba
using Date = eosio::time_point_sec;
using Hash = eosio::checksum256;

struct id {
  name     author = {};
  uint32_t time   = {};
  uint32_t seq    = {};
};

struct fibrange {
  uint16_t min;
  uint16_t max;
};

// Config table to store global configuration for the contract
struct [[eosio::table, eosio::contract( "qccontract" )]] globalconfig {
  uint32_t auctime      = 120;  // Default auction time in minutes
  fibrange actfbrange   = {};   // Fibonacci range to handle bids
  uint64_t subactnprice = 10;   // Default auction price in lks
  uint8_t  startexpdays = 90;   // Min days from now to start the experience
  uint8_t  maxstartctn  = 60;   // Max days to start the auction (Exp Start Date -
                                // maxstartctn = Max Date to start the auction)
};
typedef eosio::singleton<"globalconfig"_n, globalconfig> config_t;

// TODO: Table to store superpowers allowed

// Bakan table
struct [[eosio::table, eosio::contract( "qccontract" )]] usr {
  enum rol_type { bakan = 0x0, agency = 0x01 };
  name          usrname = {};
  eosio::string name    = "";
  eosio::string surname = "";
  uint8_t       rol;

  auto primary_key() const { return usrname.value; }
};
typedef eosio::multi_index<"usr"_n, usr> usr_t;

// Experience table
struct [[eosio::table, eosio::contract( "qccontract" )]] exp {
  uint64_t exp_id;
  name     bkn_id;
  Hash     content;
  Date     start_date;
  uint32_t places;
  uint64_t base_val;
  Date     maxactntdate;
  uint16_t engagement = 0;
  uint64_t pub_price;
  bool     sealed = false;

  uint64_t primary_key() const { return exp_id; }
  uint64_t by_bkn() const { return bkn_id.value; }  // secondary index
};
typedef eosio::multi_index<
    "exp"_n, exp,
    eosio::indexed_by<"bybkn"_n, const_mem_fun<exp, uint64_t, &exp::by_bkn>>>
    exp_t;

// Experience subscriber
struct [[eosio::table, eosio::contract( "qccontract" )]] exp_subs {
  id   exp_id;
  id   bkn_id;
  Date created_at;

  auto primary_key() const { return exp_id; }
  // Secondary index by exp
  auto by_bkn() const { return bkn_id; }  // secondary index
};
typedef eosio::multi_index<
    "expsubs"_n, exp_subs,
    eosio::indexed_by<"bybkn"_n, const_mem_fun<exp_subs, id, &exp_subs::by_bkn>>>
    exp_subs_t;

// Auction table
struct [[eosio::table, eosio::contract( "qccontract" )]] actn {
  id       actn_id;
  id       exp_id;
  Date     actn_start;
  uint16_t actn_time;  // how long the auction will last
  uint64_t highest_bid;
  Date     created_at;
  uint64_t age;

  auto primary_key() const { return actn_id; }
  // Secondary index by exp
  auto by_exp() const { return exp_id; }  // secondary index
};

typedef eosio::multi_index<
    "actn"_n, actn,
    eosio::indexed_by<"byexp"_n, const_mem_fun<actn, id, &actn::by_exp>>>
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
    eosio::indexed_by<"byexp"_n, const_mem_fun<bkn_exp, id, &bkn_exp::by_exp>>>
    bkn_exp_t;

struct [[eosio::table, eosio::contract( "qcontract" )]] bid_record {
  id       bid_id;
  id       bkn_id;
  id       exp_id;
  uint64_t price;
  uint16_t current_fib;  // Current fibonacci number of the bakan
  Date     created_at;
  // TODO: superpower optional
  auto primary_key() const { return bid_id; }
  auto by_exp() const { return exp_id; }
  auto by_bkn() const { return bkn_id; }
};

// https://eosio-cpp.readme.io/v1.1.0/docs/using-multi-index-tables
typedef eosio::multi_index<
    "bidrecord"_n, bid_record,
    eosio::indexed_by<"byexp"_n, const_mem_fun<bid_record, id, &bid_record::by_exp>>,
    eosio::indexed_by<"bybkn"_n, const_mem_fun<bid_record, id, &bid_record::by_bkn>>>
    bid_record_t;

}  // namespace quechimba
