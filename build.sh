#!usr/bin/env bash

set -e

# gcc -Wall -Wextra -O0 -ggdb -std=c99 test_msc_ht.c -o test_msc_ht.prog
g++ -Wall -Wextra -O0 -ggdb test_msc_ht.cpp -o test_msc_ht.prog

./test_msc_ht.prog