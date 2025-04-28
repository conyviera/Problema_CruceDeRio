// test_Heap.cpp
#include "Heap.h"
#include <iostream>
using namespace std;

int main() {
    // Crear un heap
    Heap heap(4);

    // Crear algunos HeapNode manualmente
    HeapNode* n1 = new HeapNode{nullptr, 0, 10, nullptr, nullptr};
    HeapNode* n2 = new HeapNode{nullptr, 0, 5, nullptr, nullptr};
    HeapNode* n3 = new HeapNode{nullptr, 0, 8, nullptr, nullptr};
    HeapNode* n4 = new HeapNode{nullptr, 0, 3, nullptr, nullptr};

    // Insertar nodos
    heap.push(n1);
    heap.push(n2);
    heap.push(n3);
    heap.push(n4);

    // Extraer nodos en orden de prioridad (f más bajo primero)
    HeapNode* popped;

    popped = heap.pop();
    cout << "Primer pop (esperado f=3): " << (popped->f == 3 ? "OK" : "FAIL") << endl;

    popped = heap.pop();
    cout << "Segundo pop (esperado f=5): " << (popped->f == 5 ? "OK" : "FAIL") << endl;

    popped = heap.pop();
    cout << "Tercer pop (esperado f=8): " << (popped->f == 8 ? "OK" : "FAIL") << endl;

    popped = heap.pop();
    cout << "Cuarto pop (esperado f=10): " << (popped->f == 10 ? "OK" : "FAIL") << endl;

    popped = heap.pop();
    cout << "Quinto pop (esperado nullptr): " << (popped == nullptr ? "OK" : "FAIL") << endl;

    // Liberar memoria
    delete n1;
    delete n2;
    delete n3;
    delete n4;

    return 0;
}
