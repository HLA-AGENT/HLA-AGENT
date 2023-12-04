/*--------------------------------------
final type should look something like,

p_typespec

         /*bytearray for storing attribute values*/
        /* Problem with undefined size on arrays
           see line 660 ref defstruct            */


        bytearray: byte[],

        /*AttributeHandleValuePairSet*/
        attributeHandleValuePairSet{
                    objectHandle: ulong,
                    value:bytearray
               },
        /*AttributeHandleSet*/
        attributeHandleSet{
                    objectHandle: ulong
              },

        /*The callback datatype*/
        callback{
                    classHandle :ulong,
                    objectHandle :ulong,
                    eventRetractionHandle :ulong,
                    ahvpset :attributeHandleValuePairSet,
                    ahset :attributeHandleSet,
                    name :full,
                    tag :full,
                    time :timetype   /*????*/
                    };
--------------------------------------*/
/*
p_typespec
      classHandle :long,
      objectHandle :long
      ;

defclass callback [external_ptr]{
               /*     xptr_props :full,*/
                    >-> xptr_ptr   :exptr,
                    classHandle:classHandle,
                    objectHandle:objectHandle
            };

defexacc: callback;

exload libtest [libtest]
foo(x) :exptr <-'foo__Fl',
foo2(x) :ulong <-'foo2__Fl'
endexload;
*/

 uses newexternal;

    external declare test in c;

        typedef struct _two_longs {long x,y;} callback;

    endexternal;


/*
vars acb = initcallback(false);
4-> acb#:x;
acb#:x=>
*/


/*


conscallback(consexternal_ptr(),5,6)->x;
x=>
exacc foo2(4294967294)->x;
x.xptr_ptr=>
x.classHandle=>
classHandle(x)->
exacc foo(3)->y;
y=>
foo=>
  */
