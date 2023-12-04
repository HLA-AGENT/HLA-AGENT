############################################################### -*- Makefile -*-
#
# Filename:  fedTime.mk
#
# Summary:   
# 
# Notes:
#   This file is only for use with GNU make version 3.78.1 or greater
#
################################################################################


OBJS = MyFedTime.o 

FEDTIME_LIB_DIR = .
LIBRARY = ${FEDTIME_LIB_DIR}/libfedtime.${LIB_SUFFIX}

RTI_ROOT_DIR = ${RTI_HOME}/${RTI_BUILD_TYPE}
RTI_INC_DIR = ${RTI_ROOT_DIR}/include

INC_PATH = -I${RTI_INC_DIR} -I../include

%.o : %.cpp
	${C++} -c ${C++FLAGS} ${INC_PATH} $< -o $@

default: ${LIBRARY}

${LIBRARY}: ${OBJS}
	${LD} ${LD_FLAGS} -o $@ ${OBJS}

clean:
	rm -rf *.o core *~ .depend Templates.DB ${LIBRARY}
