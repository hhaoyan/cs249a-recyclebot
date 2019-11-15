# nRF application makefile
PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52832
SDK_VERSION = 15
SOFTDEVICE_MODEL = s132

# Source and header files with yakindu stuff
APP_HEADER_PATHS += .
APP_SOURCE_PATHS += .

APP_SOURCES = \
	main.c \
	nrf_spi.c \
	pixy.c \

BUCKLER_DIR = ../buckler/software/

# Path to base of nRF52-base repo
NRF_BASE_DIR = $(BUCKLER_DIR)nrf52x-base/

# Include board Makefile (if any)
include $(BUCKLER_DIR)boards/buckler_revB/Board.mk

# Include main Makefile
include $(NRF_BASE_DIR)make/AppMakefile.mk
