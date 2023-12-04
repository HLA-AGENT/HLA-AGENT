#!/bin/sh

#
# This path is intended to cover all the likely locations ON ALL PLATFORMS.
#
PATH=/bin:/sbin:/usr/bin:/usr/bsd:/etc:/usr/etc:/usr/sbin:/usr/local/bin:/usr/share/bin:/usr/local/gnu/bin

if [ `uname` = 'Linux' ]; then
  awkx=awk
else
  awkx=nawk
fi


#
#                                         Solaris             Irix       Linux
#  -a  all information
#  -p  host processor type              | sparc             | mips     |
#  -m  machine (hardware) type          | sun4u             | IP32     |
#  -s  operating system name            | SunOS             | IRIX     |
#  -r  operating system release         | 5.6               | 6.5      |
#  -n  machine's network node hostname  | hla16             | hla24    |
#
#  -v  operating system version         | Generic_105181-10 | 10181059 |
#  -i  implementation platform          | SUNW,Ultra-2      |          |
#
# Solaris also supports:
#  -X	extended info/format, SCO-compatible.  Not portable.  
#       Contains interesting info, though!  E.g. number of processors.
#
# Irix:
# -d, -V do some version number decoding, but I'm not sure how to get them to work.
# it doesn't matter, though, because I don't use -v.
#


echo Hostname: `hostname`
echo Network node name: `uname -n`
echo Architecture: `uname -p -m`
echo OS: `uname -s -r`

echo ""
echo "Additional Platform Information:"
echo ""

if [ `uname` = 'IRIX' ]; then

  inst -H | grep MACHINE | sed 's/MACHINE=/Machine: /'
  hinv

elif [ `uname` = 'SunOS' ]; then

  major=`uname -r | cut -d. -f 1`
  minor=`uname -r | cut -d. -f 2`

  if [ $major = '5' ]; then

    if [ $minor = '6' -o $minor = '7' ]; then
      uname -X | nawk '/NumCPU/ { $1 = "Number of"; $2 = "Processors:"; print }'
    fi

    if [ -r /etc/release ]; then
      echo "OS Release:" `head -1 /etc/release`
    else
      echo "No Solaris release information!"
    fi

    echo Kernel Version: `uname -v`
    echo Kernel ID: `uname -i`

  else
    echo "Unsupported Solaris version."
  fi

elif [ `uname` = 'Linux' ]; then

  if [ -r /etc/redhat-release ]; then
    echo "OS Release:" `cat /etc/redhat-release`
  else
    echo "Not a RedHat Linux installation!"
  fi

fi

echo ""

