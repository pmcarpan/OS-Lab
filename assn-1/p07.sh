#!/bin/bash

GREEN=`tput setaf 2`
RESET=`tput sgr0`
BOLD=`tput bold`

function disp_prompt() {
    printf "${BOLD}${GREEN}BCSE III >>${RESET} "
}

function handle_input() {
    echo $1
}



while true; do
    disp_prompt
    read line
    handle_input "$line"
    break
done
