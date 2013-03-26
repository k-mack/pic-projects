/* 
 * File:   onewire.h
 * Author: Kevin Macksamie
 *
 */

#ifndef _OWIRE_H
#define	_OWIRE_H

typedef unsigned char uint8_t;
typedef struct owire
{
    uint8_t dq_pin;         // bit position of DQ pin in port
    uint8_t parasitic;      // If == 1, then device is operating in parasitic
                            // mode.
                            // If == 0, then device is power externally.
    volatile uint8_t *port; // port dedicated to controlling the DS18B20 device
} owire_t;

void owire_drive_low(owire_t *device);
void owire_drive_high(owire_t *device);
void owire_read(owire_t *device);
void owire_write_byte(owire_t *device, uint8_t write_byte);
uint8_t owire_reset_pulse(owire_t *device);
void owire_write_bit(owire_t *device, uint8_t write_bit);
uint8_t owire_read_bit(owire_t *device);

#endif	/* _OWIRE_H */

