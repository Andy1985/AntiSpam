#!/bin/bash

[ $# -ne 1 ] && { echo "Usage: $0 <dir>"; exit; }

OLDIFS=$IFS
IFS=$'\n'


cd $1
for file in `ls`
do
	echo "feeding $file ..."
	/home/dep/AntispamServer/feed -n $file
done

cd ..
IFS=$OLDIFS
