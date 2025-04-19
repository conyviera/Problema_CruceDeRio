#pragma once
#include "State.h"

struct HeapNode {
    State* s;
    int    g, f;
    HeapNode* parent;
    HeapNode* next;
};

class Heap {
    HeapNode** heap;
    int size, top;
public:
    Heap(int cap=1024);
    ~Heap();
    void push(HeapNode* n);
    HeapNode* pop();
};
