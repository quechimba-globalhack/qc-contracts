#!/bin/bash
# Start keosd
keosd &
# Give time to start keosd
sleep 10
nodeos -e -p eosio \
    --plugin eosio::producer_plugin \
    --plugin eosio::producer_api_plugin \
    --plugin eosio::chain_api_plugin \
    --plugin eosio::http_plugin \
    --plugin eosio::history_plugin \
    --plugin eosio::history_api_plugin \
    --filter-on='*' \
    --access-control-allow-origin='*' \
    --contracts-console \
    --http-validate-host=false \
    --replay-blockchain \
    --hard-replay-blockchain \
    --delete-all-blocks \
    --verbose-http-errors >>nodeos.log 2>&1
