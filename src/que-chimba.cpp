#include "./include/que-chimba/que-chimba.hpp"

namespace quechimba {

void
qccontract::init() {
  // Initialize global config table
  globalconfig default_config;
  config.get_or_create( _self, default_config );
}

void
qccontract::accreg(
    const name username, const string firstname, const string lastname ) const {}

// experience publish
void
qccontract::expublish( const name owner, const eosio::checksum256& content ) {
  // Valid if the user exist in the chain
  require_auth( owner );
  exp.emplace( owner, [&]( auto& row ) {
    // Autoincrement id
    row.exp_id  = exp.available_primary_key();
    row.bkn_id  = owner;
    row.content = content;
  } );
}

// auction start
void
qccontract::atnstart( const name owner, const id auction ) const {}

// auction request cancel
void
qccontract::atnrqcancel( const name owner, const id auction ) const {}

// auction reveal
void
qccontract::atnreveal( const name owner, const id auction ) const {}

// auction subscribe
void
qccontract::atnsubscribe( const name bkn, const id expid ) const {}

// auction bid
void
qccontract::atnbid( const name bkn, const float price, const id ) const {}
// bakan basic register
void
qccontract::usrregister(
    const name user, const eosio::string name, const eosio::string surname ) {
  // Valid if the user exist in the chain
  require_auth( user );
  // Register if the user does not exist
  auto itr = bkn.find( user.value );
  eosio::check( itr == bkn.end(), "The bakan is already registered" );
  if ( itr == bkn.end() ) {  // new user
    bkn.emplace( user, [&]( auto& row ) {
      row.bkn_id  = user;
      row.name    = name;
      row.surname = surname;
    } );
  }
}
}  // namespace quechimba
