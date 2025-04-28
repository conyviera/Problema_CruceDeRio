// Stack.cpp
#include "Stack.h"

// Descripcion: Constructor. Inicializa el arreglo de la pila con la capacidad dada.
// Entrada: capacity (capacidad inicial de la pila)
// Salida: Ninguna.
Stack::Stack(int capacity) : cap(capacity), top(0) {
    arr = new State*[cap];
}

// Descripcion: Destructor. Libera la memoria asignada para el arreglo de la pila.
// Entrada: Ninguna.
// Salida: Ninguna.
Stack::~Stack() {
    delete[] arr;
}

// Descripcion: Agrega un nuevo puntero a State en la cima de la pila.
// Entrada: Puntero a objeto State (s).
// Salida: Ninguna.
void Stack::push(State* s) {
    if (top < cap)
        arr[top++] = s;
}

// Descripcion: Busca si un objeto State ya existe en la pila.
// Entrada: Puntero a objeto State (s).
// Salida: true si se encuentra, false si no se encuentra.
bool Stack::find(State* s) const {
    for (int i = 0; i < top; ++i)
        if (*s == *arr[i])
            return true;
    return false;
}
