#!/bin/bash
a=1
b=10
if [ $# -gt 1]
then
	b=$1
fi
while [ $a -le $b ]
do
	touch $a.txt
	a=$[$a+1]
done

