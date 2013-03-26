/* 
 * File:   main.c
 * Author: Kevin Macksamie
 */
#include <xc.h>
#include "init.h"
#include "lcd.h"
#include "ser.h"
#include "sn74htc138.h"
#include "util.h"

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT/T1OSO and RA7/OSC1/CLKIN/T1OSI)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config DEBUG = OFF      // In-Circuit Debugger Mode bit (In-Circuit Debugger disabled, RB6/ISCPCLK and RB7/ICSPDAT are general purpose I/O pins)

sn74htc138_t decoder;
volatile uint8_t rx_data = 0xaa;
uint8_t index = 0;
uint8_t tmp = 0;

interrupt void ISR(void)
{
    // RB0 interrupt (on falling edge) detected
    if (INTF)
    {
        INTF = 0;
        lcd_clear();
        rx_data = 0;
        INTE = 1;
    }

    // Timer 1 overflowed to 0x0000
    if (TMR1IF)
    {
        TMR1IF = 0;
        if ((tmp = (rx_data >> index) & 0x1))
        {
            sn74htc138_decode(&decoder, index);
        }
        else
        {
            sn74htc138_disable(&decoder);
        }

        index = (index + 1) % 8;
        TMR1H = 0xf8;
        TMR1L = 0xf0;
        TMR1IE = 1;
    }

    ser_int();
}

/*
 * Entry point to the MCU application.
 */
int main(void) {
    // Initialization procedure
    io_init();
    lcd_init();
    ser_init();
    
    decoder.a_bit = 0;
    decoder.b_bit = 1;
    decoder.c_bit = 2;
    decoder.enable_bit = 3;
    decoder.zero_based = 1;
    decoder.port = (uint8_t *) &PORTC;

    timer_init();

    lcd_puts("Welcome!\nStart typing @$%");
    ser_puts("Welcome to the LCD module serial interface!\n\r");

    rx_data = ser_getch();
    lcd_clear();
    lcd_putch(rx_data);
    ser_putch(rx_data);

	while (1)
	{
    	rx_data = ser_getch();  // Block on serial line
    	ser_putch(rx_data);     // Echo input back to transmitter
    	lcd_putch(rx_data);     // Write received data to LCD
	}

    return 0;
}