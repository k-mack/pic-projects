/*
 * File:   sn74htc138.h
 * Author: Kevin Macksamie
 */
#ifndef SN74HTC183_H_
#define SN74HTC183_H_

typedef struct sn74htc138
{
    unsigned char a_bit;          // bit position in port
    unsigned char b_bit;          // bit position in port
    unsigned char c_bit;          // bit position in port
    unsigned char enable_bit;     // bit position in port
    unsigned char g2a_bitl;       // bit position in port
    unsigned char g2b_bit2;       // bit position in port
    unsigned char zero_based;     // If > 0, then decoding zero lights the zeroth lamp,
                                  // one lights the first lamp, two lights the second
                                  // lamp, etc.
                                  // If == 0, then decoding zero clears all the lamps,
                                  // one lights the zeroth lamp, etc.
    volatile unsigned char *port; // port dedicated to controlling the SN74HTC138
} sn74htc138_t;

void sn74htc138_decode(const sn74htc138_t *device, const unsigned char value);

void sn74htc138_disable(const sn74htc138_t *device);

#endif