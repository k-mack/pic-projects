/*
 * File:   init.c
 * Author: Kevin Macksamie
 */
#include <xc.h>
#include "init.h"

/*****************************************************************************
 * Subroutine: io_init
 *
 * Description:
 * This subroutine sets up the input/output ports on the PIC.
 *
 * Modified Registers:
 * INTCON
 * LCDCON
 * OPTION_REG
 * TRISB
 * TRISC
 *
 * Input Parameters:
 * None
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * None
 *****************************************************************************/
void io_init(void)
{
    LCDCON = 0;     // Disable LCD control register
    TRISB = 0x01;   // PORTB is used for LCD control and external interrupt
    TRISC = 0xf0;   // PORTC 0:6 are outputs
    PORTC = 0;      // Clear PORTC
    INTEDG = 0;     // Detect falling edge on RB0
    INTE = 1;       // Enable RB0 interrupt
    GIE = 1;        // Enable global interrupts
}

/*****************************************************************************
 * Subroutine: timer_init
 *
 * Description:
 * This subroutine sets up the clock on the PIC.
 *
 * Modified Registers:
 * INTCON
 * PIE
 * T1CON
 *
 * Input Parameters:
 * None
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * None
 *****************************************************************************/
void timer_init(void)
{
    // Clear timer 1 register pair and interrupt flag befre enabling interrupts
    TMR1H = 0;      // Clear high byte of timer 1
    TMR1L = 0;      // Clear low byte of timer 1
    TMR1IF = 0;     // Clear timer 1 interrupt flag

    // Now enable timer interrupts
    TMR1IE = 1;     // Timer 1 interrupt enabled
    PEIE = 1;       // Enable peripheral interrupts
    GIE = 1;        // Enable global interrupts
    
    // Setup and enable timer
    TMR1H = 0xd8;
    TMR1L = 0xf0;
    TMR1ON = 1;     // Turn on timer 1
}
