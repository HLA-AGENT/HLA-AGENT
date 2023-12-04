#!/bin/sh

#
# in case HOST not set:
#
#if [ "$HOST" = "" ]; then
#  HOST=localhost
#fi
#export HOST
#echo HOST=$HOST

#
# This path is intended to cover all the likely locations ON ALL PLATFORMS.
#
PATH=/bin:/sbin:/usr/bin:/etc:/usr/etc:/usr/sbin:/usr/local/bin:/usr/share/bin:/usr/local/gnu/bin:$PATH

#
# turns out, 'which' is csh-specific.  In fact, it reads .cshrc, rather than the env!
# and 'whence' is ksh-specific.  so I write my own.
#
Which() {
  txff=$1
  echo $PATH | tr : '\012' | while read txfd
  do
    if [ -x $txfd/$txff ]
    then
      echo $txfd/$txff
      return 0
    fi
  done

  if [ $? -eq 0 ]; then
    return 0
  else
    return 1
  fi

# or maybe we simply need to:
#   return 1
}

#
# two args:
# 1. switch for CC to request version info, typically -V or -version.
# 2. sh cmd string to report version info; will be eval'ed.
#
test_CC() {
  verswitch=$1
  report_ver=$2

  cc_loc=`Which CC`
  if [ $? -eq 0 ]; then
      echo "CC Found as: $cc_loc"

      ver=`$cc_loc $verswitch 2>&1`
      if [ $? -eq 0 ]; then
	eval $report_ver
      else
        echo "Compiler Version: Not Found"
      fi
  else
    echo "No CC in path."
  fi
}


if [ `uname` = 'SunOS' ]; then

  #
  # note, just because this doesn't find the package, doesn't mean it's not available.
  # this is more of a hint.
  #
  cpl=`pkginfo SPROcpl 2>&1`
  if [ $? -eq 0 ]; then
    echo $cpl | nawk ' { $1 = "Compiler"; $2 = "Package:"; print }'
    basedir=`pkginfo -l SPROcpl 2>&1 | nawk '/BASEDIR:/ { print $2 }'`
    PATH=$PATH:$basedir/SUNWspro/bin
  else
    echo "Compiler Package: Not Installed Locally"
  fi

  test_CC "-V" "echo \$ver | nawk ' { \$1 = \"Compiler Version:\"; print }'"

elif [ `uname` = 'IRIX' ]; then

  #
  # note, just because this doesn't find the package, doesn't mean it's not available.
  # this is more of a hint.
  #
  showprods -M -n -D 1 | nawk '$2 == "c++_dev" { if ( $1 == "I" ) { ver = $NF } }
BEGIN { ver = "Not Installed Locally" }
END { print "Compiler Package: " ver }'

  test_CC "-version" "echo \"Compiler Version: \$ver\" | sed 's/: Version//'"

elif [ `uname` = 'Linux' ]; then

  #
  # These need error checking:
  #
  echo "Compiler Package:" `rpm -q egcs`
  echo "Compiler Version:" `egcs --version`

else
  echo "Unknown OS:" `uname`
fi


