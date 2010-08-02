OBJS = utility.o print.o memory.o database.o
FLAGS=-lncurses -lmongoclient -lboost_thread-mt -lboost_filesystem -lboost_program_options
all: $(OBJS) memory.cpp main.cpp utility.cpp
	g++ $(OBJS) main.cpp -o memory $(FLAGS)

make-test: test.cpp
	//Later actually compile the test code and run it.
	g++ test.cpp -o test -lncurses

test: all make-test
	test

memory.o: print.o memory_obj.cpp
	g++ -c memory_obj.cpp -o memory.o

clean:
	@rm -f *~
	@rm -f *.out
	@rm -f test
	@rm -f main
	@rm -f *.o
