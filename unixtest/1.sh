case $# in
	1)
		;;
	*)
		echo usage:ccat file.c
		exit 1;;
esac
case $1 in
	*.c)
		echo c file: $1
		awk '/\/\/.*/{print}/\/\*.*/' $1;;
	*)
		echo the file must be c file
esac
