#include "State.h"
int State::I;
int State::B;
int* State::capacity;
int* State::maxTrips;
bool** State::incompatible;

State::State() {
    itemSide = new bool[I];
    boatSide = new bool[B];
    remTrips  = new int[B];
}
State::~State() {
    delete[] itemSide;
    delete[] boatSide;
    delete[] remTrips;
}
State* State::clone() const {
    State* n = new State();
    std::memcpy(n->itemSide, itemSide, I*sizeof(bool));
    std::memcpy(n->boatSide, boatSide, B*sizeof(bool));
    std::memcpy(n->remTrips,  remTrips,  B*sizeof(int));
    return n;
}
bool State::isValid() const {
    for(int k=0;k<I;++k) for(int j=k+1;j<I;++j) {
        if (!incompatible[k][j]) continue;
        if (itemSide[k]==itemSide[j]) {
            bool boatThere = false;
            for(int b=0;b<B;++b) if (boatSide[b]==itemSide[k]) { boatThere=true; break; }
            if (!boatThere) return false;
        }
    }
    return true;
}
bool State::isFinal() const {
    for(int k=0;k<I;++k) if(!itemSide[k]) return false;
    return true;
}
State* State::cross(int b, int itemIdx) {
    bool src = boatSide[b];
    bool dst = !src;
    State* n = clone();
    n->boatSide[b] = dst;
    if(--n->remTrips[b]<0) { delete n; return nullptr; }
    if(itemIdx>=0) {
        if(itemSide[itemIdx]!=src) { delete n; return nullptr; }
        n->itemSide[itemIdx] = dst;
    }
    if(!n->isValid()) { delete n; return nullptr; }
    return n;
}
bool State::operator==(State const& o) const {
    if(std::memcmp(itemSide, o.itemSide, I*sizeof(bool))) return false;
    if(std::memcmp(boatSide, o.boatSide, B*sizeof(bool))) return false;
    return true;
}
void State::printState() const {
    std::cout<<"Botes: ";
    for(int b=0;b<B;++b) std::cout<<(boatSide[b]?'R':'L')<<(b<B-1?'/':'\n');
    std::cout<<"Izquierda: "; for(int k=0;k<I;++k) if(!itemSide[k]) std::cout<<k<<' ';
    std::cout<<"\nDerecha:   "; for(int k=0;k<I;++k) if(itemSide[k]) std::cout<<k<<' ';
    std::cout<<"\n---\n";
}

State* State::cross(int b,int k1,int k2){
    if (capacity[b] < 2) return nullptr;
    bool src = boatSide[b], dst = !src;
    if (itemSide[k1]!=src || itemSide[k2]!=src) return nullptr;
    State* n = clone();
    n->boatSide[b]=dst;
    n->itemSide[k1]=dst;
    n->itemSide[k2]=dst;
    if(--n->remTrips[b]<0){ delete n; return nullptr; }
    if(!n->isValid()) { delete n; return nullptr; }
    return n;
}
