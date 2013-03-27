/*
 * File:   ds18b20.h
 * Author: Kevin Macksamie
 */

#include "ds18b20.h"

void ds18b20_convert_temp(ds18b20_t *sensor)
{
    // TODO: disable interrupts while communicating to device
    uint8_t lcv;
    if (owire_reset_pulse(sensor->device))
    {
        owire_write_byte(sensor->device, DS18B20_ROM_SKIP);
        owire_write_byte(sensor->device, DS18B20_CONVERT_T);

        // wait for conversion to finish
        while (!owire_read(sensor->device));

        if (owire_reset_pulse(sensor->device))
        {
            owire_write_byte(sensor->device, DS18B20_ROM_SKIP);
            owire_write_byte(sensor->device, DS18B20_READ_SCRATCHPAD);
            for (lcv = 0; lcv < 9; lcv++)
            {
                sensor->scratchpad[lcv] = owire_read_byte(sensor->device);
            }
        }
    }
    else
    {
        sensor->status |= DS18B20_STATUS_NO_PRESENCE;
    }
    // TODO: re-enable interrupts when communication is done
}
