// test_RiverCrossing.cpp
#include "RiverCrossing.h"
#include <iostream>
using namespace std;

// Configurar un problema simple para la prueba
void setupProblem() {
    State::I = 2; // Dos ítems
    State::B = 1; // Un bote
    State::capacity = new int[State::B]{2};
    State::maxTrips = new int[State::B]{5};

    State::incompatible = new bool*[State::I];
    for (int i = 0; i < State::I; ++i) {
        State::incompatible[i] = new bool[State::I]{false};
    }
}

// Liberar memoria luego de la prueba
void cleanupProblem() {
    delete[] State::capacity;
    delete[] State::maxTrips;
    for (int i = 0; i < State::I; ++i) delete[] State::incompatible[i];
    delete[] State::incompatible;
}

int main() {
    setupProblem();

    RiverCrossing solver(1024);
    State* finalState = solver.solve();

    if (finalState) {
        cout << "\nTest de RiverCrossing: Solución encontrada. OK\n";
    } else {
        cout << "\nTest de RiverCrossing: No se encontró solución. FAIL\n";
    }

    cleanupProblem();
    return 0;
}
