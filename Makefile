CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
SRCS = main.cpp State.cpp Heap.cpp Stack.cpp RiverCrossing.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = river_solver

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean