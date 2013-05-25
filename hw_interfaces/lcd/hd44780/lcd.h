/*
 * Author: Kevin Macksamie
 *
 * LCD interface header file.
 * See lcd.c for more info.
 */

#include <xc.h>

#ifndef LCD_H
#define LCD_H

#define CR        0x0D          /* Carriage return */
#define BACKSPACE 0x08          /* Backspace */
#define DEL       0x7F          /* Delete */
#define ETX       0x03          /* End of transmission */
#define NWL       '\n'          /* New line */
#define SPACE     0x20          /* Space */

#define CHAR_PER_LINE         16 /* Characters per line on LCD device */
#define NUM_LINES              2 /* Number of lines on LCD device */

#define LINE1_START_ADDR    0x00 /* Starting address of line 1 */
#define LINE2_START_ADDR    0x40 /* Starting address of line 2 */
#define LINE1_END_ADDR      0x0F /* End address of line 1 */
#define LINE2_END_ADDR      0x4F /* End address of line 2 */

#define LCD_LINE1           0x00
#define LCD_LINE2           0x40

#define CHAR_DEGREE         0xDF /* Degree symbol */

/*
 * Represents an LCD device
 */
typedef struct LCD
{
    volatile unsigned char* data_bus;   // data bus
    unsigned char bus_offset;           // offset in bus to data lines
    volatile unsigned char* en_pin;     // enable pin
    volatile unsigned char* rs_pin;     // register select pin
    volatile unsigned char* rw_pin;     // register write pin
    unsigned char addr;                 // address counter
} LCD_t;

/* Clear and home the LCD */
void lcd_clear(LCD_t* lcd);

/* Disable LCD display */
void lcd_disable(LCD_t* lcd);

/* Move cursor to address */
void lcd_goto(LCD_t* lcd, unsigned char pos);

/* Return cursor home */
void lcd_home(LCD_t* lcd);

/* Initilization LCD device */
void lcd_init(LCD_t* lcd);

/* Write a string to the LCD */
void lcd_putch(LCD_t* lcd, unsigned char byte);

/* Write a string to the LCD */
void lcd_puts(LCD_t* lcd, const char* str);

#endif
