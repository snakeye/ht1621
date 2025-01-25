#include <HT1621Display.h>

#define PIN_CS 4
#define PIN_WR 5
#define PIN_DATA 6

/**
 * Extending the default character mapper to support hexadecimal symbols
 */
class HexadecimalMapper : public LCD::CharacterMapper<LCD::DefaultHardwareConfig>
{
public:
    class Config : public LCD::DefaultHardwareConfig
    {
    };

public:
    static uint8_t mapCharacter(const char ch)
    {
        switch (ch)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return mapDigit(ch - '0');
        case 'A':
        case 'a':
            return Config::SEG_A | Config::SEG_B | Config::SEG_C | Config::SEG_E | Config::SEG_F | Config::SEG_G;
        case 'B':
        case 'b':
            return Config::SEG_C | Config::SEG_D | Config::SEG_E | Config::SEG_F | Config::SEG_G;
        case 'C':
        case 'c':
            return Config::SEG_A | Config::SEG_D | Config::SEG_E | Config::SEG_F;
        case 'D':
        case 'd':
            return Config::SEG_B | Config::SEG_C | Config::SEG_D | Config::SEG_E | Config::SEG_G;
        case 'E':
        case 'e':
            return Config::SEG_A | Config::SEG_D | Config::SEG_E | Config::SEG_F | Config::SEG_G;
        case 'F':
        case 'f':
            return Config::SEG_A | Config::SEG_E | Config::SEG_F | Config::SEG_G;
        default:
            return 0x00;
        }
    }
};

typedef HT1621::Driver<PIN_CS, PIN_WR, PIN_DATA> DisplayDriver;
typedef LCD::Display<DisplayDriver, LCD::DefaultHardwareConfig, HexadecimalMapper> Display;

Display htDisplay;

void setup()
{
    htDisplay.begin();
    htDisplay.configure();
}

void loop()
{
    htDisplay.clearBuffer();
    htDisplay.setChar(0, 'A');
    htDisplay.sync();
}
