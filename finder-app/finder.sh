#!/bin/bash
DIR=$1
search=$2

if (("$#" ==2))
then 
	if [ -d "$DIR" ]
	then
		#allFiles="$(find $DIR -type f)"
		match=0
		for file in "$DIR"/*  #allFiles /*
		do
			fileMatch="$(cat $file | grep $search | wc -l )"
			((match=match+fileMatch))
		done
		nFiles="$(ls $DIR |  wc -l)"

		echo "The number of files are $nFiles and the number of matching lines are $match" 
		exit 0
	else
		echo "fail"
		exit 1

	fi
else
	echo "fail"
	exit 1
fi
