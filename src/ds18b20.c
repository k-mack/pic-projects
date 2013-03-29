/*
 * File:   ds18b20.h
 * Author: Kevin Macksamie
 */

#include "ser.h"
#include "ds18b20.h"

uint8_t scratchpad[9];      // latest scratchpad read
uint8_t latest_ROM[8];      // latest collected ROM from search ROM command
uint8_t done;               // search ROM done flag
uint8_t last_discrepancy;   // search ROM last found discrepancy

void setup()
{
    last_discrepancy = 0;
    done = 0;
}

uint8_t next(owire_t *device)
{
    uint8_t more_searches = FALSE;  // return variable - indicates if more searching needs to be done
    uint8_t rom_bit_index = 1;      // bit index in ROM array
    uint8_t rom_byte_index = 0;     // byte index in ROM array
    uint8_t discrepancy_marker = 0; // signals where a discrepancy was detected
    uint8_t read_bits = 0;          // true and false bits read from the devices
    uint8_t rom_bit = 0;            // ROM bit to use
    uint8_t mask = 1;               // bit mask for current ROM byte

    if (done)
    {
        ser_puts("next(): done flag set - exiting\n\r");
        done = 0;
        return FALSE;
    }

    if (!owire_reset_pulse(device))
    {
        ser_puts("next(): no presence pulse found\n\r");
        last_discrepancy = 0;
        return FALSE;
    }

    ser_puts("next(): sending ROM search command\n\r");
    owire_write_byte(device, DS18B20_ROM_SEARCH);  // send search ROM command

    // collect all 8 ROM bytes
    while (rom_byte_index < 8)
    {
        if (owire_read_bit(device) == 1) // read true value of ROM bit
            read_bits = 2;
        __delay_us(6);
        if (owire_read_bit(device) == 1) // read false value of ROM bit
            read_bits |= 1;
        if (read_bits == 3) {// no devices are on the 1-Wire bus
            ser_puts("next(): no devices are on bus\n\r");
            break;
        }

        if (read_bits > 0)
            rom_bit = read_bits >> 1; // capture the true bit (will be 0 or 1)
        else
        {
            // if this discrepancy is before the last discrepancy on a previous
            // next then something went wrong; repeat and pick the same as last
            // time
            if (rom_bit_index < last_discrepancy)
                rom_bit = (latest_ROM[rom_byte_index] & mask) > 0;
            else
                rom_bit = rom_bit_index == last_discrepancy;

            // a new discrepancy has been reached, mark its position
            if (rom_bit == 0)
                discrepancy_marker = rom_bit_index;
        }

        // update ROM bits with new output bit
        if (rom_bit == 1)
            latest_ROM[rom_byte_index] |= mask;
        else
            latest_ROM[rom_byte_index] &= ~mask;

        owire_write_bit(device, rom_bit); // send ROM bit to 1-Wire bus
        rom_bit_index++;  // move to next rom bit position
        mask = mask << 1; // shift bit mask to new rom bit position

        // if the mask rolls over back to 0, then go to next ROM byte
        if (mask == 0)
        {
            // TODO: should do a CRC check here on ROM[rom_byte_index]
            rom_byte_index++;
            mask = 1;
        }
    }

    if (rom_bit_index < 65)
    {
        // search was unsuccessful if not all 64 bits were collected
        last_discrepancy = 0;
    }
    else
    {
        // search was successful
        last_discrepancy = discrepancy_marker;
        done = last_discrepancy == 0;
        more_searches = TRUE;
    }

    return more_searches;
}

uint8_t first(owire_t *device)
{
    last_discrepancy = 0;
    done = 0;
    return next(device);
}

void ds18b20_find_devices(temp_sensors_t *sensors)
{
    // XXX: before this function is called, disable interrupts while
    // communicating to device
    uint8_t num_roms;
    uint8_t lcv;
    setup();
    if (owire_reset_pulse(sensors->bus))
    {
        if (first(sensors->bus))
        {
            num_roms = 0;
            do
            {
                for (lcv = 0; lcv < 8; lcv++)
                {
                    sensors->ROMS[num_roms][lcv] = latest_ROM[lcv];
                }
                num_roms++;

                if (num_roms >= MAX_TEMP_SENSORS)
                    ser_puts("ds18b20_find_devices(): max temp sensors found\n\r");
                
            } while (num_roms < MAX_TEMP_SENSORS && next(sensors->bus));  // find all devices
        }
        else
        {
            ser_puts("ds18b20_find_devices(): no first device found\n\r");
        }
    }
    else
    {
        ser_puts("ds18b20_find_devices(): no initial presence pulse\n\r");
    }
    // XXX: after this function is called, re-enable interrupts when
    // communication is done
}

void match_rom(owire_t *bus, uint8_t ROM[])
{
    uint8_t lcv;
    owire_write_byte(bus, DS18B20_ROM_MATCH);
    for (lcv = 0; lcv < 8; lcv++)
    {
        owire_write_bit(bus, ROM[lcv]);
    }
}

void ds18b20_convert_temp(temp_sensors_t *sensors, uint8_t ROM[])
{
    // XXX: before this function is called, disable interrupts while
    // communicating to device
    uint8_t lcv;
    owire_reset_pulse(sensors->bus);
    if (ROM)
        match_rom(sensors->bus, ROM);
    else
        owire_write_byte(sensors->bus, DS18B20_ROM_SKIP);
    owire_write_byte(sensors->bus, DS18B20_CONVERT_TEMP);

    // wait for conversion to finish
    while (!owire_read(sensors->bus));

    owire_reset_pulse(sensors->bus);
    if (ROM)
        match_rom(sensors->bus, ROM);
    else
        owire_write_byte(sensors->bus, DS18B20_ROM_SKIP);
    owire_write_byte(sensors->bus, DS18B20_READ_SCRATCHPAD);
    for (lcv = 0; lcv < 9; lcv++)
    {
        scratchpad[lcv] = owire_read_byte(sensors->bus);
    }
    // XXX: after this function is called, re-enable interrupts when
    // communication is done
}

uint8_t ds18b20_temp_hi(void)
{
    return scratchpad[1];
}

uint8_t ds18b20_temp_lo(void)
{
    return scratchpad[0];
}
