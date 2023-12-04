    uses newexternal;

    external declare test in c;

    typedef struct _the_callback
    {unsigned long x,y;} Callback;
    
    typedef struct _another_callback
    {
     unsigned long Id;
     int *value;
     unsigned long length;
     } AttributeHandleValuePair;
    
    typedef struct _yet_another_callback 
    {
    ;;;NODE *Next;
     unsigned long objectHandle;
     unsigned long classHandle;
     unsigned long eventRetractionHandle;
     struct AttributeHandleValuePair *AHVPset;
     char* tag;
     char* name;
     }NODE;
    
    Callback *foo(x, y);
    unsigned long x;
    unsigned long y;
    {}
    
    NODE *foo4();
    {}
    
    void openlib();
    {}
    
    AttributeHandleValuePair *foo3(x)
    int x;
    {}
    
    Callback *foo2(x);
    int x;
    {}
    

    void prnString(astring);
    char *astring;
    {}
    void prnNode(a_node);
    NODE *a_node;
    {}

    endexternal;


    external load test;
    ;;; 'test.so'     
     'external.so'	
    endexternal;





/*Prints out result array from foo2*/

define start(array);
    ''=>
    for x from 0 to 10 do
    x><' x:'><array(x)#:x><' y:'><array(x)#:y=>
    endfor;
enddefine;

/*

    foo3(14)->array2;
    array2(2)#:Id=>
p    array2(7)#:value(4)=>
    foo2(11)->array1;
    start();
    foo(1,2)(0)->acb;
    foo4()(0)->node;
    node#:objectHandle=>
    node#:AHVPset(4)#:value(3)=>
    node#:tag(12)=>
    acb#:x=>
    acb#:y=>
    consstring(`H`,`e`,`l`,`l`,4)->arg;
    arg=>
    prnString('hello');
    prnNode(node);


*/

/*Array stuff

vars i = array_of_integer([0 9]);
4294967295->i(4);
i(3)=>
*/
