############################################################### -*- Makefile -*-
#
# Filename:  Makefile
#
# Summary:   
# 
# Notes:
#   This file is only for use with GNU make version 3.78.1 or greater
#
################################################################################

C++FLAGS = -g -O3

LDFLAGS += -g -O3 \
	${LIB_PATH} -L/usr/i386-glibc21-linux


LIBS = \
	-lRTI-NG -lfedtime \
	-lpthread -lrt -ldl

C++ = egcs++

include helloWorld.mk
