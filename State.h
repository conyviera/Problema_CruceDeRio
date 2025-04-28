// State.h
#pragma once
#include <cstring>
#include <iostream>

class State {
public:
    static int I, B;
    static int* capacity;
    static int* maxTrips;
    static bool** incompatible;

    bool* itemSide;
    bool* boatSide;
    int*  remTrips;

    // Descripcion: Constructor de la clase State. Inicializa las variables dinámicas.
    // Entrada: Ninguna.
    // Salida: Ninguna.
    State();

    // Descripcion: Destructor de la clase State. Libera memoria.
    // Entrada: Ninguna.
    // Salida: Ninguna.
    ~State();

    // Descripcion: Crea una copia idéntica del estado actual.
    // Entrada: Ninguna.
    // Salida: Puntero a nuevo objeto State.
    State* clone() const;

    // Descripcion: Verifica si el estado actual es válido (sin conflictos).
    // Entrada: Ninguna.
    // Salida: true si es válido, false si no.
    bool isValid() const;

    // Descripcion: Verifica si el estado es final (todos los ítems cruzaron).
    // Entrada: Ninguna.
    // Salida: true si es estado final, false si no.
    bool isFinal() const;

    // Descripcion: Realiza un cruce de un bote moviendo uno o cero ítems.
    // Entrada: Indice del bote, indice del ítem.
    // Salida: Nuevo estado resultante, o nullptr si es inválido.
    State* cross(int boatIdx, int itemIdx);

    // Descripcion: Realiza un cruce de un bote moviendo dos ítems.
    // Entrada: Indice del bote, índice del primer ítem, índice del segundo ítem.
    // Salida: Nuevo estado resultante, o nullptr si es inválido.
    State* cross(int boatIdx, int k1, int k2);

    // Descripcion: Compara si dos estados son iguales.
    // Entrada: Otro objeto State.
    // Salida: true si son iguales, false si no.
    bool operator==(State const& o) const;

    // Descripcion: Imprime el estado actual en consola.
    // Entrada: Ninguna.
    // Salida: Ninguna.
    void printState() const;

    unsigned long long getHash() const;

};


