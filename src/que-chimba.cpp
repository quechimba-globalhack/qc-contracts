#include "./include/que-chimba/que-chimba.hpp"

#include "./include/constants/global.hpp"
#include "./include/token/qc-token.hpp"

namespace quechimba {

void
qccontract::init() {
  // Initialize global config table
  globalconfig default_config;
  config.get_or_create( _self, default_config );
  // Fill booster table
  actnbooster.emplace( _self, [&]( auto& row ) {
    row.id          = actnbooster.available_primary_key();
    row.boost_id    = booster::booster_type::extra_bid;
    row.description = "Give an extra bid at any time of the auction";
  } );
  actnbooster.emplace( _self, [&]( auto& row ) {
    row.id          = actnbooster.available_primary_key();
    row.boost_id    = booster::booster_type::time_machine;
    row.description = "You can bid until N secs after the auction";
  } );
  actnbooster.emplace( _self, [&]( auto& row ) {
    row.id          = actnbooster.available_primary_key();
    row.boost_id    = booster::booster_type::midas_hand;
    row.description = "You will have N extra lukas to use only in the bid";
  } );
}

// experience publish
void
qccontract::exppublish(
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
      start_exp > current_time_point_sec() + days_to_sec( state.startexpdays ),
      "start experience date must be within 3 months from now." );
  // Calculate Max Auction Start Date = Start Date - 2 months
  time_point_sec maxactntdate = start_exp - days_to_sec( state.maxstartactn );
  // TODO: Update balance of the agency

  exp.emplace( owner, [&]( auto& row ) {
    // Autoincrement id
    row.exp_id    = exp.available_primary_key();
    row.bkn_id    = owner;
    row.content   = content;
    row.places    = places;
    row.base_val  = asset( baseprice, eosio::symbol( token_name, token_precision ) );
    row.pub_price = state.subactnprice;  // Price in LKS
    row.start_date   = start_exp;
    row.maxactntdate = maxactntdate;
  } );
}

// auction start
void
qccontract::atnstart(
    const name owner, const uint64_t& expid, const Date& start_date ) {
  // TODO: It is missing a validation to only allow the start action to the owner
  // agency
  require_auth( owner );
  auto itr = exp.find( expid );
  check( itr != exp.end(), "The experience does not exist" );
  // Validate that an auction with the same expid does not exist
  auto atnindex = actn.get_index<"byexp"_n>();
  auto itracrtn = atnindex.lower_bound( expid );
  check( itracrtn == atnindex.end(), "The auction was already started" );
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
    row.actn_id     = actn.available_primary_key();
    row.exp_id      = expid;
    row.start_date  = start_date;
    row.duration    = state.actntime;
    row.start_value = itr->base_val;
    row.created_at  = current_time_point_sec();
  } );
}

// auction request cancel
void
qccontract::atnrqcancel( const name owner, const uint64_t actnid ) {
  require_auth( owner );
  // TODO: Validate that is an agency
  auto itr = actn.find( actnid );
  check( itr == actn.end(), "The auction does not exist" );
  actn.modify( itr, owner, [&]( auto& row ) { row.req_cancel = true; } );
}

// auction reveal - reveal the price
uint64_t
qccontract::atnreveal( const name owner, const uint64_t& auction ) {
  require_auth( owner );
  // TODO: Validate is an agency
  return 0;
}

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
qccontract::atnbid(
    const name bkn, const uint64_t& bidprice, const uint64_t& actnid ) {
  require_auth( bkn );
  // TODO: Validaciones del drive
  // Validate the auction exist
  auto itr = actn.find( actnid );
  check( itr != actn.end(), "The auction does not exist" );
  asset price = asset( bidprice, eosio::symbol( token_name, token_precision ) );
  // Validate the bkn has the LKS
  check( price.amount > 0, "price must be positive" );
  // Validate that the bakan has the amount of lks the bid
  asset bal = balance( bkn );
  check( bal.amount > price.amount, "Insufficient balance " );
  // Validate the bid is greater than the start_value bid
  eosio::print( "Base Value ", itr->start_value );
  eosio::print( "Balance del man ", bal );
  eosio::print( "Highest price ", itr->highest_bid );
  eosio::print( "Price ", price );
  check( itr->start_value < price, "The bid does not exceed the base value" );
  eosio::print( "After check base value" );
  // Validate the bid is greater than the highest bid
  if ( itr->highest_bid.amount > 0 ) {
    check( itr->highest_bid < price, "The bid does not exceed the highest value" );
  }
  // // ***** Logic to do the bid ******
  // instance transfer token action
  // Transfer to qccontract account
  eosio::action transferBank = eosio::action(
      eosio::permission_level{bkn, "active"_n}, "qctoken"_n, "transfer"_n,
      std::make_tuple(
          bkn, _self,
          asset( price.amount, eosio::symbol( token_name, token_precision ) ),
          std::string( "bid transfer" ) ) );
  transferBank.send();

  // instance transfer token action
  // Refund the last bidder if exist with the highest price
  if ( itr->bkn ) {
    eosio::action refund = eosio::action(
        eosio::permission_level{_self, "active"_n}, "qctoken"_n, "transfer"_n,
        std::make_tuple(
            _self, itr->bkn,
            asset(
                itr->highest_bid.amount,
                eosio::symbol( token_name, token_precision ) ),
            std::string( "bid refund" ) ) );
    refund.send();
  }
  // Update highest price
  actn.modify( itr, bkn, [&]( auto& row ) {
    row.highest_bid = price;
    row.bkn         = bkn;
    row.counter     = itr->counter + 1;
  } );
  // Emplace bid table
  bid_record.emplace( bkn, [&]( auto& row ) {
    row.bid_id     = bid_record.available_primary_key();
    row.bkn_id     = bkn.value;
    row.actn_id    = itr->actn_id;
    row.price      = price;
    row.created_at = current_time_point_sec();
    // row.current_fib = *****; // TODO: Implement fibonacci time validation
    // row.booster_id = ****; // TODO: Implement booster logic
  } );
}
// bakan basic register
void
qccontract::usrregister(
    const name user, const eosio::string& name, const eosio::string& surname,
    uint8_t& role ) {
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

  require_recipient( user );

  // Transfer LKS to the new user
  // can specify the contract to send the action to as first argument
  qctoken::token::transfer_action payout( "qctoken"_n, {get_self(), "active"_n} );
  // transfer arguments are now passed as postional arguments
  payout.send(
      _self, user,
      asset( tokens_initial, eosio::symbol( token_name, token_precision ) ),
      "start" );
}  // namespace quechimba
// TODO: Define flag for this action
// Action to delete data, use only in for DEV
void
qccontract::deletedata() {
  require_auth( _self );
  // Delete config data
  // config.remove();
  // // Delete experience data
  // exp_t exp( _self, _self.value );
  // auto  itr = exp.begin();
  // while ( itr != exp.end() ) {
  //   itr = exp.erase( itr );
  // }
  // Delete experience subscriber data
  // exp_subs_t exp_subs( _self, _self.value );
  // auto       itr = exp_subs.begin();
  // while ( itr != exp_subs.end() ) {
  //   itr = exp_subs.erase( itr );
  // }
  // Delete auction data
  actn_t actn( _self, _self.value );
  auto   itr = actn.begin();
  while ( itr != actn.end() ) {
    itr = actn.erase( itr );
  }
}

// [[eosio::action]] void
// qccontract::sendtoalice() {
//   // make an inline transfer to alice
//   require_auth( eosio::name( "maluma" ) );
//   // eosio::action(
//   //     eosio::permission_level{_self, "active"_n}, "qctoken"_n, "transfer"_n,
//   //     std::make_tuple(
//   //         "bob"_n, "alice"_n, eosio::asset( 10000, eosio::symbol( "LKS", 4 ) )
//   ),
//   //     std::string( "" ) )
//   //     .send();
//   eosio::action transferAlice = eosio::action(
//       eosio::permission_level{"maluma"_n, "active"_n}, "qctoken"_n, "transfer"_n,
//       std::make_tuple(
//           "maluma"_n, "bob"_n, eosio::asset( 10000, eosio::symbol( "LKS", 4 ) ),
//           std::string( "" ) ) );
//   transferAlice.send();
// }
uint64_t
qccontract::ping() {
  return 200;
}

// ******* READ ONLY ACTIONS ********
uint32_t
qccontract::calcengexp( const uint64_t idexp, const uint32_t& places ) {
  auto cidx  = exp_subs.get_index<"byexp"_n>();
  auto itr_l = cidx.lower_bound( idexp );
  auto itr_u = cidx.upper_bound( idexp );
  check( itr_l != cidx.end(), "The experience does not have subscribers" );

  auto subs        = std::distance( itr_l, itr_u );
  auto state       = config.get();
  auto subscribers = std::distance( exp.begin(), exp.end() );
  eosio::print( "Subscribers ", subscribers );
  // Calculate engagement, result is a number greater than 0
  auto eng = ( subscribers * 100 ) / ( places * state.engpct );
  eosio::print( "Engagement", eng );
  return eng;
}
asset
qccontract::balance( const name user ) {
  // const eosio::symbol sym = eosio::symbol( token_name, token_precision );
  const asset my_balance = qctoken::token::get_balance(
      "qctoken"_n, user, eosio::symbol( token_name, token_precision ).code() );
  print( "my balance is ", my_balance );
  return my_balance;
}
}  // namespace quechimba
