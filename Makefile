# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

BOARD_TAG    = mega328
ISP_PORT = /dev/ttyACM0
MONITOR_PORT = /dev/ttyACM0

ARDMK_DIR=/usr/share/arduino

USER_LIB_PATH = libraries

CXXFLAGS = -Wno-deprecated-declarations -g3

MONITOR_BAUDRATE = 115200

ARDUINO_LIBS = AFMotor \
	       Encoder \
	       InkShield

-include $(ARDMK_DIR)/Arduino.mk

EXTRA_FLAGS = -Wall -Werror
# !!! Important. You have to use make ispload to upload when using ISP programmer
