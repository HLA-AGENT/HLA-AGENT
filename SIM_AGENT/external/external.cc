#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>



//#include <string.h>

extern "C" struct callback_t *foo(unsigned long, unsigned long);
extern "C" struct callback_t *foo2(int);
extern "C" struct AttributeHandleValuePair *foo3(int);
extern "C" struct NODE *foo4(void);
extern "C" void prnString(char *);
extern "C" void prnNode(NODE *);
extern "C" void takePtr(int *);
extern "C" int * returnPtr(void);
extern "C" void handle(unsigned long *);
extern "C" void openlib(void);

struct callback_t {unsigned long x,y;};

struct AttributeHandleValuePair
{
  unsigned long Id;
  char *value;
  unsigned long length;
};

struct NODE {
  NODE *Next;
  unsigned long objectHandle;
  unsigned long classHandle;
  unsigned long eventRetractionHandle;
  struct AttributeHandleValuePair *AHVPset;
  char* tag;
  char* name;  
};

NODE *foo4(){
  
    NODE *a_node;
    AttributeHandleValuePair *ahvpSet;
    
    a_node = new NODE;
    ahvpSet = new AttributeHandleValuePair;
    
    a_node->Next =NULL;
    a_node->objectHandle = 3;
    a_node->classHandle = 4;
    a_node->eventRetractionHandle = 5;
    a_node->AHVPset = foo3(5);
    
    a_node->tag="The Tag";
    a_node->name="The Name";
    
    return a_node;

  }

callback_t *foo(unsigned long a, unsigned long b)
{
  static callback_t a_callback;
  a_callback.x = a;
  a_callback.y = b;
  return(&a_callback);
}

callback_t *foo2(int x)
{  
  //  static struct _two_shorts *l;
  callback_t *l;
  l= new callback_t[x];
  for(int i=0;i<x;i++){
    l[i].x=i;
    l[i].y=i;
  }
  return(l);
}


int sizeOfArray(unsigned long *attributeArray)
{
  int count=0;
  int x=sizeof(unsigned long);


  while(*(&attributeArray+x)!=NULL)
    {
      x+=sizeof(unsigned long);
      count +=1;
    }
  return count;
}


AttributeHandleValuePair *foo3(int x)
{
  AttributeHandleValuePair *ahvpSet;
  char *theValue;
  ahvpSet= new AttributeHandleValuePair[x];


  
  for(int i=0;i<x;i++){
    ahvpSet[i].Id=i;
    theValue = new char[i];
      
      for(int j=0;j<i;j++){
	theValue[j]='j';
      }
 
    ahvpSet[i].value=theValue;
    ahvpSet[i].length=i;
  }
  return(ahvpSet);
}

int inc(int x){
  return x+1;
}

   
void prnString(char *str)
{
  cout << str<<"\n";
  //strcat(str," Hello");
  
}

void prnNode(NODE *a_node){
//Prints out a NODE result from foo4
  cout <<"The node\n";
  //Print the prt in NEXT
  cout <<"Next:"<<a_node->Next<<"\n";
  //Print out object handle
  cout <<"Next:"<<a_node->objectHandle<<"\n";
  cout <<"Next:"<<a_node->classHandle<<"\n";
  cout <<"Next:"<<a_node->eventRetractionHandle<<"\n";
  for(int i=0;i<5;i++){
    cout <<"ID:"<<a_node->AHVPset[i].Id;
    cout <<" Value:";
    for(int j=0;j<a_node->AHVPset[i].length;j++){
      cout <<a_node->AHVPset[i].value[j];
    }
    cout<<" lenth:";
    cout <<a_node->AHVPset[i].length;
    cout <<"\n";
  }
  cout <<a_node->tag<<"\n";
  cout <<a_node->name;
  cout <<"\n";
}

int *returnPtr()
{
  int x=3;
  int *y;

  y=&x;
  return y;
}

int *point(int *x)
{
  int *y;
  int z;
  
  z=(*x)+4;
  y=&z;
  return y;
}

void takePtr(int *x){
  cout << "x: "<<*x<<" x: "<<x<<"\n";
}

   
