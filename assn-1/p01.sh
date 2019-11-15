#!/bin/bash

NUMERIC_REGEX=^[0-9A-F]+\(\.[0-9A-F]+\)?

while true; do

    read -p "Enter var 1: " var1
    read -p "Enter var 2: " var2

    if [[ ! $var1 =~ $NUMERIC_REGEX ]] || [[ ! $var2 =~ $NUMERIC_REGEX ]]; then
        echo $var1 + $var2 = $var1$var2
    else
        res=$(echo "scale=2;$var1+$var2" | bc)
        echo $var1 + $var2 = $res

        res=$(echo "scale=2;$var1*$var2" | bc)
        echo $var1 \* $var2 = $res

        check=$(echo "$var2 == 0" | bc)
        if (( check == 1 )); then
            res="Division by zero error"    
        else
            res=$(echo "scale=2;$var1/$var2" | bc)
        fi
        echo $var1 / $var2 = $res
    fi

    read -p "Continue? (Y/N): " confirm
    if [[ ! $confirm =~ ^[[Yy]|[Yy][Ee][SsPp]] ]]; then
        exit 0
    fi

done


