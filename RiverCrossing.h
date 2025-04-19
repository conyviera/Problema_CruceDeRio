#pragma once
#include "State.h"
#include "Heap.h"
#include "Stack.h"

class RiverCrossing {
    int maxStates;
    Heap* open;
    Stack* closed;
    
    int computeHeuristic(State* s);
    
public:
    RiverCrossing(int m);
    ~RiverCrossing();
    State* solve();
    bool isSafe(const State* s) const;
};