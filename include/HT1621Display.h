#pragma once

#include <Arduino.h>

#include "driver/HT1621.h"

namespace LCD
{
    enum Bias
    {
        BIAS_1_2 = 2,
        BIAS_1_3 = 3,
    };

    class DefaultHardwareConfig
    {
    public:
        static constexpr unsigned int commons = 4;
        static constexpr unsigned int digits = 6;
        static constexpr LCD::Bias bias = LCD::Bias::BIAS_1_3;

        static constexpr uint8_t SEG_A = 0b00000001;
        static constexpr uint8_t SEG_B = 0b00000010;
        static constexpr uint8_t SEG_C = 0b00000100;
        static constexpr uint8_t SEG_D = 0b10000000;
        static constexpr uint8_t SEG_E = 0b01000000;
        static constexpr uint8_t SEG_F = 0b00010000;
        static constexpr uint8_t SEG_G = 0b00100000;
        static constexpr uint8_t SEG_P = 0b00001000;
    };

    template <class Config>
    class CharacterMapper
    {
    public:
        /**
         * Fast mapping for digits representation
         */
        static uint8_t mapDigit(const unsigned int digit)
        {
            static const uint8_t digitChars[10] = {
                Config::SEG_A | Config::SEG_B | Config::SEG_C | Config::SEG_D | Config::SEG_E | Config::SEG_F,                 // 0
                Config::SEG_B | Config::SEG_C,                                                                                 // 1
                Config::SEG_A | Config::SEG_B | Config::SEG_D | Config::SEG_E | Config::SEG_G,                                 // 2
                Config::SEG_A | Config::SEG_B | Config::SEG_C | Config::SEG_D | Config::SEG_G,                                 // 3
                Config::SEG_B | Config::SEG_C | Config::SEG_F | Config::SEG_G,                                                 // 4
                Config::SEG_A | Config::SEG_C | Config::SEG_D | Config::SEG_F | Config::SEG_G,                                 // 5
                Config::SEG_A | Config::SEG_C | Config::SEG_D | Config::SEG_E | Config::SEG_F | Config::SEG_G,                 // 6
                Config::SEG_A | Config::SEG_B | Config::SEG_C,                                                                 // 7
                Config::SEG_A | Config::SEG_B | Config::SEG_C | Config::SEG_D | Config::SEG_E | Config::SEG_F | Config::SEG_G, // 8
                Config::SEG_A | Config::SEG_B | Config::SEG_C | Config::SEG_D | Config::SEG_F | Config::SEG_G,                 // 9
            };

            if (digit > 9)
                return 0x00;

            return digitChars[digit];
        }

        /**
         * Slow mapping for arbitrary supported characters
         */
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
            default:
                return 0x00;
            }
        }
    };

    template <class Driver, class C = DefaultHardwareConfig, class M = CharacterMapper<C>>
    class Display
    {
    public:
        class Config : public C
        {
        };

        class Mapper : public M
        {
        };

        static constexpr unsigned int digits = Config::digits;

    public:
        Display() : driver(Driver())
        {
            static_assert(Config::commons >= 2 && Config::commons <= 4, "Supported only 2, 3, or 4 commons");
            static_assert(Config::digits <= 16, "Maximum supported number of digits is 16");

            memset(currentState, 0, sizeof(currentState));
            memset(previousState, 0, sizeof(previousState));
        }

        void begin()
        {
            driver.begin();
        }

        void configure()
        {
            // disable the system
            driver.sendCommand(HT1621::Command::SYS_DIS);

            // disable everything unused
            driver.sendCommand(HT1621::Command::WDT_DIS);
            driver.sendCommand(HT1621::Command::TIMER_DIS);
            driver.sendCommand(HT1621::Command::TONE_OFF);
            driver.sendCommand(HT1621::Command::CLR_TIMER);
            driver.sendCommand(HT1621::Command::CLR_WDT);
            driver.sendCommand(HT1621::Command::IRQ_DIS);

            // configure required values
            driver.sendCommand(HT1621::buildBiasCommand(Config::bias, Config::commons));
            driver.sendCommand(HT1621::Command::RC_256K);
            driver.sendCommand(HT1621::Command::F128);
            driver.sendCommand(HT1621::Command::NORMAL);

            // clear the memory
            for (unsigned int i = 0; i < 32; i++)
            {
                driver.sendData(i, 0x00);
            }

            // enable the system
            driver.sendCommand(HT1621::Command::SYS_EN);
            driver.sendCommand(HT1621::Command::LCD_ON);
        }

        void clearBuffer()
        {
            for (unsigned int i = 0; i < Config::digits; i++)
            {
                currentState[i] = 0x00;
            }
        }

        uint8_t getBuffer(const unsigned int index)
        {
            return currentState[index];
        }

        void setBuffer(const unsigned int index, const uint8_t data)
        {
            currentState[index] = data;
        }

        void setDigit(const unsigned int index, const unsigned int digit, const bool dot = false)
        {
            uint8_t segments = Mapper::mapDigit(digit);
            if (dot)
            {
                segments |= Config::SEG_P; // Set the decimal point
            }

            setBuffer(index, segments);
        }

        void setChar(const unsigned int index, const char ch)
        {
            setBuffer(index, Mapper::mapCharacter(ch));
        }

        void sync(const bool force = false)
        {
            for (unsigned int i = 0; i < Config::digits; i++)
            {
                if (force || currentState[i] != previousState[i])
                {
                    driver.sendData(i * 2, currentState[i] & 0x0f);
                    driver.sendData(i * 2 + 1, (currentState[i] >> 4) & 0x0f);

                    previousState[i] = currentState[i];
                }
            }
        }

    private:
        Driver driver; // HT1621 driver

        uint8_t currentState[Config::digits]{};
        uint8_t previousState[Config::digits]{};
    };
}