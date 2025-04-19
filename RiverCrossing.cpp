#include "RiverCrossing.h"
#include "State.h"
#include <vector>
#include <cmath>

// Heurística con matching aproximado (se podría optimizar a bitmáscara)
static int computeHeuristic(State* s) {
    int R=0;
    for(int k=0;k<State::I;++k) if(!s->itemSide[k]) ++R;
    int kmax=0;
    for(int b=0;b<State::B;++b) if(State::capacity[b]>kmax) kmax=State::capacity[b];
    if(R==0) return 0;
    int M=(R + kmax - 1)/kmax;
    return 2*M - 1;
}

RiverCrossing::RiverCrossing(int m): maxStates(m) {
    open = new Heap(m);
    closed = new Stack(m);
}
RiverCrossing::~RiverCrossing(){ delete open; delete closed; }

State* RiverCrossing::solve(){
    State* init=new State();
    for(int k=0;k<State::I;++k) init->itemSide[k]=false;
    for(int b=0;b<State::B;++b){ init->boatSide[b]=false; init->remTrips[b]=State::maxTrips[b]; }

    int h0=computeHeuristic(init);
    open->push(new HeapNode{init,0,h0,nullptr,nullptr});
    closed->push(init);

    while(HeapNode* node=open->pop()){
        State* s=node->s;
        int g=node->g;
        if(s->isFinal()){
            // reconstrucción ruta
            std::vector<State*> path;
            for(HeapNode* cur=node;cur;cur=cur->parent) path.push_back(cur->s);
            for(auto it=path.rbegin();it!=path.rend();++it) (*it)->printState();
            return s;
        }
        for(int b=0;b<State::B;++b){
            if(State* ns=s->cross(b,-1)){
                if(!closed->find(ns)){
                    int h=computeHeuristic(ns);
                    open->push(new HeapNode{ns,g+1,g+1+h,node,nullptr});
                    closed->push(ns);
                }
            }
            for(int k=0;k<State::I;++k){
                if(State* ns2=s->cross(b,k)){
                    if(!closed->find(ns2)){
                        int h=computeHeuristic(ns2);
                        open->push(new HeapNode{ns2,g+1,g+1+h,node,nullptr});
                        closed->push(ns2);
                    }
                }
            }
        }
    }
    return nullptr;
}