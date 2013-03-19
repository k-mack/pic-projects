/*
 * LCD interface header file.
 * See lcd.c for more info.
 */

#ifndef _LCD_H
#define _LCD_H

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

/* Clear and home the LCD */
void lcd_clear(void);

/* Disable LCD display */
void lcd_disable(void);

/* Return cursor home */
void lcd_home(void);

/* Initilization LCD device */
void lcd_init(void);

/* Write a string to the LCD */
void lcd_putch(unsigned char);

/* Write a string to the LCD */
void lcd_puts(const char*);

#endif