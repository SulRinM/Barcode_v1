#include "button.h"

bool TKeyboard::buttonStart()
{
    bool result;
    int adc = analogRead(A0);
    if (adc > 140 && adc < 160)
    {
        result = true;
    }
    else
        result = false;
    return result;
}

bool TKeyboard::buttonList()
{
    bool result;
    int adc = analogRead(A0);
    if (adc > 30 && adc < 70)
    {
        result = true;
    }
    else
        result = false;
    return result;
}

bool TKeyboard::buttonUP()
{
    bool result;
    int adc = analogRead(A0);
    if (adc > 1000)
    {
        result = true;
    }
    else
        result = false;
    return result;
}

bool TKeyboard::buttonDW()
{
    bool result;
    int adc = analogRead(A0);
    if (adc > 300 && adc < 350)
    {
        result = true;
    }
    else
        result = false;
    return result;
}