#include "./include/que-chimba/que-chimba.hpp"

namespace quechimba {

void
qccontract::init() {
  // Initialize global config table
  globalconfig default_config;
  config.get_or_create( _self, default_config );
}

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
  check( itr->role == usr::role_type::agency, "The user is not an agency" );
  // Get global config data
  auto state = config.get();
  // Validate that the start date can't be less
  // that today + startexpndays (MVP - 3 months)
  check(
      start_exp > current_time_point_sec() + days( state.startexpdays ),
      "start experience date must be within 3 months from now." );
  // Calculate Max Auction Start Date = Start Date - 2 months
  time_point_sec maxactntdate = start_exp - days( state.maxstartactn );
  // TODO: Update balance of the agency

  exp.emplace( owner, [&]( auto& row ) {
    // Autoincrement id
    row.exp_id       = exp.available_primary_key();
    row.bkn_id       = owner;
    row.content      = content;
    row.places       = places;
    row.base_val     = baseprice;
    row.pub_price    = state.subactnprice;  // Price in LKS
    row.start_date   = start_exp;
    row.maxactntdate = maxactntdate;
  } );
}

// auction start
void
qccontract::atnstart(
    const name owner, const uint64_t& expid, const Date& start_date ) {
  require_auth( owner );
  // TODO: Validate agency, only user that can start the auction
  auto itr = exp.find( expid );
  check( itr != exp.end(), "The experience does not exist" );
  // Only start if engagement is greater than 1
  check( itr->engagement >= 1, "The engagement is not enough" );
  // Validate start date
  check(
      start_date < itr->maxactntdate,
      "The start date is greater than the max auction date" );
  // Update sealed to avoid delete the experience
  exp.modify( itr, owner, [&]( auto& row ) { row.sealed = true; } );
  // MVP: For now it is not possible to delete the experience
  auto state = config.get();
  actn.emplace( owner, [&]( auto& row ) {
    row.actn_id    = actn.available_primary_key();
    row.exp_id     = expid;
    row.start_date = start_date;
    row.duration   = state.actntime;
    row.created_at = current_time_point_sec();
  } );
}

// auction request cancel
void
qccontract::atnrqcancel( const name owner, const id auction ) const {}

// auction reveal
void
qccontract::atnreveal( const name owner, const id auction ) const {}

// auction subscribe
void
qccontract::expsubscribe( const name bkn, const uint64_t& expid ) {
  require_auth( bkn );
  // Validate that experience exist
  auto itr_exp = exp.find( expid );
  check( itr_exp != exp.end(), "The experience does not exist" );
  // Validate bkn exist
  auto itr_bkn = usr.find( bkn.value );
  check( itr_bkn != usr.end(), "The user does not exist" );
  // Validate that the user is not subscribed to the experience
  auto exp_bkn_key = exp_subs::_by_exp_bkn( expid, bkn );
  auto exp_bkn_idx = exp_subs.get_index<"byexpbkn"_n>();
  auto itr_exp_bkn = exp_bkn_idx.find( exp_bkn_key );
  check( itr_exp_bkn == exp_bkn_idx.end(), "The user is already subscribed" );
  // Add data to expsubs table
  exp_subs.emplace( bkn, [&]( auto& row ) {
    row.exp_sub_id = exp_subs.available_primary_key();
    row.exp_id     = expid;
    row.bkn_id     = bkn;
    row.created_at = current_time_point_sec();
  } );
  // Calculate and set engagement
  uint16_t engagement = calcengexp( itr_exp->exp_id, itr_exp->places );
  exp.modify( itr_exp, bkn, [&]( auto& row ) { row.engagement = engagement; } );
}

// auction bid
void
qccontract::atnbid( const name bkn, const float price, const id ) {}
// bakan basic register
void
qccontract::usrregister(
    const name user, const string& name, const string& surname, uint8_t& role ) {
  // TODO: This method should only be used by users with admin rolee?
  // TODO: Validate valid role (0 or 1)
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
      row.role    = role;
    } );
  }
}
// Action to delete data, use only in for DEV
void
qccontract::deletedata() {
  require_auth( _self );
  // Delete config data
  config.remove();
  // // Delete experience data
  // exp_t exp( _self, _self.value );
  // auto  itr = exp.begin();
  // while ( itr != exp.end() ) {
  //   itr = exp.erase( itr );
  // }
  // Delete experience subscriber data
  exp_subs_t exp_subs( _self, _self.value );
  auto       itr = exp_subs.begin();
  while ( itr != exp_subs.end() ) {
    itr = exp_subs.erase( itr );
  }
}

// ******* READ ONLY ACTIONS ********
uint32_t
qccontract::calcengexp( const uint64_t idexp, const uint32_t& places ) {
  auto cidx  = exp_subs.get_index<"byexp"_n>();
  auto itr_l = cidx.lower_bound( idexp );
  auto itr_u = cidx.upper_bound( idexp );
  check( itr_l != cidx.end(), "The experience does not have subscribers" );

  auto subs = std::distance( itr_l, itr_u );
  // eosio::print( "Distance", subs );
  auto state       = config.get();
  auto subscribers = std::distance( exp.begin(), exp.end() );
  // Calculate engagement, result is a number greater than 0
  auto eng = ( subscribers * 100 ) / ( state.engpct * state.engpct );
  eosio::print( "Eng", eng );
  return eng;
}
}  // namespace quechimba
