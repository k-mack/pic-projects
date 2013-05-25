/* 
 * File:   main.c
 * Author: Kevin Macksamie
 */
#include <xc.h>
#include "ds18b20.h"
#include "init.h"
#include "lcd.h"
#include "ser.h"
#include "util.h"

// CONFIG
#pragma config FOSC = HS    // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT/T1OSO and RA7/OSC1/CLKIN/T1OSI)
#pragma config WDTE = OFF   // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF  // Power Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF  // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF     // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF    // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF  // Brown-out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF   // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF  // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config DEBUG = OFF  // In-Circuit Debugger Mode bit (In-Circuit Debugger disabled, RB6/ISCPCLK and RB7/ICSPDAT are general purpose I/O pins)

temp_sensors_t temp_sensors;
LCD_t lcd;
//sn74htc138_t decoder;
volatile unsigned char rx_data = 0xaa;
//unsigned char index = 0;
unsigned char tmp = 0;

interrupt void ISR(void)
{
    // RB0 interrupt (on falling edge) detected
    if (INTF)
    {
        INTF = 0;
        lcd_clear(&lcd);
        rx_data = 0;
        INTE = 1;
    }

    // Timer 1 overflowed to 0x0000
//    if (TMR1IF)
//    {
//        TMR1IF = 0;
//        if ((tmp = (rx_data >> index) & 0x1))
//        {
//            sn74htc138_decode(&decoder, index);
//        }
//        else
//        {
//            sn74htc138_disable(&decoder);
//        }
//
//        index = (index + 1) % 8;
//        TMR1H = 0xf8;
//        TMR1L = 0xf0;
//        TMR1IE = 1;
//    }

    ser_int();
}

/*
 *********************************************************************************************************
 * long_to_string_lz()
 *
 * Description : Convert a "long" to a null-terminated string, with leading zeros
 *               (base = decimal)
 * Arguments   : input = number to be converted
 *               str = pointer to string (i.e. display buffer)
 *               numdigits = number of digits to display
 * Returns     : none
 *********************************************************************************************************
 */
void long_to_string_lz(unsigned int input, char *str, char numdigits)
{
    char digit;
    for (digit = numdigits; digit > 0; digit--)
    {
        str[digit - 1] = (input % 10) + '0';
        input = input / 10;
    }
    str[numdigits] = 0; // null-terminate the string
}

void km_long_to_string_lz(unsigned int input, unsigned char *str, unsigned char len)
{
    signed char digit;
    unsigned char first = 1;
#ifndef NODEBUG
    ser_puts("converting to ascii:\n\r");
#endif
    for (digit = len - 2; digit >= 0; digit--)
    {
        str[digit] = (input % 10) + '0';
        input /= 10;
#ifndef NODEBUG
        ser_putch(str[digit]);
        ser_puts("\n\r");
#endif
        if (first)
        {
            str[--digit] = '.';
#ifndef NODEBUG
            ser_putch(str[digit]);
            ser_puts("\n\r");
#endif
        }

        first = 0;
    }
    str[len-1] = 0; // null-terminate the string
#ifndef NODEBUG
    ser_putch(str[len-1]);
    ser_puts("\n\r");
#endif
}

/*
 *********************************************************************************************************
 * long_to_string()
 *
 * Description : Convert a "long" to a null-terminated string
 *               (base = decimal)
 * Arguments   : input = number to be converted
 *               str = pointer to string (i.e. display buffer)
 *               numdigits = number of digits to display
 * Returns     : none
 *********************************************************************************************************
 */
void long_to_string(unsigned int input, char *str, char numdigits)
{
    char digit;
    int blank = 1;

    long_to_string_lz(input, str, numdigits);

    for (digit = 0; digit < numdigits - 1; digit++)
    {
        if (str[digit] == '0')
        {
            if (blank == 1)
                str[digit] = ' ';
        } 
        else
        {
            blank = 0;
        }
    }
}

void km_long_to_string(unsigned int input, unsigned char *str, unsigned char len)
{
    unsigned char digit;
    unsigned char blank = 1;

    km_long_to_string_lz(input, str, len);

#ifndef NODEBUG
    ser_puts("removing leading zeros:\n\r");
#endif
    for (digit = 0; digit < len - 1; digit++)
    {
        if (str[digit] == '0')
        {
            if (blank == 1)
                str[digit] = ' ';
        }
        else
        {
            blank = 0;
        }
#ifndef NODEBUG
        ser_putch(str[digit]);
        ser_puts("\n\r");
#endif
    }
}

/*
 * Entry point to the MCU application.
 */
int main(void) {
    lcd.data_bus = (unsigned char *) &PORTB;
    lcd.bus_offset = 4;
    lcd.en_pin = (unsigned char*)((((unsigned char) &PORTB) << 3) + 3);
    lcd.rs_pin = (unsigned char*)((((unsigned char) &PORTB) << 3) + 2);
    lcd.rw_pin = (unsigned char*)((((unsigned char) &PORTB) << 3) + 1);

    // Initialization procedure
    io_init();
    lcd_init(&lcd);
    ser_init();

    /*owire_t owire_hw;
    owire_hw.dq_pin = 4;
    owire_hw.port = (uint8_t *) &PORTC;
    owire_hw.tris = (uint8_t *) &TRISC;
    temp_sensors.bus = &owire_hw;*/
    
//    decoder.a_bit = 0;
//    decoder.b_bit = 1;
//    decoder.c_bit = 2;
//    decoder.enable_bit = 3;
//    decoder.zero_based = 1;
//    decoder.port = (unsigned char *) &PORTC;

    //timer_init();
#ifndef NODEBUG
    ser_puts("Detecting sensors...\n\r");
#endif
    ds18b20_find_devices(&temp_sensors);
#ifndef NODEBUG
    ser_puts("Detection complete\n\r");
#endif

#ifndef NODEBUG
    unsigned char i, j, k;
    for (i = 0; i < MAX_TEMP_SENSORS; i++)
    {
        ser_puts("Device: ");
        for (j = 0; j < 8; j++)
        {
            for (k = 0; k < 8; k++)
            {
                if ((temp_sensors.ROMS[i][j] >> k) & 0x01)
                    ser_puts("1");
                else
                    ser_puts("0");
            }
        }
        ser_puts("\n\r");
    }


    if (owire_read())
        ser_puts("read 1");
    else
        ser_puts("read 0");
    ser_puts("\n\r");
#endif

    lcd_puts(&lcd, "Welcome!\nStart typing @$%");
    ser_puts("Welcome to the LCD module serial interface!\n\r");

    rx_data = ser_getch();
    lcd_clear(&lcd);
//    lcd_putch(rx_data);
//    ser_putch(rx_data);

    unsigned char T_MSB;
    unsigned char T_LSB;
    unsigned char TempHi_C; // Raw high byte
    unsigned char TempLo_C; // Raw low byte
    unsigned char strbuf[8];
    unsigned int tmp16;
    while (1)
    {
//    	rx_data = ser_getch();  // Block on serial line
//    	ser_putch(rx_data);     // Echo input back to transmitter
//    	lcd_putch(rx_data);     // Write received data to LCD
//
        ds18b20_convert_temp(0);//temp_sensors.ROMS[curr_ROM]);
        TempHi_C = ds18b20_temp_hi();
        TempLo_C = ds18b20_temp_lo();

        // partition number from fraction
        T_MSB = ((TempHi_C << 4) & 0xF0) | ((TempLo_C >> 4) & 0x0F);
        T_LSB = TempLo_C & 0x0F;

        lcd_clear(&lcd);
        lcd_home(&lcd);
        if (TempHi_C & 0x80)
        {
            lcd_puts(&lcd, "-");
            T_MSB ^= 0xFF;
            T_LSB = ((T_LSB ^ 0xFF) + 1) & 0x0F;
        }
        else
        {
            lcd_puts(&lcd, "+");
        }
        long_to_string(T_MSB, strbuf, 3);   // integer is 3 sig figs
        lcd_puts(&lcd, strbuf);
        lcd_puts(&lcd, ".");
        tmp16 = ((unsigned int) T_LSB) * 625;
        long_to_string_lz(tmp16, strbuf, 4);  // fraction is 4 sig figs
        lcd_puts(&lcd, strbuf);
        lcd_putch(&lcd, CHAR_DEGREE);
        lcd_puts(&lcd, "C");

        lcd_goto(&lcd, LCD_LINE2);

        // ----- Conversion to Fahrenheit -----
        // 10F = 16C + (16C + 4) / 8 + 320
        // where: 16C = TempHi_C:TempLo_C

        unsigned char _TempLo_C;    // modified low byte
        unsigned char _TempHi_C;    // modified high byte

        _TempLo_C = TempLo_C;
        _TempHi_C = TempHi_C;

        // add 4 to round fraction part, but ignore overflow into the integer part
        _TempLo_C = ((_TempLo_C + 4) & 0x0F) | (_TempLo_C & 0xF0); // add 4

        // divide by 8: >> 3 both bytes, carry bottom 3 from high byte to the lower
        _TempLo_C >>= 3;         // shift low byte
        tmp = TempHi_C & 0x03;   // capture bottom 3 bits from high byte
        _TempLo_C |= (tmp << 5); // insert bottom 3 bits of high byte to top of lower
        _TempHi_C >>= 3;         // shift high byte
        if (TempHi_C & 0x80)     // is measured temperature negative?
            _TempHi_C |= 0xE0;   // sign extend

        // add measured temperture to divided temperature
        _TempLo_C += TempLo_C;   // add two low bytes
        if (STATUSbits.C)
            _TempHi_C++;         // mind overflow
        _TempHi_C += TempHi_C;   // add two high bytes

        // add 320
        _TempLo_C += 0x40;       // add low byte of 320
        if (STATUSbits.C)
            _TempHi_C++;         // mind overflow
        _TempHi_C += 0x01;       // add high byte of 320

        //
        // <_TempHi_C:_TempLo_C> stores the approx. F temp multipled by 10
        //

        unsigned int temperature = _TempHi_C;
        temperature = (temperature << 8) | _TempLo_C;

        km_long_to_string(temperature, strbuf, 8);

        if (TempHi_C & 0x80)
            lcd_puts(&lcd, "- ");
        else
            lcd_puts(&lcd, "+ ");

        // trim string to display
        for (tmp = 0; tmp < 8; tmp++)
        {
            if (strbuf[tmp] != ' ' && strbuf[tmp] != 0)
                lcd_putch(&lcd, strbuf[tmp]);
        }
        lcd_putch(&lcd, CHAR_DEGREE);
        lcd_puts(&lcd, "F");

        __delay_ms(100);
    }

    return 0;
}
