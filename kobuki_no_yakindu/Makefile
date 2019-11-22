# nRF application makefile
PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52832
SDK_VERSION = 15
SOFTDEVICE_MODEL = s132

# Source and header files with yakindu stuff
APP_HEADER_PATHS += \
	. \
	../pixy-spi \
	./src-gen \
	./kobuki \
	./src

APP_SOURCE_PATHS += \
	. \
	../pixy-spi/ \
	./src-gen/ \
	./kobuki/ \
	./

APP_SOURCES = \
	main.c \
	state.c \
	util.c \
	helper_funcs.c \
	kobukiActuator.c \
	kobukiSensor.c \
	kobukiSensorPoll.c \
	kobukiUART.c \
	kobukiUtilities.c \
	Robot_template.c \
	Robot_template.c \
	pixy.c \
	nrf_spi.c

CFLAGS += \
	-DNRFX_SPI2_ENABLED \
	-DSPI2_ENABLED

BUCKLER_DIR = ../buckler/software/

# Path to base of nRF52-base repo
NRF_BASE_DIR = $(BUCKLER_DIR)nrf52x-base/

# Include board Makefile (if any)
include $(BUCKLER_DIR)boards/buckler_revB/Board.mk

# Include main Makefile
include $(NRF_BASE_DIR)make/AppMakefile.mk
