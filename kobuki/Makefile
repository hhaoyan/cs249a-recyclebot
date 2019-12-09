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
	../buckler/software/libraries/kobuki/ \
	./src-gen \
	./kobuki \
	./src

APP_SOURCE_PATHS += \
	. \
	../pixy-spi/ \
	../buckler/software/libraries/kobuki/ \
	./src-gen/ \
	./kobuki/ \
	./

APP_SOURCES += \
	kobukiActuator.c \
	kobukiSensor.c \
	kobukiSensorPoll.c \
	kobukiUART.c \
	kobukiUtilities.c \

APP_SOURCES += \
	platform_bluetooth.c \
	platform_kobuki.c\
	main.c \
	Path_finding.c \
	Path_finding_2.c \
	Trash_carrier.c \
	helper_funcs.c \
	Rotate.c \
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
