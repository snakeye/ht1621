#pragma once

#include <Arduino.h>

namespace HT1621
{
    constexpr int WRITE_BIT_DELAY = 4;

    enum Command
    {
        SYS_DIS = 0b00000000,   // Turn off both system oscillator and LCD bias generator
        SYS_EN = 0b00000001,    // Turn on system oscillator
        LCD_OFF = 0b00000010,   // Turn off LCD bias generator
        LCD_ON = 0b00000011,    // Turn on LCD bias generator
        TIMER_DIS = 0b00000100, // Disable time base output
        WDT_DIS = 0b00000101,   // Disable WDT time-out flag output
        TIMER_EN = 0b00000110,  // Enable time base output
        WDT_EN = 0b00000111,    // Enable WDT time-out flag output
        TONE_OFF = 0b00001000,  // Turn off tone outputs
        TONE_ON = 0b00001001,   // Turn on tone outputs
        CLR_TIMER = 0b00001100, // Clear the contents of time base generator
        CLR_WDT = 0b00001110,   // Clear the contents of WDT stage
        XTAL_32K = 0b00010100,  // System clock source, crystal oscillator
        RC_256K = 0b00011000,   // System clock source, on-chip RC oscillator
        EXT_256K = 0b00011100,  // System clock source, external clock source
        BIAS_12 = 0b00100000,   // LCD 1/2 bias option
        BIAS_13 = 0b00100001,   // LCD 1/3 bias option
        TONE_4K = 0b01000000,   // Tone frequency, 4kHz
        TONE_2K = 0b01100000,   // Tone frequency, 2kHz
        IRQ_DIS = 0b10000000,   // Disable IRQ output
        IRQ_EN = 0b10001000,    // Enable IRQ output
        F1 = 0b10100000,        // Time base/WDT clock output:1Hz The WDT time-out flag after: 4s
        F2 = 0b10100001,        // Time base/WDT clock output:2Hz The WDT time-out flag after: 2s
        F4 = 0b10100010,        // Time base/WDT clock output:4Hz The WDT time-out flag after: 1s
        F8 = 0b10100011,        // Time base/WDT clock output:8Hz The WDT time-out flag after: 1/2s
        F16 = 0b10100100,       // Time base/WDT clock output:16Hz The WDT time-out flag after: 1/4s
        F32 = 0b10100101,       // Time base/WDT clock output:32Hz The WDT time-out flag after: 1/8s
        F64 = 0b10100110,       // Time base/WDT clock output:64Hz The WDT time-out flag after: 1/16s
        F128 = 0b10100111,      // Time base/WDT clock output:128Hz The WDT time-out flag after: 1/32s
        TEST = 0b11100000,      // Test mode, user don′t use
        NORMAL = 0b11100011,    // Normal mode
    };

    uint8_t buildBiasCommand(uint8_t bias, uint8_t commons)
    {
        uint8_t cmd = 0;

        if (bias == 2)
            cmd = HT1621::Command::BIAS_12;
        else if (bias == 3)
            cmd = HT1621::Command::BIAS_13;

        cmd |= ((commons - 2) & 0b00000011) << 2;

        return cmd;
    }

    class DriverInterface
    {
    public:
        virtual void begin() = 0;

        virtual void sendCommand(uint8_t command) = 0;

        virtual void sendData(uint8_t address, uint8_t data) = 0;
        virtual void sendData(uint8_t address, uint8_t *data, unsigned int count) = 0;
    };

    template <uint8_t CS_PIN, uint8_t WR_PIN, uint8_t DATA_PIN>
    class Driver : public DriverInterface
    {
    public:
        // Initialize the HT1621
        void begin()
        {
            pinMode(CS_PIN, OUTPUT);
            pinMode(WR_PIN, OUTPUT);
            pinMode(DATA_PIN, OUTPUT);

            digitalWrite(CS_PIN, HIGH);
            digitalWrite(WR_PIN, HIGH);
            digitalWrite(DATA_PIN, LOW);
        }

        void sendCommand(uint8_t command)
        {
            digitalWrite(CS_PIN, LOW);
            writeBits<3>(0b100); // Command mode identifier
            writeBits<8>(command);
            writeBits<1>(0b0);
            digitalWrite(CS_PIN, HIGH);
        }

        void sendData(uint8_t address, uint8_t data)
        {
            digitalWrite(CS_PIN, LOW);
            writeBits<3>(0b101);   // Write data mode identifier
            writeBits<6>(address); // Address
            writeBits<4>(data);    // Data (4 bits)
            digitalWrite(CS_PIN, HIGH);
        }

        void sendData(uint8_t address, uint8_t *data, unsigned int count)
        {
            if (count == 0)
                return;

            digitalWrite(CS_PIN, LOW);
            writeBits<3>(0b101);   // Write data mode identifier
            writeBits<6>(address); // Address
            for (unsigned int i = 0; i < count; i++)
            {
                writeBits<4>(data[i]); // Data (4 bits)
            }
            digitalWrite(CS_PIN, HIGH);
        }

    private:
        void writeBit(bool bit)
        {
            digitalWrite(WR_PIN, LOW);
            delayMicroseconds(WRITE_BIT_DELAY); // Small delay for stability

            digitalWrite(DATA_PIN, bit ? HIGH : LOW);
            digitalWrite(WR_PIN, HIGH);
            delayMicroseconds(WRITE_BIT_DELAY); // Small delay for stability
        }

        template <uint8_t BITS>
        void writeBits(uint8_t data)
        {
            if constexpr (BITS > 1)
            {
                writeBit(data & (1 << (BITS - 1)));
                writeBits<BITS - 1>(data);
            }
            else
            {
                writeBit(data & 0x01);
            }
        }
    };
}
