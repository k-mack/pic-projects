/* 
 * File:   owire.h
 * Author: Kevin Macksamie
 *
 */

#ifndef OWIRE_H
#define	OWIRE_H

#include <xc.h>
#include "common.h"
#include "util.h"

typedef struct owire
{
    uint8_t dq_pin;         // bit position of DQ pin in port and tris
    uint8_t parasitic;      // If == 1, then device is operating in parasitic
                            // mode.
                            // If == 0, then device is power externally.
    volatile uint8_t *port; // port dedicated to controlling the DS18B20 device
    volatile uint8_t *tris; // tri-state register for the respective port
} owire_t;

void owire_drive_low(owire_t *device);
void owire_drive_high(owire_t *device);
uint8_t owire_read(owire_t *device);
void owire_write_byte(owire_t *device, const uint8_t write_byte);
uint8_t owire_read_byte(owire_t *device);
uint8_t owire_reset_pulse(owire_t *device);
void owire_write_bit(owire_t *device, const uint8_t write_bit);
uint8_t owire_read_bit(owire_t *device);

#endif	/* OWIRE_H */

