############################################################### -*- Makefile -*-
#
# Filename:  Makefile
#
# Author(s): J. Russell Noseworthy
#
# Summary:   This file is only for use with GNU make version 3.78.1 or greater
#
#		       SAIC/OSC/VTC (All Rights Reserved)
################################################################################

subdirs := src


_goals := ${filter-out ${subdirs},${MAKECMDGOALS}}

all ${_goals}:    ${subdirs}

${subdirs}:
	${MAKE} -C $@ ${_goals}

.PHONY: all ${subdirs}
	echo ""  ${_includes}
