#include "Heap.h"
#include <cstring>

Heap::Heap(int cap): size(cap), top(0) {
    heap = new HeapNode*[size];
}
Heap::~Heap() { delete[] heap; }

void Heap::push(HeapNode* n) {
    if(top==size) {
        int newSize = size*2;
        auto* tmp = new HeapNode*[newSize];
        std::memcpy(tmp, heap, size*sizeof(HeapNode*));
        delete[] heap;
        heap=tmp; size=newSize;
    }
    int i=top++;
    while(i>0) {
        int p=(i-1)/2;
        if(heap[p]->f <= n->f) break;
        heap[i]=heap[p]; i=p;
    }
    heap[i]=n;
}

HeapNode* Heap::pop() {
    if(top==0) return nullptr;
    HeapNode* ret=heap[0];
    HeapNode* last=heap[--top];
    int i=0;
    while(true){
        int l=2*i+1, r=2*i+2, m=i;
        if(l<top && heap[l]->f < last->f) m=l;
        if(r<top && heap[r]->f < heap[m]->f) m=r;
        if(m==i) break;
        heap[i]=heap[m]; i=m;
    }
    heap[i]=last;
    return ret;
}