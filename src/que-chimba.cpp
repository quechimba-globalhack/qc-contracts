#include "./include/que-chimba/que-chimba.hpp"

namespace quechimba {

void
qccontract::accreg(
    const name username, const string firstname, const string lastname ) const {}

// experience publish
void
qccontract::expublish( const name owner, const id contendid ) const {}

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

}  // namespace quechimba
