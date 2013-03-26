/*
 * File:   onewire.c
 * Author: Kevin Macksamie
 *
 */

#include "owire.h"

void owire_drive_low(owire_t *device)
{
}

void owire_drive_high(owire_t *device)
{
}

void owire_read(owire_t *device)
{
}

void owire_write_byte(owire_t *device, uint8_t write_byte)
{
}

uint8_t owire_reset_pulse(owire_t *device)
{
    uint8_t presence = 0;
    *(device->port) &= ~(1 << device->dq_pin);
    __delay_us(480);
    *(device->port) |= (1 << device->dq_pin);
    presence = *(device->port) & (1 << device->dq_pin);
    __delay_us(410);
    return !presence;
}

void owire_write_bit(owire_t *device, uint8_t write_bit)
{
}

uint8_t owire_read_bit(owire_t *device)
{
    return 0;
}