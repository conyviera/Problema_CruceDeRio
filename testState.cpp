// test_State.cpp
#include "State.h"
#include <iostream>
using namespace std;

// Configurar el problema de prueba
void setupProblem() {
    State::I = 2;  // Dos ítems
    State::B = 1;  // Un bote
    State::capacity = new int[State::B]{2};
    State::maxTrips = new int[State::B]{5};

    // Inicializar matriz de incompatibilidades (todos compatibles)
    State::incompatible = new bool*[State::I];
    for (int i = 0; i < State::I; ++i) {
        State::incompatible[i] = new bool[State::I]{false};
    }
}

// Liberar memoria al final del test
void cleanupProblem() {
    delete[] State::capacity;
    delete[] State::maxTrips;
    for (int i = 0; i < State::I; ++i) delete[] State::incompatible[i];
    delete[] State::incompatible;
}

int main() {
    setupProblem();

    // Crear estado inicial
    State* initial = new State();
    for (int i = 0; i < State::I; ++i) initial->itemSide[i] = false; // Todos a la izquierda
    for (int b = 0; b < State::B; ++b) {
        initial->boatSide[b] = false; // Bote a la izquierda
        initial->remTrips[b] = State::maxTrips[b];
    }

    // Test clone()
    State* copy = initial->clone();
    cout << "Clone Test: " << ((*initial == *copy) ? "OK" : "FAIL") << endl;

    // Test isValid()
    cout << "IsValid Test: " << (initial->isValid() ? "OK" : "FAIL") << endl;

    // Test isFinal()
    cout << "IsFinal (initial) Test: " << (!initial->isFinal() ? "OK" : "FAIL") << endl;

    // Test cross() un ítem
    State* movedOne = initial->cross(0, 0);
    if (movedOne) {
        cout << "Cross one item Test: OK" << endl;
        movedOne->printState();
    } else {
        cout << "Cross one item Test: FAIL" << endl;
    }

    // Test cross() dos ítems
    State* movedTwo = initial->cross(0, 0, 1);
    if (movedTwo) {
        cout << "Cross two items Test: OK" << endl;
        movedTwo->printState();
    } else {
        cout << "Cross two items Test: FAIL" << endl;
    }

    // Liberar memoria
    delete initial;
    delete copy;
    delete movedOne;
    delete movedTwo;

    cleanupProblem();
    return 0;
}
