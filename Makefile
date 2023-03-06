.PHONY: clean case_add case_sub case_mul case_all case_div out

out: calc.c case_all
	gcc cal.c -o calc
	./calc < case_all > out
case_add: casegen.c
	gcc casegen.c -o casegen
	./casegen add 100 >case_add
case_sub: casegen.c
	gcc casegen.c -o casegen
	./casegen sub 100 >case_sub	
case_mul: casegen.c
	gcc casegen.c -o casegen
	./casegen mul 100 > case_mul
case_div: casegen.c
	gcc casegen.c -o casegen
	./casegen div 100 > case_div
case_all: case_add case_sub case_mul case_div
	make case_add
	make case_sub
	make case_mul
	make case_div
	cat case_add case_sub case_mul case_div > case_all
clean:
	rm -f out calc casegen case_*
