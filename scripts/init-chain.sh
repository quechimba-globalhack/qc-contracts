#!/bin/bash

# Exit on fail
set -eu

set -a # automatically export all variables
source .env
set +a

echo "Importing system keys"
cleos -u $NODEOS_API wallet create -n qc-wallet --file qc-wallet.txt
# cleos -u $NODEOS_API wallet unlock -n qc-wallet --password < qc-wallet.txt
cleos -u $NODEOS_API wallet import -n qc-wallet --private-key $SYSTEM_ACCOUNT_PK
cleos -u $NODEOS_API wallet import -n qc-wallet --private-key $MAIN_CONTRACT_PK
cleos -u $NODEOS_API wallet import -n qc-wallet --private-key $TOKEN_ACCOUNT_PK
cleos -u $NODEOS_API wallet import -n qc-wallet --private-key $ADMIN_ACCOUNT_PK

echo "Done importing system keys"

echo "Creating main contract users"
echo "Creating quechimba contract account"
cleos -u $NODEOS_API create account eosio $MAIN_CONTRACT_NAME $MAIN_CONTRACT_PUB_KEY
echo "Creating token contract account"
cleos -u $NODEOS_API create account eosio $TOKEN_CONTRACT_NAME $TOKEN_ACCOUNT_PUB_KEY
# cleos -u $NODEOS_API create account eosio qcadmin $ADMIN_ACCOUNT_PUB_KEY
echo "End creating main contract user"

if [[ "$CREATE_TEST_ACCOUNTS" == "true" ]]; then
  echo "Creating test users"
  cleos -u $NODEOS_API create account eosio bob $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio alice $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio james $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio maluma $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio jbalvin $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio porky $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio matarife $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio agency1 $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio agency2 $MAIN_CONTRACT_PUB_KEY -p eosio@active
  cleos -u $NODEOS_API create account eosio agency3 $MAIN_CONTRACT_PUB_KEY -p eosio@active
  echo "Done create test user"
fi

echo "Deploying contracts"
echo "Deploying quechimba contract"
cleos set code qccontract ./build/qccontract.wasm
cleos set abi qccontract ./build/qccontract.abi
echo "Deploying qctoken contract"
cleos set code qctoken ./build/qctoken.wasm
cleos set abi qctoken ./build/qctoken.abi
echo "End Deploying contract"

echo "Setting permission accounts"
cleos set account permission $MAIN_CONTRACT_NAME active --add-code
cleos set account permission $TOKEN_CONTRACT_NAME active --add-code
cleos set account permission $TOKEN_CONTRACT_NAME bob --add-code
cleos set account permission bob active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p bob
cleos set account permission alice active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p alice
cleos set account permission james active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p james
cleos set account permission maluma active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p maluma
cleos set account permission jbalvin active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p jbalvin
cleos set account permission porky active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p porky
cleos set account permission matarife active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p matarife
cleos set account permission agency1 active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p agency1
cleos set account permission agency2 active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p agency2
cleos set account permission agency3 active '{"threshold":1, "keys":[{"key":"'$MAIN_CONTRACT_PUB_KEY'", "weight":1}], "accounts": [{"permission":{"actor":"'$MAIN_CONTRACT_NAME'","permission":"eosio.code"},"weight":1}]}' owner -p agency3
echo "End setting permission accounts"
