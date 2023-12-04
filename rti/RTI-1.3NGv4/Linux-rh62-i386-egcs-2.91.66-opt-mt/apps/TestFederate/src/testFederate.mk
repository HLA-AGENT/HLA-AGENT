############################################################### -*- Makefile -*-
#
# Filename:  helloWorld.mk
#
# Summary:   
# 
# Notes:
#   This file is only for use with GNU make version 3.78.1 or greater
#
################################################################################

OBJS = \
    UxCplus.o \
    UxMethod.o \
    UxXt.o \
    commandRecordingShell.o \
    endTestModeDialog.o \
    federateAmbServices.o \
    fileSelectionShell.o \
    genericPromptDialog.o \
    helpSelectionShell.o \
    outputSelectionShell.o \
    quitQuestionDialog.o \
    replayShell.o \
    rtiAmbInterface.o \
    testFederateShell.o \
    testFederate.o


TESTFEDERATE_INC_DIR = ../include
TESTFEDERATE_EXE_DIR = ../data
EXECUTABLE = ${TESTFEDERATE_EXE_DIR}/testFederate

RTI_ROOT_DIR = ${RTI_HOME}/${RTI_BUILD_TYPE}
RTI_INC_DIR 	= ${RTI_ROOT_DIR}/include
RTI_LIB_DIR  	= ${RTI_ROOT_DIR}/lib

MOTIF_INC_DIR = ${MOTIF_ROOT_DIR}/include
MOTIF_LIB_DIR = ${MOTIF_ROOT_DIR}/lib

X_INC_DIR = ${X_ROOT_DIR}/include
X_LIB_DIR = ${X_ROOT_DIR}/lib

INC_PATH = \
	-I${TESTFEDERATE_INC_DIR} \
	-I${MOTIF_INC_DIR} \
	-I${RTI_INC_DIR} \
	-I${X_INC_DIR} 

LIB_PATH = \
	-L${RTI_LIB_DIR} \
	-L${MOTIF_LIB_DIR} \
	-L${X_LIB_DIR}

%.o : %.cpp
	@echo " "
	@echo "Compiling $< ... "
	@echo " "
	${C++} -c ${C++FLAGS} ${INC_PATH} $< -o $@

default: ${EXECUTABLE}

${EXECUTABLE}: ${OBJS}
	@echo " "
	@echo "Linking ${EXECUTABLE} ... "
	@echo " "
	${C++} ${LDFLAGS} ${OBJS} -o $@ ${LIBS}

clean:
	rm -rf *.o core *~ .depend Templates.DB ${EXECUTABLE}

