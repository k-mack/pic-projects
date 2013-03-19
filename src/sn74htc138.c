/*
 * File:   sn74htc138.c
 * Author: Kevin Macksamie
 */

#include "sn74htc138.h"

void SN74HTC138_decode(const SN74HTC138_t *sn74htc138, const uint8_t value) {
    uint8_t a = 0, b = 0, c = 0, enable = 0;
    if (sn74htc138->zero_based)
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
    
    *(sn74htc138->port) =
            (a << sn74htc138->a_bit) |
            (b << sn74htc138->b_bit) |
            (c << sn74htc138->c_bit) |
            (enable << sn74htc138->enable_bit);
}

void SN74HTC138_disable(const SN74HTC138_t *sn74htc138)
{
    *(sn74htc138->port) &= ~(1 << sn74htc138->enable_bit);
}