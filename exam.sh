#!/bin/bash
mkdir mydir
chmod 777 mydir
touch myfile
echo 2023 > myfile
mv moveme mydir
cp copyme mydir
mv mydir/copyme mydir/copied
cat readme
gcc bad.c 2> err.txt
a=1
b=10
if [ $# -gt 1]
then
	b=$1
fi
mkdir gen
cd gen
while [ $a -le $b ]
do
	touch $a.txt
	a=$[$a+1]
done

