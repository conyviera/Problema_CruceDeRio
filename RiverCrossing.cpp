#include "RiverCrossing.h"
#include <climits>
#include <iostream>

double RiverCrossing::getElapsedSeconds() {
    auto current_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(
        current_time - start_time).count();
}

RiverCrossing::RiverCrossing(int m) : maxStates(m) {
    open = new Heap(m);
    closed = new Stack(m);
    start_time = std::chrono::steady_clock::now();
}

RiverCrossing::~RiverCrossing() { 
    delete open; 
    delete closed; 
}

void RiverCrossing::applyAndPush(State* s, HeapNode* parent,
    int g, int uptoBoat, int moved) {
    if (moved == 0) return;

    State* ns = s->clone();
    for (int b = 0; b <= uptoBoat; ++b) {
        if (choice[b].k1 == -2) continue;

        ns->boatSide[b] ^= 1;
        --ns->remTrips[b];

        if (choice[b].k1 >= 0) ns->itemSide[choice[b].k1] ^= 1;
        if (choice[b].k2 >= 0) ns->itemSide[choice[b].k2] ^= 1;
    }

    if (!ns->isValid() || !isSafe(ns)) { 
        delete ns; 
        return; 
    }
    
    if (!closed->find(ns)) {
        int h = computeHeuristic(ns);
        open->push(new HeapNode{ns, g+1, g+1+h, parent, nullptr});
        closed->push(ns);
    } else {
        delete ns;
    }
}

/* --------- recursión sobre cada bote --------------------- */
void RiverCrossing::expandAllBoats(State* s, HeapNode* parent,
                                   int g, int b, int moved)
{
    if (b == State::B) {                  // todos decididos
        applyAndPush(s, parent, g, b-1, moved);
        return;
    }

    if (s->remTrips[b] == 0) {            // sin combustible
        choice[b] = {-2,-2};
        expandAllBoats(s,parent,g,b+1,moved);
        return;
    }

    bool side     = s->boatSide[b];
    int  capacity = State::capacity[b];

    /* 1) bote quieto  ------------------------------------ */
    choice[b] = {-2,-2};
    expandAllBoats(s,parent,g,b+1,moved);

    /* 2) llevar 1 ítem (el primero que encuentre) -------- */
    int first = -1;
    for (int k = 0; k < State::I && first==-1; ++k)
        if (s->itemSide[k] == side) first = k;

    if (first != -1) {
        choice[b] = {first,-1};
        expandAllBoats(s,parent,g,b+1,moved+1);

        /* 3) llevar 2 ítems (primer par compatible) ------ */
        if (capacity >= 2) {
            int second = -1;
            for (int k = first+1; k < State::I && second==-1; ++k)
                if (s->itemSide[k] == side &&
                    !State::incompatible[first][k])
                    second = k;

            if (second != -1) {
                choice[b] = {first, second};
                expandAllBoats(s,parent,g,b+1,moved+2);
            }
        }
    }
}

bool RiverCrossing::isSafe(const State* s) const {
    for (int side = 0; side <= 1; ++side) {
        bool boat = false;
        for (int b = 0; b < State::B && !boat; ++b)
            if (s->boatSide[b] == side) boat = true;
        if (boat) continue;
        
        for (int i = 0; i < State::I; ++i) {
            if (s->itemSide[i] == side) {
                for (int j = i+1; j < State::I; ++j) {
                    if (s->itemSide[j] == side && State::incompatible[i][j])
                        return false;
                }
            }
        }
    }
    return true;
}

int RiverCrossing::computeHeuristic(State* s) {
    const int INF = INT_MAX;
    int remaining = 0;
    
    for (int k = 0; k < State::I; ++k) 
        if (!s->itemSide[k]) 
            ++remaining;
    
    if (!remaining) return 0;

    bool boat = false;
    int max_cap = 0;
    for (int b = 0; b < State::B; ++b) {
        if (!s->boatSide[b] && s->remTrips[b] > 0) {
            boat = true;
            if (State::capacity[b] > max_cap) {
                max_cap = State::capacity[b];
            }
        }
    }
    
    if (!boat) return INF;

    int min_trips = (remaining + max_cap - 1) / max_cap;
    
    if (!isSafe(s)) min_trips += State::I;
    
    return 2 * min_trips - 1;
}

State* RiverCrossing::solve() {
    State* init = new State();
    for (int k = 0; k < State::I; ++k) 
        init->itemSide[k] = false;
    for (int b = 0; b < State::B; ++b) { 
        init->boatSide[b] = false; 
        init->remTrips[b] = State::maxTrips[b]; 
    }

    if (!init->isValid() || !isSafe(init)) {
        std::cout << "Estado inicial inválido\n";
        delete init;
        return nullptr;
    }

    int h0 = computeHeuristic(init);
    open->push(new HeapNode{init, 0, h0, nullptr, nullptr});
    closed->push(init);
    
    State* best = nullptr;
    int bestCost = INT_MAX;
    int expanded = 0;

    while (HeapNode* node = open->pop()) {
        ++expanded;
        
        // Verificación de tiempo
        if (expanded % 1000 == 0 && getElapsedSeconds() > timeout_seconds) {
            std::cout << "Tiempo excedido (" << expanded << " estados explorados)\n";
            delete node->s;
            delete node;
            break;
        }

        State* s = node->s;
        int g = node->g;
        
        if (g >= bestCost) {
            delete s;
            continue;
        }
            
        if (s->isFinal()) {
            best = s;
            bestCost = g;
            
            // Reconstrucción del camino
            int len = 0;
            for (HeapNode* c = node; c; c = c->parent) ++len;
            
            State** path = new State*[len];
            int idx = len - 1;
            for (HeapNode* c = node; c; c = c->parent) 
                path[idx--] = c->s;

            std::cout << "\nSOLUCIÓN (" << g << " cruces, " << expanded << " estados)\n";
            for (int i = 0; i < len; ++i) {
                std::cout << "Paso " << i << ":\n";
                path[i]->printState();
            }
            
            delete[] path;
            break;
        }

        expandAllBoats(s, node, g, 0, 0);
    }

    if (!best) {
        std::cout << "No se encontró solución tras " << expanded << " estados.\n";
    }

    return best;
}