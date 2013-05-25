/*
 * Author: Kevin Macksamie
 */

#include "lcd.h"
 
/*** LCD device with HD44780 driver ***/

/* 
 * Read/Write Enable Pulse (enable pin)
 * 0: Disabled
 * 1: Read/Write operation enabled
 * 
 * Register Select (register select pin)
 * 0: Instruction register during write 
 * operations. Busy Flash during read
 * operations.
 * 1: Data for read or write operations
* 
 * Read/Write Control (register write pin)
 * 0: WRITE, LCD accepts data
 * 1: READ, LCD presents data
 */

#define LCD_STROBE(x) ((x = 1),(x = 0))
#define LCD_STROBE_SLOW(x) { \
    x = 1;  \
    __delay_us(1); \
    x = 0;  \
}

static void lcd_cmd(LCD_t* lcd, unsigned char cmd);
static void lcd_tx_byte(LCD_t* lcd, unsigned char byte);
static void lcd_write(LCD_t* lcd, unsigned char byte);
static unsigned char read_pin(unsigned char pin);
static void write_pin(volatile unsigned char* pin, unsigned char data);
static void strobe_pin(volatile unsigned char* pin);
static void strobe_pin_slow(volatile unsigned char* pin);

/*****************************************************************************
 * Subroutine: lcd_clear
 *
 * Description:
 * This subroutine clears the LCD and the address counter is reset.
 *
 * Input Parameters:
 * LCD struct reference
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * lcd_cmd
 * __delay_ms
 *****************************************************************************/
void lcd_clear(LCD_t* lcd)
{
    lcd_cmd(lcd, 0x01);
    __delay_ms(2);
    lcd->addr = LINE1_START_ADDR;
}

/*****************************************************************************
 * Subroutine: lcd_cmd
 *
 * Description:
 * This private subroutine sends a command to the LCD device.
 *
 * Input Parameters:
 * LCD struct reference
 * LCD Command
 *
 * Output Parameters:
 * None
 * 
 *
 * Subroutines:
 * lcd_tx_byte
 *****************************************************************************/
static void lcd_cmd(LCD_t* lcd, unsigned char cmd)
{
    write_pin(lcd->rs_pin, 0);
    write_pin(lcd->rw_pin, 0);
    lcd_tx_byte(lcd, cmd);
}    

/*****************************************************************************
 * Subroutine: lcd_disable
 *
 * Description:
 * This subroutine disables the LCD.
 *
 * Input Parameters:
 * LCD struct reference
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * None
 *****************************************************************************/
void lcd_disable(LCD_t* lcd)
{
    write_pin(lcd->en_pin, 0);
    write_pin(lcd->rw_pin, 0);
}

/*****************************************************************************
 * Subroutine: lcd_disable
 *
 * Description:
 * This subroutine disables the LCD.
 *
 * Input Parameters:
 * LCD struct reference
 * LCD address
 *
 * Output Parameters:
 * lcd_en
 * lcd_rw
 *
 * Subroutines:
 * lcd_cmd
 *****************************************************************************/
void lcd_goto(LCD_t* lcd, unsigned char address)
{
    lcd_cmd(lcd, 0x80 | address);
    lcd->addr = address;
}

/*****************************************************************************
 * Subroutine: lcd_home
 *
 * Description:
 * This subroutine returns the LCD home and the address counter is reset.
 *
 * Input Parameters:
 * LCD struct reference
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * lcd_cmd
 * __delay_ms
 *****************************************************************************/
void lcd_home(LCD_t* lcd)
{
    lcd_cmd(lcd, 0x2);
    __delay_ms(2);
    lcd->addr = LINE1_START_ADDR;
}    

/*****************************************************************************
 * Subroutine: lcd_init
 *
 * Description:
 * This subroutine initializes the LCD device.
 *
 * Input Parameters:
 * LCD struct reference
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * lcd_clear
 * lcd_tx_byte
 * __delay_ms
 * __delay_us
 *****************************************************************************/
void lcd_init(LCD_t* lcd)
{
    write_pin(lcd->rw_pin, 0);
    write_pin(lcd->rs_pin, 0);
    
    /*** Power-On Initialization ***/
    /* Wait 15 ms */
    __delay_ms(15);
    
    /* Write 0x3, pulse enable, wait 4.1 ms or longer */
    *lcd->data_bus = 0x3 << lcd->bus_offset;
    strobe_pin(lcd->en_pin);
    __delay_ms(5);
    
    /* Write 0x3, pulse enable, wait 100 us or longer */
    *lcd->data_bus = 0x3 << lcd->bus_offset;
    strobe_pin(lcd->en_pin);
    __delay_us(100);
    
    /* Write 0x3, pulse enable, wait 40 us or longer */
    *lcd->data_bus = 0x3 << lcd->bus_offset;
    strobe_pin(lcd->en_pin);
    __delay_us(40);
    
    /* Write 0x2, pulse enable, wait 40 us or longer */
    *lcd->data_bus = 0x2 << lcd->bus_offset; /* Set 4-bit mode */
    strobe_pin(lcd->en_pin);
    __delay_us(40);
    
    /*** Display Configuration ***/
    lcd_tx_byte(lcd, 0x28); /* 4-bit operation */
    __delay_us(40);
    
    lcd_tx_byte(lcd, 0x06); /* Automatically increase address pointer */
    __delay_us(40);
    
    lcd_tx_byte(lcd, 0x0C); /* Turn display on */
    __delay_us(40);
    
    lcd_clear(lcd);
}

/*****************************************************************************
 * Subroutine: lcd_putch
 *
 * Description:
 * This subroutine writes a character to the LCD.
 *
 * Input Parameters:
 * LCD struct reference
 * Byte to write to LCD
 *
 * Output Parameters:
 * None 
 *
 * Subroutines:
 * lcd_write
 *****************************************************************************/
void lcd_putch(LCD_t* lcd, unsigned char data)
{
    write_pin(lcd->rs_pin, 1);
    lcd_write(lcd, data);  
}

/*****************************************************************************
 * Subroutine: lcd_puts
 *
 * Description:
 * This subroutine writes a string to the LCD.
 *
 * Input Parameters:
 * LCD struct reference
 * Data to write to LCD 
 *
 * Output Parameters:
 * None 
 *
 * Subroutines:
 * lcd_write
 *****************************************************************************/
void lcd_puts(LCD_t* lcd, const char *data)
{
    write_pin(lcd->rs_pin, 1);
    while (*data)
        lcd_write(lcd, *data++);  
}

/*****************************************************************************
 * Subroutine: lcd_tx_byte
 *
 * Description:
 * This private subroutine transmits a character to the LCD's DD RAM. This
 * subroutine bypasses the address counter in lcd_write(unsigned char).
 *
 * Input Parameters:
 * LCD struct reference
 * Byte to write to LCD 
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * __delay_us
 *****************************************************************************/
static void lcd_tx_byte(LCD_t* lcd, unsigned char byte)
{
    /* Transfer upper nibble first */
    *lcd->data_bus = ((byte >> 4) << lcd->bus_offset) | (*lcd->data_bus & 0x0F);
    strobe_pin_slow(lcd->en_pin);
    
    /* Transfer lower nibble */
    *lcd->data_bus = ((byte & 0xF) << lcd->bus_offset) | (*lcd->data_bus & 0x0F);
    strobe_pin_slow(lcd->en_pin);
    
    __delay_us(40);
}

/*****************************************************************************
 * Subroutine: lcd_write
 *
 * Description:
 * This private subroutine writes a character to the LCD.
 *
 * Input Parameters:
 * LCD struct reference
 * Byte to write to LCD
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * lcd_tx_byte
 *****************************************************************************/
static void lcd_write(LCD_t* lcd, unsigned char byte)
{
    unsigned char rs_tmp, rw_tmp, ret, nwl_addr;
    
    rs_tmp = read_pin(*lcd->rs_pin);
    rw_tmp = read_pin(*lcd->rw_pin);
    
    ret = (byte == NWL || byte == CR || byte == ETX);
    if (ret)                                    /* Check for newline */
    {
        nwl_addr = (lcd->addr <= LINE1_END_ADDR+1) ? LINE2_START_ADDR : 
            LINE1_START_ADDR;
        lcd_cmd(lcd, 0x80 | nwl_addr);
        lcd->addr = nwl_addr;
        write_pin(lcd->rs_pin, rs_tmp);
        write_pin(lcd->rw_pin, rw_tmp);
        return;
    }
    else if (byte == BACKSPACE || byte == DEL)  /* Check for backspace */
    {
        /* Decrement address */
        if (lcd->addr == LINE1_START_ADDR)
        {
            /* Do nothing */
        }
        else
        {
            lcd->addr = (lcd->addr == LINE2_START_ADDR) ? LINE1_END_ADDR : lcd->addr - 1;
        }
        
        lcd_cmd(lcd, 0x80 | lcd->addr); /* Go to new address */
        
        /* Replace previous char with space */
        write_pin(lcd->rs_pin, 1); /* Rather not call lcd_write again */
        lcd_tx_byte(lcd, SPACE);   /*                                 */
        
        lcd_cmd(lcd, 0x80 | lcd->addr); /* Go back to new address */
        
        write_pin(lcd->rs_pin, rs_tmp);
        write_pin(lcd->rw_pin, rw_tmp);
        return;
    }    
    else if (lcd->addr == LINE1_END_ADDR+1)          /* End of first line */
    {
        write_pin(lcd->rs_pin, 0);
        write_pin(lcd->rw_pin, 0);
        lcd_tx_byte(lcd, 0x80 | LINE2_START_ADDR);
        lcd->addr = LINE2_START_ADDR;
    }
    else if (lcd->addr == LINE2_END_ADDR+1)          /* End of second line */
    {
        lcd_clear(lcd);
        lcd->addr = LINE1_START_ADDR;
    }
    
    write_pin(lcd->rs_pin, rs_tmp);
    write_pin(lcd->rw_pin, rw_tmp);
    lcd_tx_byte(lcd, byte);
    ++lcd->addr;
}

static unsigned char read_pin(unsigned char pin)
{
    return pin & 1;
}

static void write_pin(volatile unsigned char* pin, unsigned char data)
{
    *pin |= data & 1;
}

static void strobe_pin(volatile unsigned char* pin)
{
    write_pin(pin, 1);
    write_pin(pin, 0);
}

static void strobe_pin_slow(volatile unsigned char* pin)
{
    write_pin(pin, 1);
    __delay_us(1);
    write_pin(pin, 0);
}

