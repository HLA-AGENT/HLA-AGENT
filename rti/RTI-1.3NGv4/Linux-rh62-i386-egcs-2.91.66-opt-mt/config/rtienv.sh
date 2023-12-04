#!/bin/sh
#---------------------------------------------------------------
# NAME:
#   rtienv.sh
#
# AUTHOR(S): Dave Meyer
# 
# SUMMARY:
#   Set up runtime environment to use HLA RTI v. 1.3 NG
#
# USAGE:
#   $ . rtienv.sh
#
# HISTORY:
#   01/06/98 J. Olszewski
#     Mods to support RTI 2.0
#   02/13/98 D. Meyer
#     Rewrote in sh 
#---------------------------------------------------------------

#---------------------------------------------------------------
# Set up environment to use HLA RTI v. 1.3NG
#---------------------------------------------------------------
RTI_HOME=/home/mhl/rti/RTI-1.3NGv4
RTI_BUILD_TYPE=Linux-rh62-i386-egcs-2.91.66-opt-mt

#---------------------------------------------------------------
# Platform Specific Configuration
#---------------------------------------------------------------
case $RTI_BUILD_TYPE in
  SunOS-5.6-Sparc-SPRO-4.2-dbg-mt|SunOS-5.6-Sparc-SPRO-4.2-opt-mt|\
  SunOS-5.7-Sparc-SPRO-5.0-dbg-mt|SunOS-5.7-Sparc-SPRO-5.0-opt-mt|\
  SunOS-5.7-Sparc-SPRO-5.1-dbg-mt|SunOS-5.7-Sparc-SPRO-5.1-opt-mt|\
  SunOS-5.8-Sparc-SPRO-5.1-dbg-mt|SunOS-5.8-Sparc-SPRO-5.1-opt-mt|\
    SunOS-bogus)
    LD_LIBRARY_PATH="$RTI_HOME/$RTI_BUILD_TYPE/lib:${LD_LIBRARY_PATH}"
    export LD_LIBRARY_PATH
    ;;
  SunOS-5.6-Sparc-gcc-2.95.2-dbg-mt|SunOS-5.6-Sparc-gcc-2.95.2-opt-mt)
    LD_LIBRARY_PATH="$RTI_HOME/$RTI_BUILD_TYPE/lib:${LD_LIBRARY_PATH}"
    export LD_LIBRARY_PATH
    ;;
  SunOS-5.7-Sparc-gcc-2.95.2-dbg-mt|SunOS-5.7-Sparc-gcc-2.95.2-opt-mt)
    LD_LIBRARY_PATH="$RTI_HOME/$RTI_BUILD_TYPE/lib:${LD_LIBRARY_PATH}"
    export LD_LIBRARY_PATH
    ;;
  IRIX-6.5-MIPSpro-7.2-dbg-mt|IRIX-6.5-MIPSpro-7.2-opt-mt|IRIX-6.5-MIPSpro-7.2-mips3-opt-mt|IRIX-6.5.6-MIPSpro-7.3-dbg-mt|IRIX-6.5.6-MIPSpro-7.3-opt-mt|IRIX-6.5.6-MIPSpro-7.3-mips3-opt-mt)
    LD_LIBRARYN32_PATH="$RTI_HOME/$RTI_BUILD_TYPE/lib:${LD_LIBRARYN32_PATH}"
    export LD_LIBRARYN32_PATH
    ;;
  Linux-rh61-i386-egcs-1.1.2-dbg-mt|Linux-rh61-i386-egcs-1.1.2-opt-mt|Linux-rh62-i386-egcs-2.91.66-dbg-mt|Linux-rh62-i386-egcs-2.91.66-opt-mt)
    LD_LIBRARY_PATH="$RTI_HOME/$RTI_BUILD_TYPE/lib:${LD_LIBRARY_PATH}"
    export LD_LIBRARY_PATH
    ;;
  VxWorks-5.3.1-PPC-GreenHills-1.8.9-opt-mt|VxWorks-5.3.1-PPC-GreenHills-1.8.9-dbg-mt)
    ;;
 *)
    echo "ERROR: Unsupported OS type: $RTI_OS"
    echo "ERROR: Unable to determine OS type using uname. Config incomplete."
    ;;
esac
  
#---------------------------------------------------------------
# Setup search path for commands
#---------------------------------------------------------------
#PATH="${RTI_HOME}/${RTI_BUILD_TYPE}/bin:${PATH}"

export RTI_HOME RTI_BUILD_TYPE

if [ -z "$QUIET_RTI_SCRIPTS" ]; then
echo ''
echo "RTI_HOME           : $RTI_HOME"
echo "RTI_BUILD_TYPE     : $RTI_BUILD_TYPE"
case $RTI_BUILD_TYPE in
  IRIX-6.5-MIPSpro-7.2-dbg-mt|IRIX-6.5-MIPSpro-7.2-opt-mt|IRIX-6.5-MIPSpro-7.2-mips3-opt-mt|IRIX-6.5.6-MIPSpro-7.3-dbg-mt|IRIX-6.5.6-MIPSpro-7.3-opt-mt|IRIX-6.5.6-MIPSpro-7.3-mips3-opt-mt)
    echo "System Libraries   : $LD_LIBRARYN32_PATH"
    ;;
  *)
    echo "System Libraries   : $LD_LIBRARY_PATH"
    ;;
esac
echo ''
fi

