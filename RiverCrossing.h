// RiverCrossing.h
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
    // Descripcion: Constructor. Inicializa la estructura open/closed para A*.
    // Entrada: maxStates (capacidad máxima de estados simultáneos).
    // Salida: Ninguna.
    RiverCrossing(int maxStates);

    // Descripcion: Destructor. Libera memoria utilizada.
    // Entrada: Ninguna.
    // Salida: Ninguna.
    ~RiverCrossing();

    // Descripcion: Ejecuta el algoritmo A* y encuentra una solución.
    // Entrada: Ninguna.
    // Salida: Puntero al estado final encontrado o nullptr si no hay solución.
    State* solve();

    // Descripcion: Imprime toda la secuencia de estados que forman la solución.
    // Entrada: Puntero al nodo final.
    // Salida: Ninguna.
    void printSolution(HeapNode* node);

    // Descripcion: Imprime el estado de forma formateada.
    // Entrada: Puntero al estado.
    // Salida: Ninguna.
    void printStateFormatted(const State* s);

private:
    Heap* open;          // Cola de prioridad A*
    Stack* closed;       // Estados ya visitados
    int maxStates;       // Tamaño máximo de estados

    // Descripcion: Verifica si un estado es seguro (no deja ítems incompatibles).
    // Entrada: Puntero al estado.
    // Salida: true si es seguro, false si no.
    bool isSafe(const State* s) const;

    // Descripcion: Calcula la heurística de un estado (número de movimientos estimados).
    // Entrada: Puntero al estado.
    // Salida: Valor heurístico.
    int computeHeuristic(State* s) const;

    // Descripcion: Genera los sucesores cargando todos los barcos posibles.
    // Entrada: Estado actual, nodo padre, costo actual g.
    // Salida: Ninguna.
    void expandAllBoats(State* s, HeapNode* parent, int g);

    // Descripcion: Intenta empaquetar ítems en el barco "b" y luego seguir con el siguiente.
    // Entrada: Indice de barco, estado, nodo padre, costo actual.
    // Salida: Ninguna.
    void packBoat(int b, State* s, HeapNode* parent, int g);

    // Descripcion: Opciones de cargar o no cargar ítems individuales en un barco.
    // Entrada: Indice de barco, indice de item, estado, nodo padre, costo actual.
    // Salida: Ninguna.
    void loadItems(int b, int idx, State* s, HeapNode* parent, int g);
};

#endif // RIVERCROSSING_H
