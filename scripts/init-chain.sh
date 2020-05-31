#!/bin/bash

# Exit on fail
set -eu

set -a # automatically export all variables
source .env
set +a

echo $CLEOS_PATH
echo "Initalizing wallet"
$CLEOS_PATH -u $NODEOS_API wallet list
echo "Sleeping to ensure keosd has a chance to start"
sleep 10

# cd /

echo "Importing system keys"
$CLEOS_PATH -u $NODEOS_API wallet create -n qc-wallet --file qc-wallet.txt
$CLEOS_PATH -u $NODEOS_API wallet import -n qc-wallet --private-key $SYSTEM_ACCOUNT_PK

echo "Done importing system keys"

echo "Creating main contract user"
$CLEOS_PATH -u $NODEOS_API create account eosio $MAIN_CONTRACT_NAME $SYSTEM_ACCOUNT_PUB_KEY -p eosio@active
echo "End creating main contract user"

if [[ "$CREATE_TEST_ACCOUNTS" == "true" ]]; then
  echo "Creating test users"
  $CLEOS_PATH -u $NODEOS_API create account eosio bob $SYSTEM_ACCOUNT_PUB_KEY -p eosio@active
  $CLEOS_PATH -u $NODEOS_API create account eosio alice $SYSTEM_ACCOUNT_PUB_KEY -p eosio@active
  $CLEOS_PATH -u $NODEOS_API create account eosio james $SYSTEM_ACCOUNT_PUB_KEY -p eosio@active
  $CLEOS_PATH -u $NODEOS_API create account eosio agency1 $SYSTEM_ACCOUNT_PUB_KEY -p eosio@active
  $CLEOS_PATH -u $NODEOS_API create account eosio agency2 $SYSTEM_ACCOUNT_PUB_KEY -p eosio@active
  echo "Done create test user"
fi

# Build contract optional
# sh ./build.sh

echo "Deploying contract"
$CLEOS_PATH set contract $MAIN_CONTRACT_NAME ./build/$MAIN_CONTRACT_NAME -p $MAIN_CONTRACT_NAME@active
echo "End Deploying contract"
