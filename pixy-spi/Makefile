# nRF application makefile
PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52832
SDK_VERSION = 15
SOFTDEVICE_MODEL = s132

# Source and header files with yakindu stuff
APP_HEADER_PATHS += .
APP_SOURCE_PATHS += .

ZBAR_SRC := \
	zbar/zbar/config.c \
	zbar/zbar/img_scanner.c \
	zbar/zbar/scanner.c \
	zbar/zbar/decoder.c \
	zbar/zbar/symbol.c \
	zbar/zbar/image.c \
	zbar/zbar/refcnt.c \
	zbar/zbar/error.c \
	zbar/zbar/decoder/qr_finder.c \
	zbar/zbar/decoder/ean.c \
	zbar/zbar/decoder/i25.c \
	zbar/zbar/decoder/code39.c \
	zbar/zbar/decoder/code128.c \
	zbar/zbar/qrcode/qrdec.c \
	zbar/zbar/qrcode/isaac.c \
	zbar/zbar/qrcode/rs.c \
	zbar/zbar/qrcode/bch15_5.c \
	zbar/zbar/qrcode/binarize.c \
	zbar/zbar/qrcode/qrdectxt.c \
	zbar/zbar/qrcode/util.c

APP_SOURCES = \
	main.c \
	nrf_spi.c \
	pixy.c \
	qrcode-zbar.c \
	$(ZBAR_SRC)

BUCKLER_DIR = ../buckler/software/

# Path to base of nRF52-base repo
NRF_BASE_DIR = $(BUCKLER_DIR)nrf52x-base/

# Include board Makefile (if any)
include $(BUCKLER_DIR)boards/buckler_revB/Board.mk

# Include main Makefile
include $(NRF_BASE_DIR)make/AppMakefile.mk
