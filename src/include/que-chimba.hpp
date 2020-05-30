#pragma once

#include <eosio/eosio.hpp>
#include "./tables.hpp"

using namespace eosio;

namespace quechimba
{
  class [[eosio::contract("qccontract")]] qccontract : public contract
  {
  public:
    qccontract(const name self, const name receiver, datastream<const char *> ds) 
      : eosio::contract(self, receiver, ds), bkn(self, self.value), exp(self, self.value) {}

    // account register
    [[eosio::action]] void accreg(const name username, const string firstname, const string lastname) const;
    // experience publish
    [[eosio::action]] void expublish(const name owner, const id contendid) const;

    // auction start
    [[eosio::action]] void atnstart(const name owner, const id auction) const;
    // auction request cancel
    [[eosio::action]] void atnrqcancel(const name owner, const id auction) const;
    // auction reveal
    [[eosio::action]] void atnreveal(const name owner, const id auction) const;
    // auction subscribe
    [[eosio::action]] void atnsubscribe(const name bkn, const id expid) const;
    // auction bid
    [[eosio::action]] void atnbid(const name bkn, const float price, const id ) const;

  private:
    bkn_t bkn;
    exp_t exp;
  };
} // namespace quechimba
