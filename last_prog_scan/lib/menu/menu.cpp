#include "menu.h"
#include "button.h"
#include "EEPROM.h"
#include "RLcd_TIW.h"

#define _LCD_TYPE 1
#include "LCD_1602_RUS_ALL.h"
LCD_1602_RUS lcd(0x27, 16, 2);

TKeyboard keybord;


extern long BLIND_SPOT_HOME; // слепая зона от "дома" до первой пробирки
extern long BLIND_SPOT;      // слепая зона между пробирками
extern long SCANNING_AREA;     // зона сканирования
extern long SPEED_SCAN;    // скорость движения портала при сканировании

extern bool startKey;
extern int key;


void init_menu()
{
    lcd.init();      //  Инициируем работу с LCD дисплеем
    lcd.backlight(); //  Включаем подсветку LCD дисплея
    lcd.clear();
}

void call_menu()
{
    char text[16];
    static int cntKey = 0;
    startKey = keybord.buttonStart();
    bool s = keybord.buttonList();
    if (s)
    {
        cntKey++;
        delay(300);
        lcd.clear();
        if (cntKey > 3)
            cntKey = 0;
    }
    //Serial.println(cntKey);
    switch (cntKey)
    {
    case 0:
        lcd.setCursor(0, 0);
        lcd.print("CЛEП.ЗOHA ДОМА 1");
        lcd.setCursor(0, 1);
        sprintf(text, "%d", BLIND_SPOT_HOME);
        lcd.print(text);
        if (keybord.buttonUP())
        {
            BLIND_SPOT_HOME++;
            key = 2571;
            EEPROM.put(0, BLIND_SPOT_HOME);
            EEPROM.put(20, key);
            delay(100);
        }
        else if (keybord.buttonDW())
        {
            if (BLIND_SPOT_HOME)
                BLIND_SPOT_HOME--;
            EEPROM.put(0, BLIND_SPOT_HOME);
            delay(100);
            key = 2571;
        }
        break;
    case 1:
        lcd.setCursor(0, 0);
        lcd.print("CЛEП.ЗOHA ДОМА 2");
        lcd.setCursor(0, 1);
        sprintf(text, "%d", BLIND_SPOT);
        lcd.print(text);
        if (keybord.buttonUP())
        {
            BLIND_SPOT++;
            EEPROM.put(4, BLIND_SPOT);
            delay(100);
            key = 2571;
        }
        else if (keybord.buttonDW())
        {
            if (BLIND_SPOT)
                BLIND_SPOT--;
            EEPROM.put(4, BLIND_SPOT);
            delay(100);
            key = 2571;
        }
        break;
    case 2:
        lcd.setCursor(0, 0);
        lcd.print("ОКНО СКАНИРОВ.");
        lcd.setCursor(0, 1);
        sprintf(text, "%d", SCANNING_AREA);
        lcd.print(text);
        if (keybord.buttonUP())
        {
            SCANNING_AREA++;
            EEPROM.put(8, SCANNING_AREA);
            delay(100);
            key = 2571;
        }
        else if (keybord.buttonDW())
        {
            if (SCANNING_AREA)
                SCANNING_AREA--;
            EEPROM.put(8, SCANNING_AREA);
            delay(100);
            key = 2571;
        }
        break;
    case 3:
        lcd.setCursor(0, 0);
        lcd.print("CKOP. CKAHИРОB.");
        lcd.setCursor(0, 1);
        sprintf(text, "%d", SPEED_SCAN);
        lcd.print(text);
        if (keybord.buttonUP())
        {
            SPEED_SCAN++;
            EEPROM.put(12, SPEED_SCAN);
            delay(100);
            key = 2571;
        }
        else if (keybord.buttonDW())
        {
            if (SPEED_SCAN)
                SPEED_SCAN--;
            EEPROM.put(12, SPEED_SCAN);
            delay(100);
            key = 2571;
        }
        break;
    }
}