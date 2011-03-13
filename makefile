OBJS = utility.o print.o memory.o database.o card.o basic.o sequential.o poem.o categories.o
LIBS=-lncurses -lmongoclient -lboost_thread-mt -lboost_filesystem -lboost_program_options
FLAG=-g #-O3 #-g
G=g++ $(FLAG) -c 

all: memory
#	./memory	

memory: $(OBJS) main.cpp
	g++ -g $(OBJS) main.cpp -o memory $(LIBS)

start: all
	ifconfig eth0 0.0.0.0
	m0 > /dev/null &
	sleep 2

run: start	
	./memory

gdbrun: start
	gdb ./memory

val: start
	valgrind --leak-check=full ./memory

install: memory
	cp /usr/bin/memory ./memorybck
	sudo cp ./memory /usr/bin

make-test: test.cpp
	#Later actually write the test code and run it.
	g++ test.cpp -o test -lncurses

test: all make-test
	test

database.o: card.h utility.h database.h database.cpp
	g++ $(FLAG) -c database.cpp

memory.o: print.h database.h card.h memory.h memory.cpp utility.h
	g++ $(FLAG) -c memory.cpp

CARD_DEP=card.h utility.h database.h
card.o: card.cpp $(CARD_DEP)
	g++ $(FLAG) -c card.cpp

basic.o: $(CARD_DEP) basic.cpp basic.h
	$(G) basic.cpp

poem.o: poem.cpp poem.h $(CARD_DEP)
	$(G) poem.cpp

sequential.o: sequential.cpp sequential.h $(CARD_DEP)
	$(G) sequential.cpp

%.o : %.cpp %.h
	g++ $(FLAG) -c $<

clean:
	@rm -f *~
	@rm -f *.out
	@rm -f test
	@rm -f main
	@rm -f *.o
