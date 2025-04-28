// Heap.h
#pragma once
#include "State.h"

// Estructura que representa un nodo del heap
struct HeapNode {
    State* s;           // Estado asociado al nodo
    int    g;           // Costo real acumulado
    int    f;           // Costo total estimado (g + heurístico)
    HeapNode* parent;   // Puntero al padre en el árbol de estados
    HeapNode* next;     // Siguiente nodo en lista (posible uso en grafos)
};

// Clase Heap: implementa un heap mínimo basado en el valor 'f' de los HeapNode
class Heap {
    HeapNode** heap; // Arreglo de punteros a HeapNode
    int size;        // Capacidad actual del heap
    int top;         // Cantidad de elementos actuales

public:
    // Descripción: Constructor de Heap. Inicializa el heap con una capacidad inicial.
    // Entrada: cap (capacidad inicial, por defecto 1024).
    // Salida: Ninguna.
    Heap(int cap=1024);

    // Descripción: Destructor de Heap. Libera la memoria del arreglo.
    // Entrada: Ninguna.
    // Salida: Ninguna.
    ~Heap();

    // Descripción: Inserta un nodo en el heap manteniendo la propiedad de heap mínimo.
    // Entrada: Puntero a HeapNode n.
    // Salida: Ninguna.
    void push(HeapNode* n);

    // Descripción: Extrae el nodo de menor prioridad (menor valor f) del heap.
    // Entrada: Ninguna.
    // Salida: Puntero al HeapNode extraído, o nullptr si está vacío.
    HeapNode* pop();
};
