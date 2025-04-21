#include "RiverCrossing.h"
#include <iostream>
#include <climits>
#include <chrono>
#include <algorithm>  // std::max

// Ajustes para backtracking sin std::vector
#define MAX_I 100
#define MAX_B 10

static int itemsAvail[MAX_I], nAvail;
static int remCapArr[MAX_B];
static int boatItemsCount[MAX_B];
static int boatItemsArr[MAX_B][MAX_I];
static bool assigned[MAX_I];  // evita tomar un ítem por más de un barco

void RiverCrossing::printStateFormatted(const State* s) {
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

void RiverCrossing::printSolution(HeapNode* node) {
    if (!node) return;
    printSolution(node->parent);
    printStateFormatted(node->s);
}

RiverCrossing::RiverCrossing(int maxStates_)
  : open(new Heap(maxStates_)), closed(new Stack(maxStates_)), maxStates(maxStates_) {
    State* init = new State();
    for (int k = 0; k < State::I; ++k) init->itemSide[k] = false;
    for (int b = 0; b < State::B; ++b) {
        init->boatSide[b] = false;
        init->remTrips[b] = State::maxTrips[b];
    }
    if (init->isValid() && isSafe(init)) {
        int h0 = computeHeuristic(init);
        open->push(new HeapNode{init, 0, h0, nullptr, nullptr});
        closed->push(init);
    } else {
        std::cout << "Estado inicial inválido.\n";
        delete init;
    }
}

RiverCrossing::~RiverCrossing() {
    delete open;
    delete closed;
}

bool RiverCrossing::isSafe(const State* s) const {
    for (int side = 0; side <= 1; ++side) {
        bool boat_here = false;
        for (int b = 0; b < State::B; ++b)
            if (s->boatSide[b] == side) { boat_here = true; break; }
        if (boat_here) continue;
        for (int i = 0; i < State::I; ++i)
            if (s->itemSide[i] == side)
                for (int j = i + 1; j < State::I; ++j)
                    if (s->itemSide[j] == side && State::incompatible[i][j])
                        return false;
    }
    return true;
}

int RiverCrossing::computeHeuristic(State* s) const {
    int remaining = 0;
    for (int k = 0; k < State::I; ++k)
        if (!s->itemSide[k]) ++remaining;
    if (remaining == 0) return 0;

    // Si no hay barcos con viajes restantes, es imposible
    bool anyBoat = false;
    for (int b = 0; b < State::B; ++b)
        if (s->remTrips[b] > 0) { anyBoat = true; break; }
    if (!anyBoat) return INT_MAX;

    // Capacidad máxima de barcos en la orilla de partida
    int maxCap = 0;
    for (int b = 0; b < State::B; ++b) {
        if (!s->boatSide[b] && s->remTrips[b] > 0)
            maxCap = std::max(maxCap, State::capacity[b]);
    }
    if (maxCap == 0) return INT_MAX;

    int trips = (remaining + maxCap - 1) / maxCap;
    // minutos de ida + vuelta menos el último retorno
    return trips + (trips - 1);
}

// Backtracking de carga de ítems para barco b
void RiverCrossing::loadItems(int b, int idx, State* s, HeapNode* parent, int g) {
    if (idx == nAvail) {
        // una configuración de carga lista -> cruce simultáneo
        packBoat(b + 1, s, parent, g);
        return;
    }
    int cur = itemsAvail[idx];
    // Opción: no cargo cur en b
    loadItems(b, idx + 1, s, parent, g);
    // Opción: cargo cur si aún no asignado
    if (!assigned[cur] && boatItemsCount[b] < remCapArr[b]) {
        bool ok = true;
        for (int k = 0; k < boatItemsCount[b]; ++k)
            if (State::incompatible[cur][boatItemsArr[b][k]]) { ok = false; break; }
        if (ok) {
            assigned[cur] = true;
            boatItemsArr[b][boatItemsCount[b]++] = cur;
            loadItems(b, idx + 1, s, parent, g);
            assigned[cur] = false;
            --boatItemsCount[b];
        }
    }
}

// Recursión barco por barco para asignar carga, y al final cruzar todos simultáneamente
void RiverCrossing::packBoat(int b, State* s, HeapNode* parent, int g) {
    if (b < State::B) {
        if (s->remTrips[b] > 0) {
            loadItems(b, 0, s, parent, g);
        } else {
            // sin viajes -> no carga
            packBoat(b + 1, s, parent, g);
        }
        return;
    }
    // b == State::B: todas las cargas asignadas, generar cruce
    bool goes[MAX_I] = {false};
    for (int bb = 0; bb < State::B; ++bb)
        for (int k = 0; k < boatItemsCount[bb]; ++k)
            goes[boatItemsArr[bb][k]] = true;
    State* ns = s->clone();
    bool moved = false;
    // mover barcos con carga o vacíos para reposicionar
    for (int bb = 0; bb < State::B; ++bb) {
        if (ns->remTrips[bb] > 0) {
            ns->boatSide[bb] = !ns->boatSide[bb];
            --ns->remTrips[bb];
            moved = true;
        }
    }
    // mover ítems asignados
    for (int it = 0; it < State::I; ++it)
        if (goes[it]) ns->itemSide[it] = !ns->itemSide[it];
    // push sucesor
    if (moved && ns->isValid() && isSafe(ns) && !closed->find(ns)) {
        int h = computeHeuristic(ns);
        open->push(new HeapNode{ns, g + 1, g + 1 + h, parent, nullptr});
        closed->push(ns);
    } else {
        delete ns;
    }
}

void RiverCrossing::expandAllBoats(State* s, HeapNode* parent, int g) {
    bool startSide = s->boatSide[0];
    nAvail = 0;
    for (int i = 0; i < State::I; ++i) {
        assigned[i] = false;
        if (s->itemSide[i] == startSide)
            itemsAvail[nAvail++] = i;
    }
    for (int b = 0; b < State::B; ++b) {
        remCapArr[b] = (s->remTrips[b] > 0 ? State::capacity[b] : 0);
        boatItemsCount[b] = 0;
    }
    packBoat(0, s, parent, g);
}

State* RiverCrossing::solve() {
    auto t0 = std::chrono::steady_clock::now();
    HeapNode* goalNode = nullptr;
    while (HeapNode* node = open->pop()) {
        if (node->s && node->s->isFinal()) { goalNode = node; break; }
        if (node->s) expandAllBoats(node->s, node, node->g);
    }
    if (!goalNode) {
        std::cout << "No se encontró solución.\n";
        return nullptr;
    }
    // Reconstruir ruta
    int length = 0;
    for (HeapNode* cur = goalNode; cur; cur = cur->parent) ++length;
    State** path = new State*[length];
    int idx = length - 1;
    for (HeapNode* cur = goalNode; cur; cur = cur->parent) {
        path[idx--] = cur->s;
    }
    // Imprimir pasos
    for (int i = 0; i < length; ++i) {
        std::cout << "Paso " << i << ":\n";
        printStateFormatted(path[i]);
    }
    delete[] path;
    auto t1 = std::chrono::steady_clock::now();
    double secs = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Tiempo total: " << secs << "segundos\n" ;
    return goalNode->s;
}
