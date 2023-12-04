uses newexternal;
 external declare fed in c;
endexternal; 
 
 external load fed;
      'simple.a'
      ;;; '-L/home/mhl/rti/RTI-1.3NGv4/Linux-rh62-i386-egcs-2.91.66-opt-mt/lib' 
      ;;;      '-L/usr/i386-glibc21-linux' 
      ;;;      '/lib/libpthread-0.9.so'
      ;;;'-lACE'
      ;;; '-lpthread'
      ;;;      '/lib/libpthread.so.0' 
      ;;;      '/lib/librt.so.1'
      ;;;      '-ldl'
      ;;;            '-lRTI-NG' 
      ;;;      '-lrtiLauncher'
      ;;;'-lRTI-NG'
      ;;;           '/home/mhl/rti/RTI-1.3NGv4/Linux-rh62-i386-egcs-2.91.66-opt-mt/lib/libfedtime.so' 
      ;;;'-lpthread'
      
      ;;;      '/lib/libc.so.6'
      ;;;      '/lib/libpthread.so.0'
      ;;;     '-lrt'
      ;;;     '/lib/librt-2.2.4.so' 
      ;;;     '-ldl' 
     ;;;'simple.so'
      endexternal;

/*
exload rti ['-L/home/mhl/rti/RTI-1.3NGv4/Linux-rh62-i386-egcs-2.91.66-opt-mt/lib' '-L/usr/i386-glibc21-linux' '-lRTI-NG' '/home/mhl/rti/RTI-1.3NGv4/Linux-rh62-i386-egcs-2.91.66-opt-mt/lib/libfedtime.so' '-lpthread' '/lib/librt-2.2.4.so' '-ldl' 'simple.so']
inc(x):int

endexload;
*/
