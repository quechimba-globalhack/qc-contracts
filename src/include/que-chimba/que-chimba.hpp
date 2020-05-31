#pragma once

#include <eosio/eosio.hpp>

#include "../constants/global.hpp"
#include "./tables.hpp"

using eosio::check;
using eosio::current_time_point;
using eosio::datastream;
using eosio::days;
using eosio::string;
using eosio::time_point_sec;

namespace quechimba {
class [[eosio::contract( "qccontract" )]] qccontract : public eosio::contract {
 public:
  qccontract( const name self, const name receiver, datastream<const char*> ds )
      : eosio::contract( self, receiver, ds ),
        config( _self, _self.value ),
        usr( self, self.value ),
        exp( self, self.value ),
        exp_subs( self, self.value ),
        actn( self, self.value ),
        bkn_exp( self, self.value ),
        bid_record( self, self.value ),
        actnbooster( self, self.value ) {}
  // First action called in the contract to inialize some data
  [[eosio::action]] void init();
  // experience publish
  [[eosio::action]] void expublish(
      const name owner, const Hash& content, const Date& start_exp,
      const uint32_t& places, const uint64_t& baseprice );

  // auction start
  [[eosio::action]] void atnstart(
      const name owner, const uint64_t& expid, const Date& start_date );
  // auction request cancel
  [[eosio::action]] void atnrqcancel( const name owner, const uint64_t actnid );
  // auction reveal - Reveal the price
  [[eosio::action]] uint64_t atnreveal( const name owner, const uint64_t& auction );
  // auction subscribe
  [[eosio::action]] void expsubscribe( const name bkn, const uint64_t& expid );
  // auction bid
  [[eosio::action]] void atnbid( const name bkn, const float price, const id );
  // bakan basic register
  [[eosio::action]] void usrregister(
      const name user, const string& name, const string& surname, uint8_t& role );
  // Action to delete data, just for dev
  [[eosio::action]] void deletedata();

  // ******* READ ONLY ACTIONS ********
  [[eosio::action]] uint32_t calcengexp(
      const uint64_t idexp, const uint32_t& places );

 private:
  static inline time_point_sec current_time_point_sec() {
    return time_point_sec( current_time_point() );
  }
  config_t      config;
  usr_t         usr;
  exp_t         exp;
  exp_subs_t    exp_subs;
  actn_t        actn;
  bkn_exp_t     bkn_exp;
  bid_record_t  bid_record;
  actnbooster_t actnbooster;
};
}  // namespace quechimba
