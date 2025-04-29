# Makefile completo

all: State.o Heap.o Stack.o RiverCrossing.o testState testHeap testStack testRiverCrossing main

State.o: State.cpp State.h
	g++ -g -c State.cpp

Heap.o: Heap.cpp Heap.h
	g++ -g -c Heap.cpp

Stack.o: Stack.cpp Stack.h
	g++ -g -c Stack.cpp

RiverCrossing.o: RiverCrossing.cpp RiverCrossing.h
	g++ -g -c RiverCrossing.cpp

testState: testState.cpp State.o
	g++ -g testState.cpp State.o -o testState

testHeap: testHeap.cpp Heap.o State.o
	g++ -g testHeap.cpp Heap.o State.o -o testHeap

testStack: testStack.cpp Stack.o State.o
	g++ -g testStack.cpp Stack.o State.o -o testStack

testRiverCrossing: testRiverCrossing.cpp RiverCrossing.o Heap.o Stack.o State.o
	g++ -g testRiverCrossing.cpp RiverCrossing.o Heap.o Stack.o State.o -o testRiverCrossing

main: main.cpp RiverCrossing.o Heap.o Stack.o State.o
	g++ -g main.cpp RiverCrossing.o Heap.o Stack.o State.o -o main

clean:
	rm -f *.o testState testHeap testStack testRiverCrossing main
