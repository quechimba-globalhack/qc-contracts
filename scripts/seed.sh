#!/bin/bash

set -eu

set -a # automatically export all variables
source .env
set +a

# Initializing chain
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME init '[]' -p eosio@active

# Creating and issue token to contract account
cleos push action qctoken create '["qccontract", "10000000000.0000 LKS"]' -p qctoken
cleos push action qctoken issue '["qccontract", "10000000.0000 LKS", "issuing tokens"]' -p qccontract

#Register users
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["bob", "Bob", "Patino", "0"]' -p bob@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["alice", "Alice", "Wonderlan", "0"]' -p alice@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["james", "James", "Rodriguez", "0"]' -p james@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["maluma", "Juan", "Londono", "0"]' -p maluma@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["jbalvin", "Jose", "Osorio", "0"]' -p jbalvin@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["porky", "Ivan", "Duque", "0"]' -p porky@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["matarife", "Lord", "Voldermort", "0"]' -p matarife@active

# Register agencies
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["agency1", "Expedia ", "", "1"]' -p agency1@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["agency2", "Travel Ace Assistance", "", "1"]' -p agency2@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME usrregister '["agency3", "Parce Colombia", "", "1"]' -p agency3@active

# Create experiences
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME exppublish '["agency1","96ad17940e347866dc7463f291132d313d6778d60f0e47fb3a369d1f0a743c9d","2020-12-27T17:03:20","2", "50000000"]' -p agency1@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME exppublish '["agency2","96ad17940e347866dc7463f291132d313d6778d60f0e47fb3a369d1f0a743c9d","2020-12-27T17:03:20","2", "10000000"]' -p agency2@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME exppublish '["agency3","96ad17940e347866dc7463f291132d313d6778d60f0e47fb3a369d1f0a743c9d","2020-12-27T17:03:20","2", "10000000"]' -p agency3@active

#Subscribe users
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME expsubscribe '["bob", "0"]' -p bob@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME expsubscribe '["alice", "0"]' -p alice@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME expsubscribe '["james", "0"]' -p james@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME expsubscribe '["maluma", "1"]' -p maluma@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME expsubscribe '["jbalvin", "1"]' -p jbalvin@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME expsubscribe '["porky", "2"]' -p porky@active
cleos -u $NODEOS_API push action $MAIN_CONTRACT_NAME expsubscribe '["matarife", "2"]' -p matarife@active

#Create auction
cleos push action $MAIN_CONTRACT_NAME atnstart '["agency1", "0", "2020-10-28T17:01:20"]' -p agency1@active
