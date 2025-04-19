#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
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

    // 1) Leemos número de ítems I
    fin >> State::I;
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // 2) Leemos capacidades C[] en segunda línea
    std::string line;
    std::getline(fin, line);
    int B = 0;
    for (char c : line) if (c == ' ') B++;
    B++;
    State::B = B;
    State::capacity = new int[B];
    {
        std::istringstream iss(line);
        for (int i = 0; i < B; ++i) iss >> State::capacity[i];
    }

    // 3) Leemos viajes máximos V[] en tercera línea
    std::getline(fin, line);
    State::maxTrips = new int[B];
    {
        std::istringstream iss(line);
        for (int i = 0; i < B; ++i) iss >> State::maxTrips[i];
    }

    // 4) Inicializamos matriz de incompatibilidades
    State::incompatible = new bool*[State::I];
    for (int i = 0; i < State::I; ++i) {
        State::incompatible[i] = new bool[State::I]();
    }

    // 5) Leemos pares de incompatibilidad hasta EOF
    int k, j;
    while (fin >> k >> j) {
        State::incompatible[k][j] = true;
        State::incompatible[j][k] = true;
    }

    // Ejecutar A*
    RiverCrossing solver(200000);
    State* sol = solver.solve();
    if (!sol) std::cout << "Sin solución.\n";
    return 0;
}