#!/bin/bash

if [[ ! -f $1 ]]; then
    echo "File does not exist"
    exit 1
fi

blocks=$(stat --format=%b $1)

printf "%s occupies %d blocks.\n" "$1" ${blocks}
