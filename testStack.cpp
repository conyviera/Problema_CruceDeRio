// test_Stack.cpp
#include "Stack.h"
#include <iostream>
using namespace std;

// Configurar un entorno de prueba simple
void setupProblem() {
    State::I = 2;
    State::B = 1;
    State::capacity = new int[State::B]{2};
    State::maxTrips = new int[State::B]{5};

    State::incompatible = new bool*[State::I];
    for (int i = 0; i < State::I; ++i) {
        State::incompatible[i] = new bool[State::I]{false};
    }
}

void cleanupProblem() {
    delete[] State::capacity;
    delete[] State::maxTrips;
    for (int i = 0; i < State::I; ++i)
        delete[] State::incompatible[i];
    delete[] State::incompatible;
}

int main() {
    setupProblem();

    Stack stack;

    // Crear algunos estados
    State* s1 = new State();
    State* s2 = new State();
    
    for (int i = 0; i < State::I; ++i) {
        s1->itemSide[i] = false;
        s2->itemSide[i] = true;
    }
    for (int b = 0; b < State::B; ++b) {
        s1->boatSide[b] = false;
        s2->boatSide[b] = true;
        s1->remTrips[b] = State::maxTrips[b];
        s2->remTrips[b] = State::maxTrips[b];
    }

    // Probar push
    stack.push(s1);

    // Probar find
    cout << "Find s1 (esperado OK): " << (stack.find(s1) ? "OK" : "FAIL") << endl;
    cout << "Find s2 (esperado FAIL): " << (!stack.find(s2) ? "OK" : "FAIL") << endl;

    delete s1;
    delete s2;
    cleanupProblem();

    return 0;
}