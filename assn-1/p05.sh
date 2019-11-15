#!/bin/bash

GREEN=`tput setaf 2`
YELLOW=`tput setaf 3`
RESET=`tput sgr0`

read -p "Enter file name: " filename

if [[ ! -r $filename ]]; then
    printf "$YELLOW[WARN] File does not exist or is not readable$RESET\n"
    exit 1
fi

read -p "Enter search string: " searchterm

freq=$(grep -o $searchterm $filename | wc -l)

if [[ $freq == 0 ]]; then
    printf "Search string $GREEN%s$RESET not found in file $GREEN%s$RESET\n" $searchterm $filename
    exit 0
fi

printf "\nTotal Frequency: %d\n\n" $freq
printf "%-8s -> %s\n" "Line" "Frequency"

# (grep) [line-number]:search-term ->
# (uniq) occurrences [line-number]:search-term ->
# (cut) occurrences [line-number] -> pipe to while
grep -on $searchterm $filename | uniq -c | cut -d : -f 1 |
while read -r line; do
    tokens=($line) # extract tokens into array
    printf "%-8d -> %d\n" ${tokens[1]} ${tokens[0]}
done

