# STM32F4 library code directory
STM_COMMON=../../STM32F4-Discovery_FW_V1.1.0

# Source files
SRCS= main.c stm32f4xx_it.c system_stm32f4xx.c 

# Usb Library
SRCS += usb_bsp.c usbd_cdc_vcp.c usbd_desc.c usbd_usr.c stm32f4_discovery.c

# Library code
SRCS += stm32f4xx_exti.c stm32f4xx_gpio.c stm32f4xx_rcc.c misc.c stm32f4xx_syscfg.c
SRCS += usb_core.c usb_dcd.c usb_dcd_int.c 
SRCS += usbd_cdc_core.c usbd_core.c usbd_ioreq.c usbd_req.c

# Project name
PROJ_NAME=$(notdir $(CURDIR))
OUTPATH=build

# add startup file to build
SRCS += $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s

# Compiler Flags and Settings
#######################################################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

MCU=cortex-m4

DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DMANGUSTA_DISCOVERY -DUSE_USB_OTG_FS -DHSE_VALUE=8000000

MCFLAGS = -mcpu=$(MCU) -mthumb -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork -fsingle-precision-constant

OPTIMIZE = -ggdb -O0

CFLAGS  = $(MCFLAGS) $(OPTIMIZE) $(DEFS) -Wl,-T,stm32_flash.ld

# Include files and Search Paths
#######################################################################################

# Search paths
vpath %.c \
src \
usb_vcp_lib/src \
$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src \
$(STM_COMMON)/Utilities/STM32F4-Discovery \
$(STM_COMMON)/Libraries/STM32_USB_OTG_Driver/src \
$(STM_COMMON)/Libraries/STM32_USB_Device_Library/Core/src \
$(STM_COMMON)/Libraries/STM32_USB_Device_Library/Class/cdc/src

# Include files from STM libraries
CFLAGS += -I.
CFLAGS += -Iinc
CFLAGS += -Iusb_vcp_lib/inc
CFLAGS += -I$(STM_COMMON)/Utilities/STM32F4-Discovery
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include 
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/inc
CFLAGS += -I$(STM_COMMON)/Libraries/STM32_USB_OTG_Driver/inc
CFLAGS += -I$(STM_COMMON)/Libraries/STM32_USB_Device_Library/Core/inc
CFLAGS += -I$(STM_COMMON)/Libraries/STM32_USB_Device_Library/Class/cdc/inc

OBJS = $(SRCS:.c=.o)

# Make and Flash
#######################################################################################

all: proj
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -lm -lc -lnosys -o $@ $(LIBPATHS) $(LIBS)
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	find . -name \*.o -type f -delete
	find . -name \*.lst -type f -delete
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin

# Flash the STM32F4
burn: proj
	st-flash write $(OUTPATH)/$(PROJ_NAME).bin 0x8000000
