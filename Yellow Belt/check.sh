#!/usr/bin/env bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color
TEST_BIN="C:\Users\Andrew\source\repos\Yandex Coursera C++ Specialization\Debug\Yellow belt.exe"

for input in `ls input*`;
do
    output=`echo -n $input | perl -lpe 's/^input/expected/g'`
    res=`diff -u <(cat "$input" | "$TEST_BIN") <(cat "$output") | perl -lpe 's/^/\t/g'`
    if [ ! -z "$res" ];
    then
        echo -e "${RED}Failed test $input:${NC}\n$res"
    else
        echo -e "${GREEN}Test $input OK${NC}"        
    fi;
done
    read -n 1 -s -r -p "Press any key to continue"