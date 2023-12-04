#!/bin/sh
      #
      # first parameter is number of frames in
      # second is number of output frames per input frame
      # It creates links called link#.ppm -> frame#.ppm.  Then the
      # resulting links can be used to generate an mpeg that runs slower
      # than the default 30 frames per second.
      #
      framein=$1
      replicate=$2
      icount=1
      lcount=1
      while [ $icount -le $framein ]
      do
        reps=$replicate
        while [ $reps -ge 1 ]
        do
          ln -s pic$icount.yuv link$lcount.yuv
          reps=`expr $reps - 1`
          lcount=`expr $lcount + 1`
        done
        icount=`expr $icount + 1`
      done

