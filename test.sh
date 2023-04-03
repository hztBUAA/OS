#!/bin/bash
echo $*
head -6 $1
if	echo "just for if test"
	 test $# -eq 4
then	
	echo "test check argument num is 4"
fi
echo "程序传入参数共有$#个"
echo $0 $1 $2 $3 $4
set b1 b2
echo "参数变为$#个"
echo $0 $1 $2 $3 $4
