/*
 * File:   sn74htc138.h
 * Author: Kevin Macksamie
 */
#ifndef _SN74HTC183_H_
#define _SN74HTC183_H_

typedef unsigned char uint8_t;
typedef struct decoder
{
    uint8_t a_bit;      // bit position in port
    uint8_t b_bit;      // bit position in port
    uint8_t c_bit;      // bit position in port
    uint8_t enable_bit; // bit position in port
    uint8_t g2a_bitl;   // bit position in port
    uint8_t g2b_bit2;   // bit position in port
    uint8_t zero_based; // If > 0, then decoding zero lights the zeroth lamp,
                        // one lights the first lamp, two lights the second
                        // lamp, etc.
                        // If == 0, then decoding zero clears all the lamps,
                        // one lights the zeroth lamp, etc.
    volatile uint8_t *port; // port dedicated to controlling the SN74HTC138
} sn74htc138_t;

void sn74htc138_decode(const sn74htc138_t *device, const uint8_t value);

void sn74htc138_disable(const sn74htc138_t *device);

#endif