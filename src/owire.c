/*
 * File:   owire.c
 * Author: Kevin Macksamie
 *
 */
#include "ser.h"
#include "owire.h"

void owire_drive_low(owire_t *device)
{
    *(device->tris) &= ~(1 << device->dq_pin); // make dq an output pin
    *(device->port) &= ~(1 << device->dq_pin); // drive pin low
}

void owire_drive_high(owire_t *device)
{
    *(device->tris) &= ~(1 << device->dq_pin); // make dq an output pin
    *(device->port) |= 1 << device->dq_pin;    // drive pin high
}

uint8_t owire_read(owire_t *device)
{
    uint8_t sample;
    *(device->tris) |= 1 << device->dq_pin; // make dq an input pin
    sample = *(device->port);               // sample bus
    sample &= 1 << device->dq_pin;
    sample >>= device->dq_pin;
    return sample;
}

void owire_write_byte(owire_t *device, uint8_t write_byte)
{
    uint8_t lcv;
    for (lcv = 0; lcv < 8; lcv++)
    {
        owire_write_bit(device, write_byte & 0x01);
        write_byte >>= 1;
    }
}

uint8_t owire_read_byte(owire_t *device)
{
    uint8_t lcv;
    uint8_t result = 0;
    for (lcv = 0; lcv < 8; lcv++)
    {
        result >>= 1;       // shift result to get it ready for the next bit
        if (owire_read_bit(device))
        {
            result |= 0x80; // if result is one, then set MS-bit
        }
    }
    return result;
}

uint8_t owire_reset_pulse(owire_t *device)
{
    uint8_t presence;
    *(device->tris) |= 1 << device->dq_pin; // release bus
    owire_drive_low(device);
    __delay_us(480);
    *(device->tris) |= 1 << device->dq_pin; // release bus
    __delay_us(70);
    presence = owire_read(device); // sample bus
    __delay_us(410);
    
    if (presence == 1)
    {
        ser_puts("owire_reset_pulse(): no device present\n\r");
    }
    else if (presence == 0)
    {
        ser_puts("owire_reset_pulse(): device(s) present\n\r");
    }
    
    return !presence;
}

void owire_write_bit(owire_t *device, const uint8_t write_bit)
{
    owire_drive_low(device);
    if (write_bit == 1)
    {
        __delay_us(6);
        *(device->tris) |= 1 << device->dq_pin; // release bus
        __delay_us(64);
    }
    else
    {
        __delay_us(60);
        *(device->tris) |= 1 << device->dq_pin; // release bus
        __delay_us(10);
    }
}

uint8_t owire_read_bit(owire_t *device)
{
    uint8_t read_bit;
    owire_drive_low(device);                // drive bus low
    __delay_us(6);
    *(device->tris) |= 1 << device->dq_pin; // release bus
    __delay_us(9);
    read_bit = owire_read(device);          // sample bus
    __delay_us(55);
    return read_bit;
}
