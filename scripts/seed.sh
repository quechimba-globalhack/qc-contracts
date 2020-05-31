#!/bin/bash

set -eu

set -a # automatically export all variables
source .env
set +a


# Initializing chain
$CLEOS_PATH -u $NODEOS_API push action $MAIN_CONTRACT_NAME init '[]' -p eosio@active

#Register users
$CLEOS_PATH push action $MAIN_CONTRACT_NAME usrregister '["bob", "Alice", "Patino", "0"]' -p bob@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME usrregister '["alice", "Alice", "Patino", "0"]' -p alice@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME usrregister '["david", "David", "Patino", "0"]' -p david@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME usrregister '["jose", "Jose", "Patino", "0"]' -p jose@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME usrregister '["james", "James", "Patino", "0"]' -p james@active

# Register agencies
$CLEOS_PATH push action $MAIN_CONTRACT_NAME usrregister '["agency1", "Agency ", "", "1"]' -p agency1@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME usrregister '["agency2", "Agency1", "", "1"]' -p agency2@active

# Create experiences
$CLEOS_PATH push action $MAIN_CONTRACT_NAME expublish '["agency1","96ad17940e347866dc7463f291132d313d6778d60f0e47fb3a369d1f0a743c9d","2020-12-27T17:03:20","15", "5000"]' -p agency1@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME expublish '["agency2","96ad17940e347866dc7463f291132d313d6778d60f0e47fb3a369d1f0a743c9d","2020-12-27T17:03:20","15", "1000"]' -p agency2@active

#Subscribe users
$CLEOS_PATH push action $MAIN_CONTRACT_NAME expsubscribe '["bob", "0"]' -p bob@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME expsubscribe '["alice", "0"]' -p alice@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME expsubscribe '["david", "0"]' -p david@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME expsubscribe '["jose", "0"]' -p jose@active
$CLEOS_PATH push action $MAIN_CONTRACT_NAME expsubscribe '["james", "0"]' -p james@active
