#!/bin/bash
SRC_PATH=./src
BUILD_PATH=./build/
echo "Compiling with eosio-cpp"
# Build quechimba contract
eosio-cpp $SRC_PATH/que-chimba.cpp -o $BUILD_PATH/qccontract.wasm
# Build qctoken contract
eosio-cpp $SRC_PATH/qc-token.cpp -o $BUILD_PATH/qctoken.wasm
