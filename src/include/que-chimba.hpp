#pragma once

#include <eosio/eosio.hpp>
#include "./tables.hpp"

using eosio::contract;

namespace quechimba
{

  class [[eosio::contract("quechimba")]] quechimba : public contract
  {
  public:
    quechimba(name receiver, , name code, datastream<const char *> ds) : eosio::contract(receiver, code, ds);

    [[eosio::action]] void accreg(const name username, const string firstname, const string lastname) const;
    [[eosio::action]] void expublish(const id contendid) const;

    [[eosio::action]] void auctsubscribe(const id expid, name bkn) const;
    [[eosio::action]] void auctbid(const id expid, const name bkn) const;

  private:
    usertable ut;
  }
} // namespace quechimba
