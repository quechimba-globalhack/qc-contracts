#!/bin/bash
SRC_PATH=./src
BUILD_PATH=./build/
echo "Compiling with eosio-cpp"
eosio-cpp $SRC_PATH/que-chimba.cpp -o $BUILD_PATH/qccontract.wasm
