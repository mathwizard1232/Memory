OBJS = utility.o print.o memory.o database.o card.o basic.o sequential.o poem.o categories.o
LIBS=-lncurses -lmongoclient -lboost_thread-mt -lboost_filesystem -lboost_program_options
FLAG=-g #-O3 #-g
all: memory
#	./memory	

memory: $(OBJS)
	g++ -g $(OBJS) main.cpp -o memory $(LIBS)

install: memory
	sudo cp ./memory /usr/bin

make-test: test.cpp
	#Later actually write the test code and run it.
	g++ test.cpp -o test -lncurses

test: all make-test
	test

database.o: card.h utility.h database.h database.cpp
	g++ $(FLAG) -c database.cpp

memory.o: print.h database.h card.h memory.h memory.cpp
	g++ $(FLAG) -c memory.cpp -o memory.o

%.o : %.cpp %.h
	g++ $(FLAG) -c $<

clean:
	@rm -f *~
	@rm -f *.out
	@rm -f test
	@rm -f main
	@rm -f *.o
