// RiverCrossing.cpp
#include "RiverCrossing.h"
#include <iostream>
#include <climits>
#include <chrono>
#include <algorithm>  // std::max

// Variables auxiliares para la carga de barcos
#define MAX_I 100
#define MAX_B 10

static int itemsAvail[MAX_I], nAvail;
static int remCapArr[MAX_B];
static int boatItemsCount[MAX_B];
static int boatItemsArr[MAX_B][MAX_I];
static bool assigned[MAX_I];

// Descripcion: Imprime un estado en formato legible.
// Entrada: Puntero al estado.
// Salida: Ninguna.
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

// Descripcion: Imprime el camino completo desde el estado inicial hasta el final.
// Entrada: Puntero al HeapNode final.
// Salida: Ninguna.
void RiverCrossing::printSolution(HeapNode* node) {
    if (!node) return;
    printSolution(node->parent);
    printStateFormatted(node->s);
}

// Descripcion: Constructor. Inicializa el heap y la pila para A*.
// Entrada: maxStates (cantidad máxima de estados).
// Salida: Ninguna.
RiverCrossing::RiverCrossing(int maxStates_) : open(new Heap(maxStates_)), closed(new Stack(maxStates_)), maxStates(maxStates_) {
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

// Descripcion: Destructor. Libera memoria utilizada.
// Entrada: Ninguna.
// Salida: Ninguna.
RiverCrossing::~RiverCrossing() {
    delete open;
    delete closed;
}

// Descripcion: Verifica que un estado no tenga conflictos de ítems incompatibles sin supervisión.
// Entrada: Puntero a estado.
// Salida: true si es seguro, false si no.
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

// Descripcion: Calcula una heurística aproximada de movimientos restantes.
// Entrada: Puntero a estado.
// Salida: Valor entero heurístico.
int RiverCrossing::computeHeuristic(State* s) const {
    int remaining = 0;
    for (int k = 0; k < State::I; ++k)
        if (!s->itemSide[k]) ++remaining;
    if (remaining == 0) return 0;

    bool anyBoat = false;
    for (int b = 0; b < State::B; ++b)
        if (s->remTrips[b] > 0) { anyBoat = true; break; }
    if (!anyBoat) return INT_MAX;

    int maxCap = 0;
    for (int b = 0; b < State::B; ++b)
        if (!s->boatSide[b] && s->remTrips[b] > 0)
            maxCap = std::max(maxCap, State::capacity[b]);
    if (maxCap == 0) return INT_MAX;

    int trips = (remaining + maxCap - 1) / maxCap;
    return trips + (trips - 1);
}

// Descripcion: Intenta cargar ítems en el barco "b" desde el índice actual.
// Entrada: índice de barco, índice de ítem, estado actual, nodo padre, costo actual.
// Salida: Ninguna.
void RiverCrossing::loadItems(int b, int idx, State* s, HeapNode* parent, int g) {
    if (idx == nAvail) {
        packBoat(b + 1, s, parent, g);
        return;
    }
    int cur = itemsAvail[idx];
    loadItems(b, idx + 1, s, parent, g);
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

// Descripcion: Gestiona la asignación de carga de barcos de manera recursiva.
// Entrada: índice de barco, estado actual, nodo padre, costo actual.
// Salida: Ninguna.
void RiverCrossing::packBoat(int b, State* s, HeapNode* parent, int g) {
    if (b < State::B) {
        if (s->remTrips[b] > 0)
            loadItems(b, 0, s, parent, g);
        else
            packBoat(b + 1, s, parent, g);
        return;
    }
    bool goes[MAX_I] = {false};
    for (int bb = 0; bb < State::B; ++bb)
        for (int k = 0; k < boatItemsCount[bb]; ++k)
            goes[boatItemsArr[bb][k]] = true;
    State* ns = s->clone();
    bool moved = false;
    for (int bb = 0; bb < State::B; ++bb) {
        if (ns->remTrips[bb] > 0) {
            ns->boatSide[bb] = !ns->boatSide[bb];
            --ns->remTrips[bb];
            moved = true;
        }
    }
    for (int it = 0; it < State::I; ++it)
        if (goes[it]) ns->itemSide[it] = !ns->itemSide[it];

    if (moved && ns->isValid() && isSafe(ns) && !closed->find(ns)) {
        int h = computeHeuristic(ns);
        open->push(new HeapNode{ns, g + 1, g + 1 + h, parent, nullptr});
        closed->push(ns);
    } else {
        delete ns;
    }
}

// Descripcion: Prepara todo para expandir posibles movimientos simultáneos de barcos.
// Entrada: Estado actual, nodo padre, costo actual.
// Salida: Ninguna.
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

// Descripcion: Algoritmo A* para encontrar la solución del cruce de río.
// Entrada: Ninguna.
// Salida: Estado final encontrado o nullptr si no hay solución.
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

    int length = 0;
    for (HeapNode* cur = goalNode; cur; cur = cur->parent) ++length;
    State** path = new State*[length];
    int idx = length - 1;
    for (HeapNode* cur = goalNode; cur; cur = cur->parent)
        path[idx--] = cur->s;

    for (int i = 0; i < length; ++i) {
        std::cout << "Paso " << i << ":\n";
        printStateFormatted(path[i]);
    }
    delete[] path;

    auto t1 = std::chrono::steady_clock::now();
    double secs = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Tiempo total: " << secs << " segundos\n";
    return goalNode->s;
}
