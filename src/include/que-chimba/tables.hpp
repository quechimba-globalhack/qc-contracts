#pragma once

#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/string.hpp>
#include <eosio/time.hpp>

using eosio::asset;
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
  uint32_t actntime     = 120;  // Default auction time in minutes
  fibrange actnbrange   = {};   // Fibonacci range to handle bids
  uint64_t subactnprice = 10;   // Default auction price in lks
  uint8_t  startexpdays = 90;   // Min days from now to start the experience
  uint8_t  maxstartactn = 60;   // Max days to start the auction (Exp Start Date -
                                // maxstartctn = Max Date to start the auction)
  uint8_t engpct = 110;         // Percentage engagement. NumCupos * engpct
};
typedef eosio::singleton<"globalconfig"_n, globalconfig> config_t;

// Table to store superpowers allowed
struct [[eosio::table, eosio::contract( "qccontract" )]] booster {
  enum booster_type { extra_bid = 0x0, time_machine = 0x01, midas_hand = 0x02 };
  uint64_t    id;
  uint64_t    boost_id;
  std::string description;

  uint64_t primary_key() const { return id; }
  auto     by_boost_id() const { return boost_id; }
};
typedef eosio::multi_index<
    "booster"_n, booster,
    eosio::indexed_by<
        "byboost"_n, const_mem_fun<booster, uint64_t, &booster::by_boost_id>>>
    actnbooster_t;

// Bakan table
struct [[eosio::table, eosio::contract( "qccontract" )]] usr {
  enum role_type { bakan = 0x0, agency = 0x01 };
  name          usrname = {};
  eosio::string name    = "";
  eosio::string surname = "";
  uint8_t       role;

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
  asset    base_val;  // Start auction price
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
  uint64_t exp_sub_id;
  uint64_t exp_id;
  name     bkn_id;
  Date     created_at;
  // Indices
  uint64_t  primary_key() const { return exp_sub_id; }
  uint64_t  by_exp() const { return exp_id; }
  uint128_t by_exp_bkn() const { return _by_exp_bkn( exp_id, bkn_id ); }

  static uint128_t _by_exp_bkn( uint64_t expid, name bknid ) {
    // Left Shift 64 bits expId, then paply bitwise OR operator with bknid
    return ( uint128_t{expid} << 64 ) | bknid.value;
  }

};  // namespace quechimba

typedef eosio::multi_index<
    "expsubs"_n, exp_subs,
    eosio::indexed_by<
        "byexp"_n, const_mem_fun<exp_subs, uint64_t, &exp_subs::by_exp>>,
    eosio::indexed_by<
        "byexpbkn"_n, const_mem_fun<exp_subs, uint128_t, &exp_subs::by_exp_bkn>>>
    exp_subs_t;

// Auction table
struct [[eosio::table, eosio::contract( "qccontract" )]] actn {
  uint64_t actn_id;
  uint64_t exp_id;
  Date     start_date;
  uint16_t duration;     // Duration in mins
  asset    start_value;  // Start auction value, the same value is in experience
  asset    highest_bid;  // Current highest bid
  name     bkn;          // Bakan with the highest
  uint64_t counter;
  bool     req_cancel = false;
  Date     created_at;

  auto primary_key() const { return actn_id; }
  // Secondary index by exp
  auto by_exp() const { return exp_id; }  // secondary index
};

typedef eosio::multi_index<
    "actn"_n, actn,
    eosio::indexed_by<"byexp"_n, const_mem_fun<actn, uint64_t, &actn::by_exp>>>
    actn_t;

// Table to store exp by bkn (experience that have been assigned)
struct [[eosio::table, eosio::contract( "qccontract" )]] bkn_exp {
  uint64_t bkn_exp_id;
  uint64_t bkn_id;
  uint64_t exp_id;
  asset    final_price;  // Price which the experience was acquired

  auto primary_key() const { return bkn_id; }
  auto by_exp() const { return exp_id; }
};
typedef eosio::multi_index<
    "bknexp"_n, bkn_exp,
    eosio::indexed_by<"byexp"_n, const_mem_fun<bkn_exp, uint64_t, &bkn_exp::by_exp>>>
    bkn_exp_t;

struct [[eosio::table, eosio::contract( "qccontract" )]] bid_record {
  uint64_t bid_id;
  uint64_t bkn_id;
  uint64_t actn_id;
  asset    price;
  uint16_t current_fib;  // Current fibonacci number of the bakan
  Date     created_at;
  uint64_t booster_id;  // Booster optional
  uint64_t primary_key() const { return bid_id; }
  uint64_t by_actn() const { return actn_id; }
  uint64_t by_bkn() const { return bkn_id; }
};

// https://eosio-cpp.readme.io/v1.1.0/docs/using-multi-index-tables
typedef eosio::multi_index<
    "bidrecord"_n, bid_record,
    eosio::indexed_by<
        "byactn"_n, const_mem_fun<bid_record, uint64_t, &bid_record::by_actn>>,
    eosio::indexed_by<
        "bybkn"_n, const_mem_fun<bid_record, uint64_t, &bid_record::by_bkn>>>
    bid_record_t;

}  // namespace quechimba
