#include <xc.h>
#include "lcd.h"
 
/*** LCD device with HD44780 driver ***/

static volatile unsigned char *lcd_db = (unsigned char *) &PORTB; /* Data */
static unsigned char lcd_doffset = 4; /* Data offset in the register */
static unsigned char addr = LINE1_START_ADDR; /* Address counter */

/* Read/Write Enable Pulse 
 * 0: Disabled
 * 1: Read/Write operation enabled
 */
static volatile bit lcd_en @ ((unsigned)&PORTB<<3)+BIT3;

/* Register Select
 * 0: Instruction register during write 
 * operations. Busy Flash during read
 * operations.
 * 1: Data for read or write operations
 */
static volatile bit lcd_rs @ ((unsigned)&PORTB<<3)+BIT2;

/* Read/Write Control
 * 0: WRITE, LCD accepts data
 * 1: READ, LCD presents data
 */
static volatile bit lcd_rw @ ((unsigned)&PORTB<<3)+BIT1;



#define LCD_STROBE ((lcd_en = 1),(lcd_en = 0))
#define LCD_STROBE_SLOW { \
    lcd_en = 1;  \
    __delay_us(1); \
    lcd_en = 0;  \
}

static void lcd_cmd(unsigned char);
static void lcd_tx_byte(unsigned char);
static void lcd_write(unsigned char);

/*****************************************************************************
 * Subroutine: lcd_clear
 *
 * Description:
 * This subroutine clears the LCD and the address counter is reset.
 *
 * Input Parameters:
 * None
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * lcd_tx_byte
 *****************************************************************************/
void lcd_clear(void)
{
    lcd_cmd(0x01);
    __delay_ms(2);
    addr = LINE1_START_ADDR;
}

/*****************************************************************************
 * Subroutine: lcd_cmd
 *
 * Description:
 * This private subroutine sends a command to the LCD device.
 *
 * Input Parameters:
 * None
 *
 * Output Parameters:
 * lcd_rs
 * lcd_rw
 *
 * Subroutines:
 * lcd_tx_byte
 *****************************************************************************/
static void lcd_cmd(unsigned char cmd)
{
    lcd_rs = 0;
    lcd_rw = 0;
    lcd_tx_byte(cmd);
}    

/*****************************************************************************
 * Subroutine: lcd_disable
 *
 * Description:
 * This subroutine disables the LCD.
 *
 * Input Parameters:
 * None
 *
 * Output Parameters:
 * lcd_en
 * lcd_rw
 *
 * Subroutines:
 * None
 *****************************************************************************/
void lcd_disable(void)
{
    lcd_en = 0;
    lcd_rw = 0;
}

/*****************************************************************************
 * Subroutine: lcd_disable
 *
 * Description:
 * This subroutine disables the LCD.
 *
 * Input Parameters:
 * None
 *
 * Output Parameters:
 * lcd_en
 * lcd_rw
 *
 * Subroutines:
 * None
 *****************************************************************************/
void lcd_goto(unsigned char address)
{
    lcd_cmd(0x80 | address);
    addr = address;
}

/*****************************************************************************
 * Subroutine: lcd_home
 *
 * Description:
 * This subroutine returns the LCD home and the address counter is reset.
 *
 * Input Parameters:
 * None
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * lcd_cmd
 *****************************************************************************/
void lcd_home(void)
{
    lcd_cmd(0x2);
    __delay_ms(2);
    addr = LINE1_START_ADDR;
}    

/*****************************************************************************
 * Subroutine: lcd_init
 *
 * Description:
 * This subroutine initializes the LCD device.
 *
 * Input Parameters:
 * None
 *
 * Output Parameters:
 * lcd_db
 * lcd_rs
 * lcd_rw
 *
 * Subroutines:
 * lcd_clear
 * lcd_tx_byte
 *****************************************************************************/
void lcd_init(void)
{
    lcd_rw = 0;
    lcd_rs = 0;
    
    /*** Power-On Initialization ***/
    /* Wait 15 ms */
    __delay_ms(15);
    
    /* Write 0x3, pulse enable, wait 4.1 ms or longer */
    *lcd_db = 0x3 << lcd_doffset;
    LCD_STROBE;
    __delay_ms(5);
    
    /* Write 0x3, pulse enable, wait 100 us or longer */
    *lcd_db = 0x3 << lcd_doffset;
    LCD_STROBE;
    __delay_us(100);
    
    /* Write 0x3, pulse enable, wait 40 us or longer */
    *lcd_db = 0x3 << lcd_doffset;
    LCD_STROBE;
    __delay_us(40);
    
    /* Write 0x2, pulse enable, wait 40 us or longer */
    *lcd_db = 0x2 << lcd_doffset; /* Set 4-bit mode */
    LCD_STROBE;
    __delay_us(40);
    
    /*** Display Configuration ***/
    lcd_tx_byte(0x28); /* 4-bit operation */
    __delay_us(40);
    
    lcd_tx_byte(0x06); /* Automatically increase address pointer */
    __delay_us(40);
    
    lcd_tx_byte(0x0C); /* Turn display on */
    __delay_us(40);
    
    lcd_clear();
}

/*****************************************************************************
 * Subroutine: lcd_putch
 *
 * Description:
 * This subroutine writes a character to the LCD.
 *
 * Input Parameters:
 * data
 *
 * Output Parameters:
 * lcd_rs
 *
 * Subroutines:
 * lcd_write
 *****************************************************************************/
void lcd_putch(unsigned char data)
{
    lcd_rs = 1;
    lcd_write(data);  
}

/*****************************************************************************
 * Subroutine: lcd_puts
 *
 * Description:
 * This subroutine writes a string to the LCD.
 *
 * Input Parameters:
 * data
 *
 * Output Parameters:
 * lcd_rs
 *
 * Subroutines:
 * lcd_write
 *****************************************************************************/
void lcd_puts(const char *data)
{
    lcd_rs = 1;
    while (*data)
        lcd_write(*data++);  
}

/*****************************************************************************
 * Subroutine: lcd_tx_byte
 *
 * Description:
 * This private subroutine transmits a character to the LCD's DD RAM. This
 * subroutine bypasses the address counter in lcd_write(unsigned char).
 *
 * Input Parameters:
 * byte
 *
 * Output Parameters:
 * lcd_db
 *
 * Subroutines:
 * None
 *****************************************************************************/
static void lcd_tx_byte(unsigned char byte)
{
    /* Transfer upper nibble first */
    *lcd_db = ((byte >> 4) << lcd_doffset) | (*lcd_db & 0x0F);
    LCD_STROBE_SLOW;
    
    /* Transfer lower nibble */
    *lcd_db = ((byte & 0xF) << lcd_doffset) | (*lcd_db & 0x0F);
    LCD_STROBE_SLOW;
    
    __delay_us(40);
}

/*****************************************************************************
 * Subroutine: lcd_write
 *
 * Description:
 * This private subroutine writes a character to the LCD.
 *
 * Input Parameters:
 * byte
 *
 * Output Parameters:
 * None
 *
 * Subroutines:
 * lcd_tx_byte
 *****************************************************************************/
static void lcd_write(unsigned char byte)
{
    unsigned char rs_tmp, rw_tmp, ret, nwl_addr;
    
    rs_tmp = lcd_rs;
    rw_tmp = lcd_rw;
    
    ret = (byte == NWL || byte == CR || byte == ETX);
    if (ret)                                    /* Check for newline */
    {
        nwl_addr = (addr <= LINE1_END_ADDR+1) ? LINE2_START_ADDR : 
            LINE1_START_ADDR;
        lcd_cmd(0x80 | nwl_addr);
        addr = nwl_addr;
        lcd_rs = rs_tmp;
        lcd_rw = rw_tmp;
        return;
    }
    else if (byte == BACKSPACE || byte == DEL)  /* Check for backspace */
    {
        /* Decrement address */
        if (addr == LINE1_START_ADDR)
        {
            /* Do nothing */
        }
        else
        {
            addr = (addr == LINE2_START_ADDR) ? LINE1_END_ADDR : addr - 1;
        }
        
        lcd_cmd(0x80 | addr); /* Go to new address */
        
        /* Replace previous char with space */
        lcd_rs = 1;         /* Rather not call lcd_write again */
        lcd_tx_byte(SPACE); /*                                 */
        
        lcd_cmd(0x80 | addr); /* Go back to new address */
        
        lcd_rs = rs_tmp;
        lcd_rw = rw_tmp;
        return;
    }    
    else if (addr == LINE1_END_ADDR+1)          /* End of first line */
    {
        lcd_rs = 0;
        lcd_rw = 0;
        lcd_tx_byte(0x80 | LINE2_START_ADDR);
        addr = LINE2_START_ADDR;
    }
    else if (addr == LINE2_END_ADDR+1)          /* End of second line */
    {
        lcd_clear();
        addr = LINE1_START_ADDR;
    }
    
    lcd_rs = rs_tmp;
    lcd_rw = rw_tmp;
    lcd_tx_byte(byte);
    ++addr;
}
