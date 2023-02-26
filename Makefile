.PHONY: clean case_add case_sub case_mul case_div case_all

out: calc case_all
	./calc < case_all > out
case_add: case_add casegen
	gcc casegen.c casegen 
	./casegen < echo "add 100" > case_add
case_sub: case_sub casegen
	gcc casegen.c casegen
	./casegen < echo "sub 100" > case_sub
case_mul: case_mul casegen
	gcc casegen.c casegen
	./casegen < echo "mul 100" > case_mul
case_div: case_div casegen
	gcc casegen.c casegen
	./casegen < echo "div 100" > case_div
case_all:
	make case_add
	make case_sub
	make case_mul
	make case_div
cat case_add case_sub case_mul case_div > case_all

# Your code here.

clean:
	rm -f out calc casegen case_*
