#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>    // std::count, std::fill_n
#include "State.h"
#include "RiverCrossing.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " input.txt\n";
        return 1;
    }
    std::ifstream fin(argv[1]);
    if (!fin) {
        std::cerr << "No se puede abrir " << argv[1] << "\n";
        return 1;
    }

    // 1) Leer número de ítems I
    fin >> State::I;
    // Pasar al comienzo de la siguiente línea
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // 2) Leer línea de capacidades y determinar B
    std::string line;
    std::getline(fin, line);  // línea con C0 C1 ... C{B-1}
    {
        // Contar espacios para saber cuántos botes hay
        State::B = 1 + static_cast<int>(std::count(line.begin(), line.end(), ' '));
        State::capacity = new int[State::B];
        std::istringstream iss(line);
        for (int b = 0; b < State::B; ++b) {
            iss >> State::capacity[b];
        }
    }

    // 3) Leer línea de viajes máximos
    std::getline(fin, line);
    {
        State::maxTrips = new int[State::B];
        std::istringstream iss(line);
        for (int b = 0; b < State::B; ++b) {
            iss >> State::maxTrips[b];
        }
    }

    // 4) Inicializar matriz de incompatibilidades a false
    State::incompatible = new bool*[State::I];
    for (int i = 0; i < State::I; ++i) {
        State::incompatible[i] = new bool[State::I];
        std::fill_n(State::incompatible[i], State::I, false);
    }

    // 5) Leer pares de incompatibilidad hasta EOF
    int k, j;
    while (fin >> k >> j) {
        if (k >= 0 && k < State::I && j >= 0 && j < State::I) {
            State::incompatible[k][j] = true;
            State::incompatible[j][k] = true;
        }
    }
    std::cout << "SE COMENZO A EJECUTAR\n";
    // 6) Ejecutar A*
    RiverCrossing solver(2000000);
    State* sol = solver.solve();
    if (!sol) {
        std::cout << "Sin solución.\n";
    }
    return 0;
}
