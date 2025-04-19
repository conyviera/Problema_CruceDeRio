#pragma once
#include "Heap.h"
#include "Stack.h"

class RiverCrossing {
    Heap* open;
    Stack* closed;
    int maxStates;
public:
    RiverCrossing(int maxStates=100000);
    ~RiverCrossing();
    State* solve();
};