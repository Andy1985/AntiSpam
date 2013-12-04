#!/bin/bash

[ $# -ne 1 ] && { echo "Usage: $0 <dir>"; exit; }

OLDIFS=$IFS
IFS=$'\n'


cd $1
for file in `ls`
do
	#echo "testing $file ..."
	/home/dep/AntispamServer/test  "$file"
done

cd ..
IFS=$OLDIFS
