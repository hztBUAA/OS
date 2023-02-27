.PHONY: clean case_add case_sub case_mul case_div case_all out

out: calc.c case_all
	gcc -o calc calc.c
	./calc < case_all > out.txt
case_add: case_add casegen.c
	gcc -o casegen casegen.c 
	 cat "add 100" | ./casegen > case_add.txt
case_sub: case_sub casegen.c
	gcc -o casegen casegen.c
	 cat "sub 100" | ./casegen > case_sub.txt
case_mul: case_mul casegen.c
	gcc -o casegen casegen.c
	 cat "mul 100" | ./casegen  > case_mul.txt
case_div: case_div casegen.c
	gcc -o casegen casegen.c
	 cat "div 100" | ./casegen > case_div.txt
case_all:
	make case_add
	make case_sub
	make case_mul
	make case_div
	cat case_add case_sub case_mul case_div > case_all.txt


clean:
	rm -f out calc casegen case_*
