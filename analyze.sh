#!/bin/bash
if [ $# -eq 1 ]
then
    # Your code here. (1/4)
	grep "WARN\|ERROR"  $1 > bug.txt
else
    case $2 in
    "--latest")
        # Your code here. (2/4)
	tail -n , --lines=5 $1 
    ;;
    "--find")
        # Your code here. (3/4)
	grep $3 $1  > $3.txt
    ;;
    "--diff")
	diff -q $1 $3 > /dev/null
        # Your code here. (4/4)
	if [/dev/null -nt $3 ]
	then
		echo different
	else
		echo same
	fi	

    ;;
    esac
fi
