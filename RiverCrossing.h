#ifndef RIVERCROSSING_H
#define RIVERCROSSING_H

#include "State.h"
#include "Heap.h"
#include "Stack.h"

/**
 * Clase que resuelve el problema de cruce de río con múltiples barcos usando A*.
 */
class RiverCrossing {
public:
    /// Constructor: máximo de estados simultáneos en open.
    RiverCrossing(int maxStates);
    ~RiverCrossing();

    /// Ejecuta A* y devuelve puntero al estado final (o nullptr si no hay solución).
    State* solve();

    /// Para depuración: imprime la solución completa desde el inicial.
    void printSolution(HeapNode* node);
    void printStateFormatted(const State* s);

private:
    Heap* open;          // Cola de prioridad A*
    Stack* closed;       // Estados ya visitados
    int maxStates;

    // Seguridad e heurística
    bool isSafe(const State* s) const;
    int computeHeuristic(State* s) const;

    // Expansión: cruce simultáneo de todos los barcos
    void expandAllBoats(State* s, HeapNode* parent, int g);
    void packBoat(int b, State* s, HeapNode* parent, int g);
    void loadItems(int b, int idx, State* s, HeapNode* parent, int g);
};

#endif // RIVERCROSSING_H
