#!/bin/bash

YELLOW=`tput setaf 3`
RESET=`tput sgr0`

if [[ $# > 0 ]]; then
    printf "%-15s%-10s%-10s%-10s\n" "File" "printf" "scanf" "int"
    for file in "$@"; do
        if [[ ! -f $file ]]; then
            printf "%-15s%s\n" $file "$YELLOW[WARN] File does not exist$RESET"
            continue
        fi

	    printfCount=$(grep -o "printf(" $file | wc -l)
	    scanfCount=$(grep -o "scanf(" $file | wc -l)
	    intCount=$(grep -o "int " $file | wc -l)
	    printf "%-15s%-10s%-10s%-10s\n" $file $printfCount $scanfCount $intCount
    done
else
    printf "$YELLOW[WARN] Insufficient arguments$RESET\n"
fi
