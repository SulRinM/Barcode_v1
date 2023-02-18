#include "barcode.h"
#include "SoftwareSerial.h"

#define DEBUG_SERIAL Serial
#define SCAN_LEFT Serial1
#define SCAN_RIGHT Serial2

extern int NUMB_TUBE;
const int nTUBE = (NUMB_TUBE >> 1);

const int txPin = 13;
const int rxPin = 12;
SoftwareSerial rackSerial(txPin, rxPin);

extern long debugMotorPosition;

Barcode::Barcode()
{
}

void Barcode::init()
{
    DEBUG_SERIAL.begin(9600);
    SCAN_LEFT.begin(9600);
    SCAN_RIGHT.begin(9600);
    Serial3.begin(9600);

    while (!Serial)
        ;
    pinMode(txPin, OUTPUT);
    pinMode(rxPin, INPUT);
    rackSerial.begin(9600);

    SCAN_LEFT.setTimeout(10);
    SCAN_RIGHT.setTimeout(10);
    clear();
}

void Barcode::clear()
{
    counter_left = 0;
    counter_right = 0;
    for (int i = 0; i <= nTUBE; ++i)
    {
        arr[0][i] = 0;
        arr[1][i] = 0;
    }
}

void Barcode::debugData()
{
    static bool flag1 = false;
    static bool flag2 = false;
    DEBUG_SERIAL.println(debugMotorPosition);

    if (SCAN_LEFT.available() > 0)
    {
        arr[0][counter_left++] = SCAN_LEFT.parseInt();
        if(counter_left >= nTUBE) flag1 = true;
        counter_left = counter_left % nTUBE;
    }
    if (SCAN_RIGHT.available() > 0)
    {
        arr[1][counter_right++] = SCAN_RIGHT.parseInt();
        if(counter_right >= nTUBE) flag2 = true;
        counter_right = counter_right % nTUBE;
    }
    if (flag1 && flag2)
    {
        delay(100); // отладочный - потом удалить!
        for (int a = 0; a < nTUBE; a++)
        {
            DEBUG_SERIAL.println(arr[0][a]);
            DEBUG_SERIAL.println('\t');
            DEBUG_SERIAL.println(arr[1][a]);
            DEBUG_SERIAL.println('\n');
        }
        //clear();
       flag1 = flag2 = false;
    }
}