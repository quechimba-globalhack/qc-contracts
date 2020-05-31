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
qccontract::expublish(
    const name owner, const Hash& content, const Date& start_exp,
    const uint32_t& places, const uint64_t& baseprice ) {
  // Valid if the user exist in the chain
  require_auth( owner );
  // Validate that the user exist in the state
  auto itr = usr.find( owner.value );
  check( itr != usr.end(), "The user does not exist" );
  check( itr->rol == usr::rol_type::agency, "The user is not an agency" );
  // Get globan config data
  auto state = config.get();
  // Validate that the start date can't be less
  // that today + startexpndays (MVP - 3 months)
  check(
      start_exp > current_time_point_sec() + days( state.startexpdays ),
      "start experience date must be within 3 months from now." );
  // Calculate Max Auction Start Date = Start Date - 2 months
  time_point_sec maxactntdate = start_exp - days( state.maxstartctn );
  // TODO: Update balance of the agency

  exp.emplace( owner, [&]( auto& row ) {
    // Autoincrement id
    row.exp_id       = exp.available_primary_key();
    row.bkn_id       = owner;
    row.content      = content;
    row.engagement   = 0;
    row.places       = places;
    row.base_val     = baseprice;
    row.pub_price    = state.subactnprice;  // Price in LKS
    row.start_date   = start_exp;
    row.maxactntdate = maxactntdate;
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
    const name user, const string& name, const string& surname, uint8_t& rol ) {
  // TODO: This method should only be used by users with admin role?
  // TODO: Validate valid rol
  // eosio::print( "get_self ", get_self() );
  // Valid if the user exist in the chain
  require_auth( user );
  // Register if the user does not exist
  auto itr = usr.find( user.value );
  check( itr == usr.end(), "The user is already registered" );
  if ( itr == usr.end() ) {  // new user
    usr.emplace( user, [&]( auto& row ) {
      row.usrname = user;
      row.name    = name;
      row.surname = surname;
      row.rol     = rol;
    } );
  }
}
// Action to delete data, just for dev
void
qccontract::deletedata() {
  require_auth( _self );
  config.remove();
  exp_t exp( _self, _self.value );
  auto  itr = exp.begin();
  while ( itr != exp.end() ) {
    itr = exp.erase( itr );
  }
}
}  // namespace quechimba
