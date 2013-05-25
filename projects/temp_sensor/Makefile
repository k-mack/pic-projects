# XC8 Makefile

PROJECT:=temp_sensor
MCU:=16F913
F_CPU:=20000000
TOOLDIR:="/opt/microchip/xc8/v1.12/bin"

#===================================

#===================================

.SUFFIXES:
.SUFFIXES: .p1 .c

COMPILE.c = $(CC) $(CFLAGS) $(OPTS) --pass1
COMPILE.p1 = $(CC) $(CFLAGS) $(OPTS)
CFLAGS = -D_XTAL_FREQ=$(F_CPU) --chip=$(MCU)
CFLAGS += $(LCD_FLAGS) $(TEMP_FLAGS) $(SER_FLAGS) -Iinclude
LCD_FLAGS = -I$(LCD_SRC)
TEMP_FLAGS = -I$(TSENSOR_SRC) -I$(1WIRE_SRC)
SER_FLAGS = -I$(USART_SRC)

CC = $(TOOLDIR)/xc8
OPTS = --double=24 --float=24 -N31 --warn=0 --opt=default,+asm,-asmfile,+speed,+space,-debug --addrqual=require --summary=default,-psect,-class,+mem,-hex,-file

PROJECT_SRC = src
LCD_SRC = ../../hw_interfaces/lcd/hd44780
TSENSOR_SRC = ../../hw_interfaces/sensors/ds18b20
1WIRE_SRC = ../../hw_interfaces/protocol/1wire
USART_SRC = ../../hw_interfaces/protocol/usart

SRCS = $(shell ls $(PROJECT_SRC)/*.c 2>/dev/null)
SRCS += $(shell ls $(LCD_SRC)/*.c 2>/dev/null)
SRCS += $(shell ls $(TSENSOR_SRC)/*.c 2>/dev/null)
SRCS += $(shell ls $(1WIRE_SRC)/*.c 2>/dev/null)
SRCS += $(shell ls $(USART_SRC)/*.c 2>/dev/null)

OBJS = $(SRCS:.c=.p1)

all: compile hex

compile: $(OBJS)

hex:
	$(COMPILE.p1) -m$(PROJECT).map -o$(PROJECT).cof $(shell ls *.p1 2>/dev/null)

clean: 
	rm *.p1 *.d *.lst *.pre *.hex *.hxl *.cof *.as *.obj *.sdb *.sym *.map *.rlf funclist

%.p1: %.c
	$(COMPILE.c) $<
