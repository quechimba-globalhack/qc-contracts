#pragma once

#include <eosio/eosio.hpp>

#include "./tables.hpp"

using namespace eosio;

namespace quechimba {
class [[eosio::contract( "qccontract" )]] qccontract : public contract {
 public:
  qccontract( const name self, const name receiver, datastream<const char *> ds )
      : eosio::contract( self, receiver, ds ),
        config( self, self.value ),
        bkn( self, self.value ),
        exp( self, self.value ),
        exp_subs( self, self.value ),
        actn( self, self.value ),
        bkn_exp( self, self.value ),
        bid_record( self, self.value ) {}

  // account register
  [[eosio::action]] void accreg(
      const name username, const string firstname, const string lastname ) const;
  // experience publish
  [[eosio::action]] void expublish( const name owner, const id contendid ) const;

  // auction start
  [[eosio::action]] void atnstart( const name owner, const id auction ) const;
  // auction request cancel
  [[eosio::action]] void atnrqcancel( const name owner, const id auction ) const;
  // auction reveal
  [[eosio::action]] void atnreveal( const name owner, const id auction ) const;
  // auction subscribe
  [[eosio::action]] void atnsubscribe( const name bkn, const id expid ) const;
  // auction bid
  [[eosio::action]] void atnbid( const name bkn, const float price, const id ) const;

 private:
  config_t     config;
  bkn_t        bkn;
  exp_t        exp;
  exp_subs_t   exp_subs;
  actn_t       actn;
  bkn_exp_t    bkn_exp;
  bid_record_t bid_record;
};
}  // namespace quechimba
