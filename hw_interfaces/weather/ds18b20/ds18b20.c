/*
 * File:   ds18b20.h
 * Author: Kevin Macksamie
 */
#include "ds18b20.h"
#ifndef NODEBUG
#include "ser.h"
#endif

unsigned char scratchpad[9];      // latest scratchpad read
unsigned char latest_ROM[8];      // latest collected ROM from search ROM command
unsigned char done;               // search ROM done flag
unsigned char last_discrepancy;   // search ROM last found discrepancy

void setup()
{
    last_discrepancy = 0;
    done = 0;
}

unsigned char next()
{
    unsigned char more_searches = 0;      // return variable - indicates if more searching needs to be done
    unsigned char rom_bit_index = 1;      // bit index in ROM array
    unsigned char rom_byte_index = 0;     // byte index in ROM array
    unsigned char discrepancy_marker = 0; // signals where a discrepancy was detected
    unsigned char read_bits = 0;          // true and false bits read from the devices
    unsigned char rom_bit = 0;            // ROM bit to use
    unsigned char mask = 1;               // bit mask for current ROM byte

    if (done)
    {
#ifndef NODEBUG
        ser_puts("next(): done flag set - exiting\n\r");
#endif
        done = 0;
        return 0;
    }

    if (!owire_reset_pulse())
    {
#ifndef NODEBUG
        ser_puts("next(): no presence pulse found\n\r");
#endif
        last_discrepancy = 0;
        return 0;
    }

#ifndef NODEBUG
    ser_puts("next(): sending ROM search command\n\r");
#endif
    owire_write_byte(DS18B20_ROM_SEARCH);  // send search ROM command

    // collect all 8 ROM bytes
    while (rom_byte_index < 8)
    {
        if (owire_read_bit() == 1) // read true value of ROM bit
            read_bits = 2;
        __delay_us(15);
        if (owire_read_bit() == 1) // read false value of ROM bit
            read_bits |= 1;
        if (read_bits == 3) {// no devices are on the 1-Wire bus
#ifndef NODEBUG
            ser_puts("next(): no devices are on bus\n\r");
#endif
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

        owire_write_bit(rom_bit); // send ROM bit to 1-Wire bus
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
        more_searches = 1;
    }

    return more_searches;
}

unsigned char first()
{
    last_discrepancy = 0;
    done = 0;
    return next();
}

void ds18b20_find_devices(temp_sensors_t *sensors)
{
    // XXX: before this function is called, disable interrupts while
    // communicating to device
    unsigned char num_roms;
    unsigned char lcv;
    setup();
    if (owire_reset_pulse())
    {
        if (first())
        {
            num_roms = 0;
            do
            {
                for (lcv = 0; lcv < 8; lcv++)
                {
                    sensors->ROMS[num_roms][lcv] = latest_ROM[lcv];
                }
                num_roms++;

#ifndef NODEBUG
                if (num_roms >= MAX_TEMP_SENSORS)
                    ser_puts("ds18b20_find_devices(): max temp sensors found\n\r");
#endif
                
            } while (num_roms < MAX_TEMP_SENSORS && next());  // find all devices
        }
        else
        {
#ifndef NODEBUG
            ser_puts("ds18b20_find_devices(): no first device found\n\r");
#endif
        }
    }
    else
    {
#ifndef NODEBUG
        ser_puts("ds18b20_find_devices(): no initial presence pulse\n\r");
#endif
    }
    // XXX: after this function is called, re-enable interrupts when
    // communication is done
}

void match_rom(unsigned char ROM[])
{
    unsigned char lcv;
    owire_write_byte(DS18B20_ROM_MATCH);
    for (lcv = 0; lcv < 8; lcv++)
    {
        owire_write_bit(ROM[lcv]);
    }
}

void ds18b20_convert_temp(unsigned char ROM[])
{
    // XXX: before this function is called, disable interrupts while
    // communicating to device
    unsigned char lcv;
    owire_reset_pulse();
    if (ROM)
        match_rom(ROM);
    else
        owire_write_byte(DS18B20_ROM_SKIP);
    owire_write_byte(DS18B20_CONVERT_TEMP);

    // wait for conversion to finish
    __delay_ms(750);

    owire_reset_pulse();
    if (ROM)
        match_rom(ROM);
    else
        owire_write_byte(DS18B20_ROM_SKIP);
    owire_write_byte(DS18B20_READ_SCRATCHPAD);
    for (lcv = 0; lcv < 9; lcv++)
    {
        scratchpad[lcv] = owire_read_byte();
    }

#ifndef NODEBUG
    for (lcv = 0; lcv < 9; lcv++)
    {
        if (scratchpad[lcv])
            ser_puts("scratchpad not empty\n\r");
        else
            ser_puts("scratchpad empty\n\r");

    }
#endif
    // XXX: after this function is called, re-enable interrupts when
    // communication is done
}

unsigned char ds18b20_temp_hi(void)
{
    return scratchpad[1];
}

unsigned char ds18b20_temp_lo(void)
{
    return scratchpad[0];
}
