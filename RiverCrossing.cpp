#include "RiverCrossing.h"
#include <climits>

/* ============ 1.  Funciones auxiliares sin ‹vector› ============ */

static inline int maxCap() {
    int m = 0;
    for (int b = 0; b < State::B; ++b)
        if (State::capacity[b] > m) m = State::capacity[b];
    return m;
}

/* -------- isSafe: nadie incompatible queda solo sin bote -------- */
bool RiverCrossing::isSafe(const State* s) const {
    /* Revisar orilla izquierda (false) y derecha (true) */
    for (int side = 0; side <= 1; ++side) {
        /* ¿hay algún bote anclado en esa orilla? */
        bool boat_here = false;
        for (int b = 0; b < State::B && !boat_here; ++b)
            if (s->boatSide[b] == side) boat_here = true;

        if (boat_here) continue;              // supervisado

        /* Sin bote: no pueden quedar pares incompatibles juntos */
        for (int i = 0; i < State::I; ++i)
            if (s->itemSide[i] == side)
                for (int j = i + 1; j < State::I; ++j)
                    if (s->itemSide[j] == side &&
                        State::incompatible[i][j])
                        return false;
    }
    return true;
}

/* -------- heurística sin ‹vector› / ‹algorithm›  ---------------- */
int RiverCrossing::computeHeuristic(State* s) {
    const int INF = INT_MAX;

    /* 1) cuántos ítems quedan en la orilla inicial */
    int remaining_items = 0;
    for (int k = 0; k < State::I; ++k)
        if (!s->itemSide[k]) ++remaining_items;
    if (remaining_items == 0) return 0;

    /* 2) ¿hay al menos un bote disponible en la orilla inicial? */
    bool boat_available = false;
    int kmax = 0;
    for (int b = 0; b < State::B; ++b)
        if (!s->boatSide[b] && s->remTrips[b] > 0) {
            boat_available = true;
            if (State::capacity[b] > kmax) kmax = State::capacity[b];
        }
    if (!boat_available) return INF;

    /* 3) viajes necesarios aún (ida + posibles vueltas)            */
    int trips = (remaining_items + kmax - 1) / kmax;      // >=1
    if (!isSafe(s)) trips += 10;                          // penaliza

    return 2 * trips - 1;
}

/* ================== 2.  Constructor / destructor =============== */

RiverCrossing::RiverCrossing(int m) : maxStates(m) {
    open   = new Heap(m);
    closed = new Stack(m);
}
RiverCrossing::~RiverCrossing() { delete open; delete closed; }

/* ====================== 3.  Solución A* ========================= */

State* RiverCrossing::solve() {

    /* ---- estado inicial (todo a la izquierda) ---- */
    State* init = new State();
    for (int k = 0; k < State::I; ++k)  init->itemSide[k] = false;
    for (int b = 0; b < State::B; ++b) {
        init->boatSide[b] = false;
        init->remTrips[b] = State::maxTrips[b];
    }
    if (!init->isValid() || !isSafe(init)) {
        std::cout << "Estado inicial invalido.\\n";
        delete init; return nullptr;
    }

    int h0 = computeHeuristic(init);
    open->push(new HeapNode{init, 0, h0, nullptr, nullptr});
    closed->push(init);

    State* best_solution = nullptr;
    int    best_cost     = INT_MAX;
    int states_explored  = 0;

    while (HeapNode* node = open->pop()) {
        ++states_explored;
        State* s = node->s;
        int g    = node->g;

        if (g >= best_cost) { delete s; continue; }

        if (s->isFinal()) {
            best_cost     = g;
            best_solution = s;

            /* ------- imprimir ruta ----------- */
            int length = 0;
            for (HeapNode* cur = node; cur; cur = cur->parent) ++length;

            State** path = new State*[length];
            int idx = length - 1;
            for (HeapNode* cur = node; cur; cur = cur->parent)
                path[idx--] = cur->s;

            std::cout << "n SOLUCIÓN (" << g
                      << " cruces, estados exp. " << states_explored << ") \\n";
            for (int i = 0; i < length; ++i) {
                std::cout << "Paso " << i << ":\n";
                path[i]->printState();
            }
            delete[] path;
            break;                         // primera óptima
        }

        /* ------- EXPANSIÓN de sucesores ------- */ 
        for (int b = 0; b < State::B; ++b) {

            if (s->remTrips[b] == 0) continue;          // sin combustible
            bool boat_side = s->boatSide[b];

            /* --- cruce vacío --- */
            if (State* ns = s->cross(b, -1)) {
                if (!closed->find(ns) && ns->isValid() && isSafe(ns)) {
                    int h = computeHeuristic(ns);
                    open->push(new HeapNode{ns, g + 1, g + 1 + h, node, nullptr});
                    closed->push(ns);
                } else delete ns;
            }

            /* --- con 1 item --- */
            for (int k = 0; k < State::I; ++k)
                if (s->itemSide[k] == boat_side)             // en el bote
                    if (State* ns = s->cross(b, k)) {
                        if (!closed->find(ns) && ns->isValid() && isSafe(ns)) {
                            int h = computeHeuristic(ns);
                            open->push(new HeapNode{ns, g + 1, g + 1 + h, node, nullptr});
                            closed->push(ns);
                        } else delete ns;
                    }

            /* --- con 2 items si capacidad >=2 --- */
            if (State::capacity[b] >= 2) {
                for (int k1 = 0; k1 < State::I; ++k1)
                    if (s->itemSide[k1] == boat_side)
                        for (int k2 = k1 + 1; k2 < State::I; ++k2)
                            if (s->itemSide[k2] == boat_side &&
                                !State::incompatible[k1][k2])
                                if (State* ns = s->cross(b, k1, k2)) {
                                    if (!closed->find(ns) && ns->isValid() && isSafe(ns)) {
                                        int h = computeHeuristic(ns);
                                        open->push(new HeapNode{ns, g + 1, g + 1 + h, node, nullptr});
                                        closed->push(ns);
                                    } else delete ns;
                                }
            }
        }
    }

    if (!best_solution)
    std::cout << "No se encontró solución tras " << states_explored << " estados.\n";
    return best_solution;
}
