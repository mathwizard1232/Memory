OBJS = utility.o print.o memory.o database.o card.o
FLAGS=-lncurses -lmongoclient -lboost_thread-mt -lboost_filesystem -lboost_program_options
all: memory
	./memory	

memory: $(OBJS)# memory.cpp main.cpp utility.cpp card.cpp
	g++ $(OBJS) main.cpp -o memory $(FLAGS)

install: memory
	sudo cp ./memory /usr/bin

make-test: test.cpp
	#Later actually compile the test code and run it.
	g++ test.cpp -o test -lncurses

test: all make-test
	test

memory.o: print.h database.h card.h memory_obj.cpp
	g++ -c memory_obj.cpp -o memory.o

clean:
	@rm -f *~
	@rm -f *.out
	@rm -f test
	@rm -f main
	@rm -f *.o
