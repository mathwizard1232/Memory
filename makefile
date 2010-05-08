all: print.o memory.cpp main.cpp utility.cpp
	g++ print.o main.cpp -o memory -lncurses

make-test: test.cpp
	//Later actually compile the test code and run it.
	g++ test.cpp -o test -lncurses

test: all make-test
	test

clean:
	@rm -f *~
	@rm -f *.out
	@rm -f test
	@rm -f main
	@rm -f memory
	@rm -f *.o
