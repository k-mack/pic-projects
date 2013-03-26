/*
 * File:   onewire.c
 * Author: Kevin Macksamie
 *
 */

#include "onewire.h"

uint8_t owire_reset(one_wire_t *device)
{
    uint8_t presence = 0;
    *(device->port) &= ~(1 << device->dq_pin);
    __delay_us(480);
    *(device->port) |= (1 << device->dq_pin);
    presence = *(device->port) & (1 << device->dq_pin);
    __delay_us(410);
    return !presence;
}