#!/bin/csh
#---------------------------------------------------------------
# NAME:
#   rtienv.csh
#
# AUTHOR(S): Jeff Olszewski
# 
# SUMMARY:
#   Set up runtime environment to use HLA RTI v. 1.3 NG
#
# USAGE:
#   % source rtienv.csh
#
# HISTORY:
#   01/06/98 J. Olszewski
#     Mods to support RTI 2.0
#---------------------------------------------------------------

#---------------------------------------------------------------
# Set up environment to use HLA RTI v. 1.3NG
#---------------------------------------------------------------

setenv RTI_HOME /home/mhl/rti/RTI-1.3NGv4
setenv RTI_BUILD_TYPE Linux-rh62-i386-egcs-2.91.66-opt-mt

switch ($RTI_BUILD_TYPE)
  case 'SunOS-5.8-Sparc-SPRO-5.1-dbg-mt':
  case 'SunOS-5.8-Sparc-SPRO-5.1-opt-mt':
  case 'SunOS-5.7-Sparc-SPRO-5.1-dbg-mt':
  case 'SunOS-5.7-Sparc-SPRO-5.1-opt-mt':
  case 'SunOS-5.7-Sparc-SPRO-5.0-dbg-mt':
  case 'SunOS-5.7-Sparc-SPRO-5.0-opt-mt':
  case 'SunOS-5.6-Sparc-SPRO-4.2-dbg-mt':
  case 'SunOS-5.6-Sparc-SPRO-4.2-opt-mt':
    if( ! $?LD_LIBRARY_PATH ) then
      setenv LD_LIBRARY_PATH 
    endif
    setenv LD_LIBRARY_PATH \
	 "${RTI_HOME}/${RTI_BUILD_TYPE}/lib:${LD_LIBRARY_PATH}"
    breaksw
  case 'SunOS-5.6-Sparc-gcc-2.95.2-dbg-mt':
  case 'SunOS-5.6-Sparc-gcc-2.95.2-opt-mt':
  case 'SunOS-5.7-Sparc-gcc-2.95.2-dbg-mt':
  case 'SunOS-5.7-Sparc-gcc-2.95.2-opt-mt':
    if( ! $?LD_LIBRARY_PATH ) then
      setenv LD_LIBRARY_PATH 
    endif
    setenv LD_LIBRARY_PATH \
	 "${RTI_HOME}/${RTI_BUILD_TYPE}/lib:${LD_LIBRARY_PATH}"
    breaksw
  case 'IRIX-6.5-MIPSpro-7.2-dbg-mt':
  case 'IRIX-6.5-MIPSpro-7.2-opt-mt':
  case 'IRIX-6.5-MIPSpro-7.2-mips3-opt-mt':
  case 'IRIX-6.5.6-MIPSpro-7.3-dbg-mt':
  case 'IRIX-6.5.6-MIPSpro-7.3-opt-mt':
  case 'IRIX-6.5.6-MIPSpro-7.3-mips3-opt-mt':
    if( ! $?LD_LIBRARYN32_PATH ) then
      setenv LD_LIBRARYN32_PATH 
    endif
    setenv LD_LIBRARYN32_PATH \
	 "${RTI_HOME}/${RTI_BUILD_TYPE}/lib:${LD_LIBRARYN32_PATH}"
    breaksw  
  case 'Linux-rh61-i386-egcs-1.1.2-dbg-mt':
  case 'Linux-rh61-i386-egcs-1.1.2-opt-mt':
  case 'Linux-rh62-i386-egcs-2.91.66-dbg-mt':
  case 'Linux-rh62-i386-egcs-2.91.66-opt-mt':
   if( ! $?LD_LIBRARY_PATH ) then
      setenv LD_LIBRARY_PATH 
    endif
    setenv LD_LIBRARY_PATH \
	 "${RTI_HOME}/${RTI_BUILD_TYPE}/lib:${LD_LIBRARY_PATH}"
    breaksw
  case 'VxWorks-5.3.1-PPC-GreenHills-1.8.9-dbg-mt':
  case 'VxWorks-5.3.1-PPC-GreenHills-1.8.9-opt-mt':
    breaksw
  case '':
  default:
    echo "ERROR: Unsupported OS type: $RTI_BUILD_TYPE"
    echo "ERROR: Unable to determine OS type. Config incomplete."
endsw

#---------------------------------------------------------------
# Setup search path for commands
#---------------------------------------------------------------
#setenv PATH "${PATH}:${RTI_HOME}/${RTI_BUILD_TYPE}/bin"

if (! $?QUIET_RTI_SCRIPTS) then
echo ''
echo "RTI_HOME           : $RTI_HOME"
echo "RTI_BUILD_TYPE     : $RTI_BUILD_TYPE"
switch ($RTI_BUILD_TYPE)
  case 'IRIX-6.5-MIPSpro-7.2-dbg-mt':
  case 'IRIX-6.5-MIPSpro-7.2-opt-mt':
  case 'IRIX-6.5-MIPSpro-7.2-mips3-opt-mt':
  case 'IRIX-6.5.6-MIPSpro-7.3-dbg-mt':
  case 'IRIX-6.5.6-MIPSpro-7.3-opt-mt':
  case 'IRIX-6.5.6-MIPSpro-7.3-mips3-opt-mt':
    echo "System Libraries   : $LD_LIBRARYN32_PATH"
    breaksw  
  default:
    echo "System Libraries   : $LD_LIBRARY_PATH"
    breaksw
endsw
echo ''
endif

