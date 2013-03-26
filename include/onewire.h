/* 
 * File:   onewire.h
 * Author: Kevin Macksamie
 *
 */

#ifndef _ONEWIRE_H
#define	_ONEWIRE_H

typedef unsigned char uint8_t;
typedef struct one_wire
{
    uint8_t dq_pin;         // bit position of DQ pin in port
    uint8_t parasitic;      // If == 1, then device is operating in parasitic
                            // mode.
                            // If == 0, then device is power externally.
    volatile uint8_t *port; // port dedicated to controlling the DS18B20 device
} one_wire_t;

void owire_drive_low(one_wire_t *device);
void owire_drive_high(one_wire_t *device);
void owire_read(one_wire_t *device);
void owire_write_byte(one_wire_t *device, uint8_t write_byte);
uint8_t owire_reset_pulse(one_wire_t *device);
void owire_write_bit(one_wire_t *device, uint8_t write_bit);
uint8_t owire_read_bit(one_wire_t *device);

#endif	/* _ONEWIRE_H */

