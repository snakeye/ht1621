#include <HT1621Display.h>

#define PIN_CS 4
#define PIN_WR 5
#define PIN_DATA 6

/**
 * Define hardware configuration to support different type of LCD displays
 */
class CustomHardwareConfig
{
public:
    static constexpr unsigned int commons = 4;
    static constexpr unsigned int digits = 6;
    static constexpr LCD::Bias bias = LCD::Bias::BIAS_1_3;

    static constexpr uint8_t SEG_A = 0b00001000;
    static constexpr uint8_t SEG_B = 0b10000000;
    static constexpr uint8_t SEG_C = 0b00100000;
    static constexpr uint8_t SEG_D = 0b00010000;
    static constexpr uint8_t SEG_E = 0b00000010;
    static constexpr uint8_t SEG_F = 0b00000100;
    static constexpr uint8_t SEG_G = 0b01000000;
    static constexpr uint8_t SEG_P = 0b00000001;
};

typedef HT1621::Driver<PIN_CS, PIN_WR, PIN_DATA> DisplayDriver;
typedef LCD::Display<DisplayDriver, CustomHardwareConfig> Display;

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
