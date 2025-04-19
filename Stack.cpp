#include "Stack.h"

Stack::Stack(int capacity): cap(capacity), top(0) {
    arr=new State*[cap];
}
Stack::~Stack() { delete[] arr; }

void Stack::push(State* s) {
    if(top<cap) arr[top++]=s;
}
bool Stack::find(State* s) const {
    for(int i=0;i<top;++i)
        if(*s == *arr[i]) return true;
    return false;
}