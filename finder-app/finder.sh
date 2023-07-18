#!/bin/sh

DIR=$1
search=$2

if [ "$#" -eq 2 ]
then 
	if [ -d $DIR ]
	then
		nFiles=$(find $DIR -type f |  wc -l)
		nMatch=$(grep -r $search $DIR | wc -l)

		echo "The number of files are $nFiles and the number of matching lines are $nMatch" 
		exit 0
	else
		echo "fail"
		exit 1

	fi
else
	echo "fail"
	exit 1
fi
