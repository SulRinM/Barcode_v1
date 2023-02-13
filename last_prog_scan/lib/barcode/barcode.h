#pragma once
#include <Arduino.h>

class Barcode
{
public:
    Barcode();
    void init();
    void clear();
    void debugData();
protected:
    long arr[2][24];
    int counter_left = 0;
    int counter_right = 0;
    long rack_id = 0;
};