/*
 * File:   owire.c
 * Author: Kevin Macksamie
 *
 */
#include "owire.h"
#ifndef NODEBUG
#include "ser.h"
#endif

/*
 * Definitions 1-Wire hardware interface
 */
#define DQ_TRIS TRISCbits.TRISC4
#define DQ_PIN  PORTCbits.RC4

void owire_drive_low()
{
    DQ_TRIS = 0;    // make dq an output pin
    DQ_PIN = 0;     // drive pin low
}

void owire_drive_high()
{
    DQ_TRIS = 0;    // make dq an output pin
    DQ_PIN = 1;     // drive pin high
}

unsigned char owire_read()
{
    unsigned char sample;
    DQ_TRIS = 1;        // make dq an input pin
    sample = DQ_PIN;    // sample bus
    return sample;
}

void owire_write_byte(unsigned char write_byte)
{
    unsigned char lcv;
    for (lcv = 0; lcv < 8; lcv++)
    {
        owire_write_bit(write_byte & 0x01);
        write_byte >>= 1;
    }
}

unsigned char owire_read_byte()
{
    unsigned char lcv;
    unsigned char result = 0;
    for (lcv = 0; lcv < 8; lcv++)
    {
        result >>= 1;       // shift result to get it ready for the next bit
        if (owire_read_bit())
        {
            result |= 0x80; // if result is one, then set MS-bit
        }
    }
    return result;
}

unsigned char owire_reset_pulse()
{
    unsigned char presence;
    DQ_PIN = 1;                 // release bus
    owire_drive_low();
    __delay_us(480);
    DQ_PIN = 1;                 // release bus
    __delay_us(70);
    presence = owire_read();    // sample bus
    __delay_us(410);

#ifndef NODEBUG
    if (presence == 1)
        ser_puts("owire_reset_pulse(): no device present\n\r");
    else if (presence == 0)
        ser_puts("owire_reset_pulse(): device(s) present\n\r");
#endif
    
    return !presence;
}

void owire_write_bit(const unsigned char write_bit)
{
    owire_drive_low();
    if (write_bit == 1)
    {
        __delay_us(6);
        DQ_PIN = 1;     // release bus
        __delay_us(64);
    }
    else
    {
        __delay_us(60);
        DQ_PIN = 1;     // release bus
        __delay_us(10);
    }
}

unsigned char owire_read_bit()
{
    unsigned char read_bit;
    owire_drive_low();          // drive bus low
    __delay_us(6);
    DQ_PIN = 1;                 // release bus
    __delay_us(9);
    read_bit = owire_read();    // sample bus
    __delay_us(55);
    return read_bit;
}
