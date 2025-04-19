#pragma once
#include "State.h"
#include "Heap.h"
#include "Stack.h"
#include <chrono>

class RiverCrossing {
    int maxStates;
    Heap* open;
    Stack* closed;
    
    // Variables para control de tiempo
    std::chrono::steady_clock::time_point start_time;
    const int timeout_seconds = 60;  // 1 minuto
    
    // Estructura para movimientos temporales
    struct TmpMove { int k1, k2; };
    static const int MAXB = 8;
    TmpMove choice[MAXB];  // elección para cada bote
    
    // Métodos privados
    double getElapsedSeconds();
    void applyAndPush(State* s, HeapNode* parent, int g, int uptoBoat, int moved);
    void expandAllBoats(State* s, HeapNode* parent, int g, int b, int moved);
    bool isSafe(const State* s) const;
    int computeHeuristic(State* s);

public:
    RiverCrossing(int m);
    ~RiverCrossing();
    State* solve();
};