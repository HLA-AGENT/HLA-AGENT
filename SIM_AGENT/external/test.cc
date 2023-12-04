#include <iostream.h>

struct callback{
  long classHandle;
  long objectHandle;
};

callback *foo(long x)
{
  callback y;
  y.classHandle=3;
  y.objectHandle=4;
  return &y;
}

unsigned long foo2(long x){
  return x+2;
}



int main()
{

  callback *theCallback;
  
  
  theCallback = foo(4);
  
  cout <<"ClassHandle: "<<theCallback->classHandle;
  cout <<"objectHandle: "<<theCallback->objectHandle;
  
  return 0;

}
