#!/bin/bash
# ./sm_test.exe "`cat cmd.txt`"

cat $1 | while read line
do
    echo "line [$line]" >> 1.txt
    ./sm_test.exe -s "$line"
    sleep 0.2
done

exit 0
