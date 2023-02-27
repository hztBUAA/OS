.PHONY: clean case_add case_sub case_mul case_div case_all out

out: calc.c case_all
	gcc -o calc calc.c
	./calc < case_all > out.txt
case_add: case_add casegen.c
	gcc -o casegen casegen.c 
	./casegen add 100 > case_add.txt
case_sub: case_sub casegen.c
	gcc -o casegen casegen.c
	./casegen sub 100 > case_sub.txt
case_mul: case_mul casegen.c
	gcc -o casegen casegen.c
	./casegen mul 100 > case_mul.txt
case_div: case_div casegen.c
	gcc -o casegen casegen.c
	./casegen div 100 > case_div.txt
case_all:
	make case_add
	make case_sub
	make case_mul
	make case_div
	cat case_add.txt  case_sub.txt case_mul.txt case_div.txt > case_all.txt


clean:
	rm -f out calc casegen case_*
