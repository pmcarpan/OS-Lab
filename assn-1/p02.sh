#!/bin/bash

tot_files=$(find . -type f | wc -l)

echo "Total files in curr dir: " $tot_files

echo -e "\nFile info for subdirectories:\n"

find . -mindepth 1 -maxdepth 1 -type d -print0 |
while read -r -d '' line; do
    echo $line "->" $(find "$line" -maxdepth 1 -type f | wc -l) "file(s)"
    find "$line" -mindepth 1 -maxdepth 1 -type f -print0 |    
    while read -r -d '' filename; do
        echo -e "\t" $filename
    done
done

echo -e "\nFiles modfied <= 2 days ago:\n"

find . -maxdepth 1 -type f -mtime -3 -print0 |
while read -r -d '' filename; do
    echo -e $filename
done
