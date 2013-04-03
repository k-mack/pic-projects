/* 
 * File:   owire.h
 * Author: Kevin Macksamie
 *
 */

#ifndef OWIRE_H
#define	OWIRE_H

#include <xc.h>
#include "util.h"

/*typedef struct owire
{
    uint8_t dq_pin;         // bit position of DQ pin in port and tris
    volatile uint8_t *port; // port dedicated to controlling the DS18B20 device
    volatile uint8_t *tris; // tri-state register for the respective port
} owire_t;*/

void owire_drive_low();

void owire_drive_high();

unsigned char owire_read();

void owire_write_byte(unsigned char write_byte);

unsigned char owire_read_byte();

unsigned char owire_reset_pulse();

void owire_write_bit(const unsigned char write_bit);

unsigned char owire_read_bit();

#endif	/* OWIRE_H */

