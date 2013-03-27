/*
 * File:   owire.c
 * Author: Kevin Macksamie
 *
 */

#include "owire.h"

void owire_drive_low(owire_t *device)
{
    *(device->tris) &= ~(1 << device->dq_pin); // make dq an output pin
    *(device->port) &= ~(1 << device->dq_pin);
}

void owire_drive_high(owire_t *device)
{
    *(device->tris) &= ~(1 << device->dq_pin); // make dq an output pin
    *(device->port) |= 1 << device->dq_pin;
}

uint8_t owire_read(owire_t *device)
{
    *(device->tris) |= 1 << device->dq_pin; // make dq an input pin
    return (*(device->port) & (1 << device->dq_pin)) >> device->dq_pin;
}

void owire_write_byte(owire_t *device, const uint8_t write_byte)
{
    uint8_t lcv;
    uint8_t write_data = write_byte;
    for (lcv = 0; lcv < 8; lcv++)
    {
        owire_write_bit(device, write_byte & 0x01);
        write_data >>= 1;
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
    uint8_t presence = 0;
    *(device->tris) &= ~(1 << device->dq_pin); // make dq an output pin
    *(device->port) &= ~(1 << device->dq_pin);
    __delay_us(480);
    *(device->port) |= 1 << device->dq_pin;
    presence = *(device->port) & (1 << device->dq_pin);
    __delay_us(410);
    return !presence;
}

void owire_write_bit(owire_t *device, const uint8_t write_bit)
{
    if (write_bit)
    {
        owire_drive_low(device);
        __delay_us(60);
        owire_drive_high(device);
        __delay_us(10);
    }
    else
    {
        owire_drive_low(device);
        __delay_us(6);
        owire_drive_high(device);
        __delay_us(64);
    }
}

uint8_t owire_read_bit(owire_t *device)
{
    owire_drive_low(device);
    __delay_us(6);
    owire_drive_high(device);
    __delay_us(9);
    uint8_t read_bit = owire_read(device);
    __delay_us(55);
    return read_bit;
}
