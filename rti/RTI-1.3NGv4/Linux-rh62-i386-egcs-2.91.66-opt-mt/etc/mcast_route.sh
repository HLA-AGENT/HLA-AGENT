#!/bin/sh

#
# This path is intended to cover all the likely locations ON ALL PLATFORMS.
#
# NOTE, we have /usr/sbin before /sbin, because it makes a difference in the execution of ifconfig.
#
PATH=/bin:/usr/sbin:/usr/bin:/etc:/usr/etc:/sbin:/usr/local/bin:/usr/share/bin:/usr/local/gnu/bin:$PATH


echo ""
echo "Multicast:"
echo ""

route_of_netmask() {
  expected=$1
  verbose=""
  awkx=nawk
  col=3

  if [ `uname` = 'SunOS' ]; then
    verbose="-v"
    col=2
  elif [ `uname` = 'Linux' ]; then
    awkx=awk
  fi

  netstat -rn $verbose | $awkx "\$$col ~ /$expected/ { print \$1 }"
}


if [ `uname` = 'IRIX' ]; then
  mcast_route=`route_of_netmask 0xf0000000`
else
  mcast_route=`route_of_netmask 240.0.0.0`
fi

# if no mcast route was found, make sure there's a default.
if [ "$mcast_route" = "" ]; then

  echo "No Multicast Route configured."

  if [ `uname` = 'IRIX' ]; then
    # GUESSING:
    default_route=`route_of_netmask 0x00000000`
  else
    default_route=`route_of_netmask 0.0.0.0`
  fi

  if [ "$default_route" = "" ]; then
    echo "No Default Route configured."
    echo "Multicasting will not be possible until a Multicast Route or a Default Route is configured!"
  else
    echo "Multicast will use the Default Route."
  fi

else
  echo "Multicast Route:" $mcast_route
fi

