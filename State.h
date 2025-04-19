#pragma once
#include <cstring>
#include <iostream>

class State {
public:
    static int I, B;
    static int* capacity;
    static int* maxTrips;
    static bool** incompatible;

    bool* itemSide;
    bool* boatSide;
    int*  remTrips;

    State();
    ~State();
    State* clone() const;
    bool   isValid() const;
    bool   isFinal() const;
    State* cross(int boatIdx, int itemIdx);
    State* cross(int boatIdx,int k1,int k2); 
    bool   operator==(State const& o) const;
    void   printState() const;
};
