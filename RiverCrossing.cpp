#include "RiverCrossing.h"
#include <iostream>
#include <climits>
#include <chrono>

// helper para imprimir estado formateado
void RiverCrossing::printStateFormatted(const State* s) const {
    std::cout << "Botes: ";
    for (int b = 0; b < State::B; ++b) {
        std::cout << (s->boatSide[b] ? 'R' : 'L');
        if (b + 1 < State::B) std::cout << '/';
    }
    std::cout << '\n';
    std::cout << "Izquierda: ";
    bool first = true;
    for (int i = 0; i < State::I; ++i) {
        if (!s->itemSide[i]) {
            if (!first) std::cout << ' ';
            std::cout << i;
            first = false;
        }
    }
    std::cout << '\n';

    std::cout << "Derecha:   ";
    first = true;
    for (int i = 0; i < State::I; ++i) {
        if (s->itemSide[i]) {
            if (!first) std::cout << ' ';
            std::cout << i;
            first = false;
        }
    }
    std::cout << "\n---\n";
}

RiverCrossing::RiverCrossing(int maxStates_)
  : open(nullptr), closed(nullptr), maxStates(maxStates_), choice(nullptr) {
    open = new Heap(maxStates);
    closed = new Stack(maxStates);
    choice = new TmpMove[State::B];
    for (int b = 0; b < State::B; ++b)
        choice[b] = TmpMove{-2, -2};

    State* init = new State();
    for (int k = 0; k < State::I; ++k) init->itemSide[k] = false;
    for (int b = 0; b < State::B; ++b) {
        init->boatSide[b] = false;
        init->remTrips[b] = State::maxTrips[b];
    }
    if (!init->isValid() || !isSafe(init)) {
        std::cout << "Estado inicial inválido.\n";
        delete init;
        return;
    }
    int h0 = computeHeuristic(init);
    open->push(new HeapNode{init, 0, h0, nullptr, nullptr});
    closed->push(init);
}

RiverCrossing::~RiverCrossing() {
    delete[] choice;
    delete open;
    delete closed;
}

bool RiverCrossing::isSafe(const State* s) const {
    for (int side = 0; side <= 1; ++side) {
        bool boat_here = false;
        for (int b = 0; b < State::B; ++b) {
            if (s->boatSide[b] == side) { boat_here = true; break; }
        }
        if (boat_here) continue;
        for (int i = 0; i < State::I; ++i) {
            if (s->itemSide[i] != side) continue;
            for (int j = i + 1; j < State::I; ++j) {
                if (s->itemSide[j] == side && State::incompatible[i][j])
                    return false;
            }
        }
    }
    return true;
}

int RiverCrossing::computeHeuristic(State* s) const {
    const int INF = INT_MAX;
    int remaining = 0;
    for (int k = 0; k < State::I; ++k)
        if (!s->itemSide[k]) ++remaining;
    if (remaining == 0) return 0;

    bool boat_avail = false;
    int kmax = 0;
    for (int b = 0; b < State::B; ++b) {
        if (!s->boatSide[b] && s->remTrips[b] > 0) {
            boat_avail = true;
            if (State::capacity[b] > kmax) kmax = State::capacity[b];
        }
    }
    if (!boat_avail) return INF;

    int trips = (remaining + kmax - 1) / kmax;
    if (!isSafe(s)) trips += 10;
    return 2 * trips - 1;
}

void RiverCrossing::applyAndPush(State* s, HeapNode* parent, int g, int uptoBoat) {
    // 1) filtrar pasos sin movimiento
    bool anyMove = false;
    for (int b = 0; b < uptoBoat; ++b) {
        if (choice[b].k1 != -2) { anyMove = true; break; }
    }
    if (!anyMove) return;

    // 2) clonar y aplicar los movimientos
    State* ns = s->clone();
    for (int b = 0; b < uptoBoat; ++b) {
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

    // 3) heurística
    int h = computeHeuristic(ns);

    // 4) ¡ANTES de empujar! comprobamos que no esté ya en closed
    if (!closed->find(ns)) {
        open->push(new HeapNode{ns, g + 1, g + 1 + h, parent, nullptr});
        closed->push(ns);
    } else {
        delete ns;
    }
}


void RiverCrossing::expandAllBoats(State* s, HeapNode* parent, int g, int b) {
    if (b >= State::B) {
        applyAndPush(s, parent, g, State::B);
        return;
    }
    if (s->remTrips[b] == 0) {
        choice[b] = TmpMove{-2, -2};
        expandAllBoats(s, parent, g, b + 1);
        return;
    }

    bool side = s->boatSide[b];
    choice[b] = TmpMove{-1, -1};
    expandAllBoats(s, parent, g, b + 1);

    for (int k = 0; k < State::I; ++k) {
        if (s->itemSide[k] == side) {
            choice[b] = TmpMove{k, -1};
            expandAllBoats(s, parent, g, b + 1);
        }
    }

    if (State::capacity[b] >= 2) {
        for (int k1 = 0; k1 < State::I; ++k1) {
            if (s->itemSide[k1] != side) continue;
            for (int k2 = k1 + 1; k2 < State::I; ++k2) {
                if (s->itemSide[k2] == side && !State::incompatible[k1][k2]) {
                    choice[b] = TmpMove{k1, k2};
                    expandAllBoats(s, parent, g, b + 1);
                }
            }
        }
    }
}

State* RiverCrossing::solve() {
    auto t0 = std::chrono::steady_clock::now();
    HeapNode* goalNode = nullptr;
    while (HeapNode* node = open->pop()) {
        if (node->s->isFinal()) { goalNode = node; break; }
        expandAllBoats(node->s, node, node->g, 0);
    }
    if (!goalNode) {
        std::cout << "No se encontró solución.\n";
        return nullptr;
    }
    int length = 0;
    for (HeapNode* cur = goalNode; cur; cur = cur->parent) ++length;
    State** path = new State*[length];
    int idx = length - 1;
    for (HeapNode* cur = goalNode; cur; cur = cur->parent) {
        path[idx--] = cur->s;
    }
    for (int i = 0; i < length; ++i) {
        std::cout << "Paso " << i << ":\n";
        printStateFormatted(path[i]);
    }
    delete[] path;
    auto t1 = std::chrono::steady_clock::now();
    double secs = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Tiempo total: " << secs << " s\n";
    return goalNode->s;
}
