#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
g++ -std=c++20 -O2 -Wall -Wextra server.cpp -o server_day6
./server_day6 .
