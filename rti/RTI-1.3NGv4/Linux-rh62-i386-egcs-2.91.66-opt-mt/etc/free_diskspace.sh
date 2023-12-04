#!/bin/sh

#
# This path is intended to cover all the likely locations ON ALL PLATFORMS.
#
PATH=/bin:/sbin:/usr/bin:/etc:/usr/etc:/usr/sbin:/usr/local/bin:/usr/share/bin:/usr/local/gnu/bin

if [ `uname` = 'Linux' ]; then
  awkx=awk
else
  awkx=nawk
fi

#df -k | $awkx -f $HOME/work/ConfigTool/df.awk
df -k | $awkx '
BEGIN { "pwd" | getline pwd; longest = ""; free = 0; }
END { free = int( free / 1024 ); print "Free space on this volume: " free " Megabytes" }

NR == 1 { next }

 {
   dir = $NF
   pos = match( pwd, dir )
   if ( pos == 1 ) {
     if ( length( dir ) > length( longest ) ) {
       longest = dir
       free = $(NF-2)
     }
   }
 }
'

