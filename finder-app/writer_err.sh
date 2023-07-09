#!/bin/bash

NUMFILES=${1:-10}
WRITESTR=${2:-"AELD_IS_FUN"}
USERNAME="$(cat ./conf/username.txt)"

mkdir /tmp/aeld-data

for((i=1; i<=NUM_FILES; i++))
do
	filename="$username$i"
	touch "/

