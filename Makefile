all: p2

run: p2
	./p2 $(file)

p2: p2.o
	g++ -Wall -lncurses -g -o p2 p2.o

p2.o: p2.cpp
	g++ -Wall -std=c++14 -c -lncurses -g -O0 -pedantic-errors p2.cpp

clean: 
	rm -f *~
	rm -f *.o
	rm -f p2
	rm -f untitled.txt