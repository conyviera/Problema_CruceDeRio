
// RiverCrossing.h
#ifndef RIVERCROSSING_H
#define RIVERCROSSING_H

#include <climits>
#include <iostream>
#include "State.h"
#include "Heap.h"
#include "Stack.h"

struct TmpMove { int k1, k2; };

class RiverCrossing {
public:
    explicit RiverCrossing(int maxStates);
    ~RiverCrossing();
    State* solve();

private:
    bool isSafe(const State* s) const;
    int computeHeuristic(State* s) const;
    void applyAndPush(State* s, HeapNode* parent, int g, int uptoBoat);
    void expandAllBoats(State* s, HeapNode* parent, int g, int b);
    void printStateFormatted(const State* s) const;
    static void printSolution(HeapNode* node);

    Heap*     open;
    Stack*    closed;
    int       maxStates;
    TmpMove*  choice;
};

#endif // RIVERCROSSING_H
