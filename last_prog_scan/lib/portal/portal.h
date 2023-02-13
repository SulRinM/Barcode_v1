#pragma once
#include <Arduino.h>

class Portal
{
public:
    Portal();
    void init();
    void run_HomePosition(bool state); // портал в поизиции дома
    
protected:
    int statePortal();

    int cnt = 0;
    long Capture = 0;
    long sum = 0;
    bool positionHome = false;
};