# Linking object files
sample: Clsfunc.o appmain.o
		cc -o sample Clsfunc.o appmain.o
		echo sample: make complete

# Compiling source files
Clsfunc.o: classdef.h
		cc -c Clsfunc.c
appmain.o: classdef.h
		cc -c appmain.c


