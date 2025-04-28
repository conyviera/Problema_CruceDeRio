// Stack.h
#pragma once
#include "State.h"

class Stack {
    State** arr;  // Arreglo de punteros a objetos State (pila)
    int cap;      // Capacidad máxima de la pila
    int top;      // Posición del último elemento insertado
public:
    // Descripcion: Constructor de la clase Stack. Inicializa el arreglo con capacidad dada.
    // Entrada: capacity (capacidad inicial de la pila, por defecto 1024)
    // Salida: Ninguna.
    Stack(int capacity = 1024);

    // Descripcion: Destructor. Libera la memoria utilizada por la pila.
    // Entrada: Ninguna.
    // Salida: Ninguna.
    ~Stack();

    // Descripcion: Agrega un nuevo puntero a State en la cima de la pila.
    // Entrada: Puntero a un objeto State (s).
    // Salida: Ninguna.
    void push(State* s);

    // Descripcion: Busca si un State ya existe en la pila (comparación de igualdad).
    // Entrada: Puntero a un objeto State (s).
    // Salida: true si existe, false si no existe.
    bool find(State* s) const;
};
