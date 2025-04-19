#pragma once
#include "State.h"

class Stack {
    State** arr;
    int cap, top;
public:
    Stack(int capacity=1024);
    ~Stack();
    void push(State* s);
    bool find(State* s) const;
};