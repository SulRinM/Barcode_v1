#include "portal.h"
#include "RStepper.h"

RStepper Tstepper(1, 11, 9);

const int pinSensorHome = 30;                 // датчик положения "дома"
const int pinSensorStart = 29;                // датчик положения "старта"
const int pinleftBarcode = 33;                // пин включения левого сканера
const int pinrightBarcode = 34;               // пин включения правого сканера
const int TIME_SCAN_ON_START_POSITION = 5000; // 500ms

extern long BLIND_SPOT_HOME; // слепая зона от "дома" до первой пробирки
extern long BLIND_SPOT;      // слепая зона между пробирками
extern long SCANNING_AREA;   // зона сканирования
extern long SPEED_SCAN;      // скорость движения портала при сканировании
extern long TRAY_CORRECTION; // корреция порта относительно датчика "дом"

long debugMotorPosition = 0;

enum TrayState
{
    homePositon = 1,
    startPosition = 2,
    nonePositon = 3
};
enum zone
{
    BLIND_SPOT_CASEHOME = 0, // слепая зона от позиции дома до первой пробирки
    BLIND_SPOT_CASETUBE,     // слепая зона между пробирками
    SCAN_REGION,             // зона сканирования
    GO_HOME                  // сканирование завершено - поехали домой
};

Portal::Portal()
{
}

void Portal::init()
{
    pinMode(pinleftBarcode, OUTPUT);
    pinMode(pinrightBarcode, OUTPUT);
    pinMode(pinSensorHome, INPUT);
    pinMode(pinSensorStart, INPUT);

    Tstepper.setMaxSpeed(4000);     // SPEED = Steps / second
    Tstepper.setAcceleration(1000); // ACCELERATION = Steps /(second)^2
    Tstepper.enableOutputs();       // enable pins
                                    //  Tstepper.moveTo(150000);
                                    //  Tstepper.run();
}

void Portal::run_HomePosition(bool state)
{
    // после запуска начинаем коррекцию
    static bool Correction = false;
    static bool StartScan = false;
    static long LastPosition = 0;
    if (!Correction && homePositon != statePortal())
    {
        Tstepper.setSpeed(-1000);
        Tstepper.run();
    }
    // коррекция завершена
    else if (!Correction && homePositon == statePortal())
    {
        Tstepper.stop();
        LastPosition = Tstepper.currentPosition();
        Correction = true;
    }
    // запуск портала
    if (Correction && (state || StartScan))
    {
        StartScan = true;
        // отработал датчик старта
        if (startPosition == statePortal())
        {
            cnt = GO_HOME;
        }
        switch (cnt)
        {
        case BLIND_SPOT_CASEHOME:
            Tstepper.moveTo(LastPosition + BLIND_SPOT_HOME); // едем до первой пробирки от дома
            Tstepper.run();
            if (Tstepper.currentPosition() == (LastPosition + BLIND_SPOT_HOME))
            {
                cnt = SCAN_REGION;
                Capture = Tstepper.currentPosition();
            }
            break;

        case SCAN_REGION:
            sum = Capture + SCANNING_AREA;
            Tstepper.moveTo(sum);
            Tstepper.run();
            if (Tstepper.currentPosition() == sum)
            {
                digitalWrite(pinleftBarcode, HIGH);
                digitalWrite(pinrightBarcode, HIGH);
                cnt = BLIND_SPOT_CASETUBE;
                Capture = Tstepper.currentPosition();
            }
            break;

        case BLIND_SPOT_CASETUBE:
            sum = Capture + BLIND_SPOT;
            Tstepper.moveTo(sum);
            Tstepper.run();
            if (Tstepper.currentPosition() == sum)
            {
                digitalWrite(pinleftBarcode, LOW);
                digitalWrite(pinrightBarcode, LOW);
                cnt = SCAN_REGION;
                Capture = Tstepper.currentPosition();
            }
            break;

        case GO_HOME:
            static unsigned long CounterOnState = 0;
            Tstepper.moveTo(LastPosition);
            Tstepper.run();
            if (CounterOnState < TIME_SCAN_ON_START_POSITION)
            {
                digitalWrite(pinleftBarcode, HIGH);
                digitalWrite(pinrightBarcode, HIGH);
            }
            else
            {
                digitalWrite(pinleftBarcode, LOW);
                digitalWrite(pinrightBarcode, LOW);
            }
            CounterOnState++;
            if (Tstepper.currentPosition() == LastPosition)
            {
                StartScan = false;
                CounterOnState = 0;
                cnt = BLIND_SPOT_CASEHOME;
            }
            break;
        }
    }
}

int Portal::statePortal()
{
    int result = 0;
    if (digitalRead(pinSensorHome))
    {
        result = homePositon;
    }
    else if (digitalRead(pinSensorStart))
    {
        result = startPosition;
    }
    return result;
}