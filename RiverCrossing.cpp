#include "RiverCrossing.h"
#include <climits>
#include <iostream>

/* ------------ CONSTANTE de botes máximo (si no está) ----------- */
#ifndef MAXB
#define MAXB 8
#endif

/* ---------- tabla auxiliar para sucesores ---------------- */
struct TmpMove { int k1, k2; };         // -2= bote no se mueve (-1,-1)=vacío ; k2=-1 ⇒ solo k1
static TmpMove choice[MAXB];            // elección para cada bote

/* ---- aplica simultáneamente los movimientos elegidos ---- */
void RiverCrossing::applyAndPush(State* s, HeapNode* parent,
    int g, int uptoBoat)
{
State* ns = s->clone();

for (int b = 0; b <= uptoBoat; ++b) {

/* ---- ❶  bote se queda donde está -------------- */
if (choice[b].k1 == -2) continue;

/* ---- ❷  el bote cruza con la carga elegida ---- */
ns->boatSide[b] ^= 1;           // cambia de orilla
--ns->remTrips[b];              // gasta un viaje

if (choice[b].k1 >= 0)
ns->itemSide[choice[b].k1] ^= 1;  // mueve 1er ítem
if (choice[b].k2 >= 0)
ns->itemSide[choice[b].k2] ^= 1;  // mueve 2º ítem
}

/* validaciones y push como antes */
if (!ns->isValid() || !isSafe(ns)) { delete ns; return; }
if (!closed->find(ns)) {
int h = computeHeuristic(ns);
open->push(new HeapNode{ns, g+1, g+1+h, parent, nullptr});
closed->push(ns);
} else delete ns;
}


/* --------- recursión sobre cada bote --------------------- */
void RiverCrossing::expandAllBoats(State* s, HeapNode* parent, int g, int b)
{
    if (b == State::B) { applyAndPush(s, parent, g, b-1); return; }

    if (s->remTrips[b] == 0) {            // bote sin combustible
        choice[b] = {-2,-2};
        this->expandAllBoats(s,parent,g,b+1);
        return;
    }

    bool side = s->boatSide[b];

    /* 0 ítems */
    choice[b] = {-1,-1};
    this->expandAllBoats(s,parent,g,b+1);

    /* 1 ítem */
    for (int k = 0; k < State::I; ++k)
        if (s->itemSide[k] == side) {
            choice[b] = {k,-1};
            this->expandAllBoats(s,parent,g,b+1);
        }

    /* 2 ítems si cabe */
    if (State::capacity[b] >= 2)
        for (int k1 = 0; k1 < State::I; ++k1)
            if (s->itemSide[k1] == side)
                for (int k2 = k1+1; k2 < State::I; ++k2)
                    if (s->itemSide[k2] == side &&
                        !State::incompatible[k1][k2]) {
                        choice[b] = {k1,k2};
                        expandAllBoats(s,parent,g,b+1);
                    }
}

/* -------- isSafe (igual que antes) ---------------------- */
bool RiverCrossing::isSafe(const State* s) const {
    for (int side=0; side<=1; ++side) {
        bool boat=false;
        for (int b=0;b<State::B && !boat;++b)
            if (s->boatSide[b]==side) boat=true;
        if (boat) continue;
        for (int i=0;i<State::I;++i) if (s->itemSide[i]==side)
            for (int j=i+1;j<State::I;++j)
                if (s->itemSide[j]==side && State::incompatible[i][j])
                    return false;
    }
    return true;
}

/* ---------------- heurística simple --------------------- */
int RiverCrossing::computeHeuristic(State* s) {
    const int INF = INT_MAX;
    int remaining=0;
    for(int k=0;k<State::I;++k) if(!s->itemSide[k]) ++remaining;
    if (!remaining) return 0;
    bool boat=false; int kmax=0;
    for(int b=0;b<State::B;++b)
        if(!s->boatSide[b] && s->remTrips[b]>0){
            boat=true;
            if(State::capacity[b]>kmax) kmax=State::capacity[b];
        }
    if(!boat) return INF;
    int trips=(remaining+kmax-1)/kmax;
    if(!isSafe(s)) trips+=10;
    return 2*trips-1;
}

/* --------------- ctor / dtor ---------------------------- */
RiverCrossing::RiverCrossing(int m):maxStates(m){
    open=new Heap(m); closed=new Stack(m);
}
RiverCrossing::~RiverCrossing(){ delete open; delete closed; }

/* -------------------- A* main --------------------------- */
State* RiverCrossing::solve() {

    State* init=new State();
    for(int k=0;k<State::I;++k) init->itemSide[k]=false;
    for(int b=0;b<State::B;++b){ init->boatSide[b]=false;
                                 init->remTrips[b]=State::maxTrips[b]; }
    if(!init->isValid()||!isSafe(init)){ std::cout<<"Estado inicial inválido\n";
        delete init; return nullptr; }

    open->push(new HeapNode{init,0,computeHeuristic(init),nullptr,nullptr});
    closed->push(init);

    State* best=nullptr; int bestCost=INT_MAX, expanded=0;

    while(HeapNode* node=open->pop()){
        ++expanded;
        State* s=node->s; int g=node->g;
        if(g>=bestCost){ delete s; continue; }

        if(s->isFinal()){
            best=s; bestCost=g;
            int len=0; for(HeapNode* c=node;c;c=c->parent) ++len;
            State** path=new State*[len];
            for(HeapNode* c=node;c;c=c->parent) path[--len]=c->s;

            std::cout<<"\nSOLUCIÓN ("<<g<<" cruces, "<<expanded<<" estados)\n";
            for(int i=0;i<State::I*10 && path[i];++i){
                std::cout<<"Paso "<<i<<":\n"; path[i]->printState();
            }
            delete[] path; break;
        }
        /* --- nueva expansión simultánea --- */
        this->expandAllBoats(s,node,g,0);
    }

    if(!best)
        std::cout<<"No se encontró solución tras "<<expanded<<" estados.\n";
    return best;
}
