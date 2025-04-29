#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>    // std::count, std::fill_n
#include <chrono>
#include "State.h"
#include "RiverCrossing.h"

void cargarDatosDesdeArchivo(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin) {
        throw std::runtime_error("No se puede abrir el archivo: " + filename);
    }

    // Leer número de ítems I
    fin >> State::I;
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Leer línea de capacidades
    std::string line;
    std::getline(fin, line);
    {
        State::B = 1 + static_cast<int>(std::count(line.begin(), line.end(), ' '));
        State::capacity = new int[State::B];
        std::istringstream iss(line);
        for (int b = 0; b < State::B; ++b) {
            iss >> State::capacity[b];
        }
    }

    // Leer línea de viajes máximos
    std::getline(fin, line);
    {
        State::maxTrips = new int[State::B];
        std::istringstream iss(line);
        for (int b = 0; b < State::B; ++b) {
            iss >> State::maxTrips[b];
        }
    }

    // Inicializar matriz de incompatibilidades
    State::incompatible = new bool*[State::I];
    for (int i = 0; i < State::I; ++i) {
        State::incompatible[i] = new bool[State::I];
        std::fill_n(State::incompatible[i], State::I, false);
    }

    // Leer pares de incompatibilidad
    int k, j;
    while (fin >> k >> j) {
        if (k >= 0 && k < State::I && j >= 0 && j < State::I) {
            State::incompatible[k][j] = true;
            State::incompatible[j][k] = true;
        }
    }
}

int main() {
    while (true) {
        std::cout << "\n=== Menu ===\n";
        std::cout << "1. Resolver un archivo\n";
        std::cout << "2. Salir\n";
        std::cout << "Seleccione una opción: ";

        int opcion;
        std::cin >> opcion;

        if (opcion == 2) {
            std::cout << "Saliendo del programa.\n";
            break;
        } else if (opcion == 1) {
            std::string filename;
            std::cout << "Ingrese el nombre del archivo de entrada (ej: input.txt): ";
            std::cin >> filename;

            try {
                cargarDatosDesdeArchivo(filename);

                RiverCrossing solver(2000000);

                auto start = std::chrono::steady_clock::now();
                State* sol = solver.solve();
                auto end = std::chrono::steady_clock::now();

                double tiempo = std::chrono::duration<double>(end - start).count();

                if (!sol) {
                    std::cout << "\nNo se encontró solución para el archivo: " << filename << "\n";
                } else {
                    std::cout << "\nSolución encontrada en " << tiempo << " segundos.\n";
                }
            } catch (const std::exception& e) {
                std::cerr << e.what() << "\n";
            }

        } else {
            std::cout << "\nOpción inválida. Intente nuevamente.\n";
        }
    }

    return 0;
}
