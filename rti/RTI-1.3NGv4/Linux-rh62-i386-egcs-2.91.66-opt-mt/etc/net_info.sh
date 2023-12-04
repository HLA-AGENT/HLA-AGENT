#!/bin/sh

#
# This path is intended to cover all the likely locations ON ALL PLATFORMS.
#
# NOTE, we have /usr/sbin before /sbin, because it makes a difference in the execution of ifconfig.
#
PATH=/bin:/usr/sbin:/usr/bin:/etc:/usr/etc:/sbin:/usr/local/bin:/usr/share/bin:/usr/local/gnu/bin:$PATH


echo ""
echo "Interface Configuration:"
echo ""

#false; if [ $? -eq 0 ]; then
if [ `uname` = 'Linux' ]; then

  #
  # the format of this out put on Linux is not conformant;
  # but we'll worry about that at some later date.
  #
  ifconfig -a

else

  ifconfig -a | nawk '/^\t/ { sub("\t"," "); l = l " " $0; next }
  { if ( length(l) ) { print l }
    l = $0 }
END { print l }' | nawk ' { sub("=[^<]*"," "); 
  print $1
  for ( i = 2; i < NF; i+=2 ) {
  print "\t" $i ": " $(i+1)
  }
}'

fi
#fi

#
# Linux and Irix include the Netmask info in netstat -r anyway.
# To get it on Solaris, add a -v.
#
if [ `uname` = 'SunOS' ]; then
  verbose="-v"
else
  verbose=""
fi

#
# the formats of the outputs of these are different on each of the 3 major platforms!
#

echo ""
echo "Network Status:"
echo ""

echo ""
echo "Interfaces:"
echo ""
netstat -in

echo ""
echo "Routing Tables:"
echo ""
netstat -rn $verbose

# note, netstat -p appears to be Solaris-specific.  It might be mostly redundant anyway...
# furthermore, the output of netstat -p is the same as arp -a.
# adding -n causes the addresses to remain unresolved IP numbers.
# arp -a has no way to do this, AFAICT.
if [ `uname` = 'SunOS' ]; then
  echo ""
  echo "Address Resolution Tables:"
  echo ""
  netstat -pn
fi

echo ""
echo "Address Resolution Protocol Tables:"
echo ""
arp -a
echo ""


