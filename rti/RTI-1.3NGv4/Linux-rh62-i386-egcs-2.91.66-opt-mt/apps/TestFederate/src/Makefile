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

MOTIF_ROOT_DIR = /usr
X_ROOT_DIR = /usr/openwin


C++FLAGS = \
        -fexceptions \
        -D_POSIX_PTHREAD_SEMANTICS \
        -D_POSIX_C_SOURCE=199506L \
	-DXT_CODE -D_GNU_SOURCE  \
	-DMOTIF -DXT_CODE -D_GNU_SOURCE\
        -D__EXTENSIONS__ 


LDFLAGS = ${LIB_PATH}

LIBS = \
	-lRTI-NG \
	-lfedtime \
	-lXm -lXt -lX11 \
	-lposix4 -lpthread

C++ = g++

include testFederate.mk
