/* 
 * File:   ds18b20.h
 * Author: Kevin Macksamie
 */

#ifndef DS18B20_H
#define	DS18B20_H

#include "owire.h"

// DS18B20 ROM Command Set
#define DS18B20_ROM_SEARCH          0xF0 // Learn the ROM codes of all devices on bus
#define DS18B20_ROM_READ            0x33 // Can only be used when there is one slave on the bus. It allows the bus master to read the slave's 64-bit ROM code without using the Search ROM procedure
#define DS18B20_ROM_MATCH           0x55 // Followed by a 64-bit ROM code sequence allows the bus master to address a specific slave device or single-drop bus
#define DS18B20_ROM_SKIP            0xCC // Use this command to address all devices on the bus without sending out any ROM code information
#define DS18B20_ALARM_SEARCH        0xEC // Identical to Search ROM command except that only slaves with a set alarm will respond

// DS18B20 Function Command Set
#define DS18B20_CONVERT_TEMP        0x44 // Initiates temperature conversion
#define DS18B20_READ_SCRATCHPAD     0xBE // Reads entire scratchpad including the CRC byte
#define DS18B20_WRITE_SCRATCHPAD    0x4E // Writes data into scratchpad bytes 2, 3, and 4 (T_H, T_L, and configuration registers)
#define DS18B20_COPY_SCRATCHPAD     0x48 // Copies T_H, T_L, configuration register data from the scratchpad to EEPROM
#define DS18B20_RECALL_E2           0xB8 // Recalls T_H, T_L, and configuration register data from EEPROM to the scratchpad
#define DS18B20_READ_POWERSUPPLY    0x84 // Signals DS18B20 power supply mode to the master

#define MAX_TEMP_SENSORS 1

typedef struct temp_sensors
{
    owire_t *bus;                       // the 1-Wire bus used for the sensors
    uint8_t ROMS[MAX_TEMP_SENSORS][8];  // 1-Wire sensors' ROMS
} temp_sensors_t;

void ds18b20_find_devices(temp_sensors_t *sensors);
void ds18b20_convert_temp(temp_sensors_t *sensors, uint8_t ROM[]);
uint8_t ds18b20_temp_hi();
uint8_t ds18b20_temp_lo();

#endif	/* DS18B20_H */

