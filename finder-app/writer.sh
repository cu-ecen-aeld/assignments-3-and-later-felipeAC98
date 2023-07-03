#!/bin/bash
FPATH=$1
STRING=$2

if (("$#" == 2))
then
	directory=$(dirname "$FPATH")
	mkdir -p "$directory"
	echo "$STRING" > "$FPATH"
	exit 0
else
	exit 1
fi
