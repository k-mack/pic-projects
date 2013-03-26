/*
 * File:   sn74htc138.c
 * Author: Kevin Macksamie
 */

#include "sn74htc138.h"

void sn74htc138_decode(const sn74htc138_t *device, const uint8_t value) {
    uint8_t a = 0, b = 0, c = 0, enable = 0;
    if (device->zero_based)
    {
        a = 0x1 & value;
        b = (0x2 & value) >> 1;
        c = (0x4 & value) >> 2;
        enable = 1;
    }
    else if (value == 0)
    {
        enable = 0;
    }
    else
    {
        a = value == 0 ? 0 : 0x1 & value - 1;
        b = value == 0 ? 0 : (0x2 & value - 1) >> 1;
        c = value == 0 ? 0 : (0x4 & value - 1) >> 2;
        enable = 1;
    }
    
    *(device->port) =
            (a << device->a_bit) |
            (b << device->b_bit) |
            (c << device->c_bit) |
            (enable << device->enable_bit);
}

void sn74htc138_disable(const sn74htc138_t *device)
{
    *(device->port) &= ~(1 << device->enable_bit);
}