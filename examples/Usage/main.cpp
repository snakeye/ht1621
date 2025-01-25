#include <HT1621Display.h>

#define PIN_CS 4
#define PIN_WR 5
#define PIN_DATA 6

typedef HT1621::Driver<PIN_CS, PIN_WR, PIN_DATA> DisplayDriver;
typedef LCD::Display<DisplayDriver> Display;

Display htDisplay;

void setup()
{
    htDisplay.begin();
    htDisplay.configure();
}

void loop()
{
    htDisplay.clearBuffer();
    htDisplay.setDigit(0, 1);
    htDisplay.sync();
}
