#include <stdio.h>
#include <stdlib.h>
#include <RTI.hh>
#include "llist.hh"


//extern SimFederateAmbassador    fedAmb;
//extern RTI::RTIambassador       rtiAmb;

//#include "FedAmbGlobals.hh"
/*
struct NODE {
  NODE *Next;
  unsigned long objectHandle;
  unsigned long classHandle;
  unsigned long eventRetractionHandle;
  RTI::AttributeHandleValuePairSet& AHVPset;
  RTI::FedTime&                     theTime;
  RTI::AttributeHandleSet& theAttributes;
  const char* tag;
  const char* name;  
};

typedef struct Queue{
  NODE *head; 
  NODE *tail;
}Queue_t;

Queue_t *discoverObjectInstance_q;
Queue_t *reflectAttributeValues_q;
Queue_t *removeObjectInstance_q;
Queue_t *provideAttributeValueUpdate_q;
Queue_t *attributeOwnershipDivestitureNotification_q;
Queue_t *attributeOwnershipAcquisitionNotification_q;
Queue_t *attributeOwnershipUnavailable_q;
Queue_t *timeRegulationEnabled_q;
Queue_t *timeConstrainedEnabled_q;
Queue_t *timeAdvanceGrant_q;
*/
/*
void makeNODE(NODE *anode)
{
  anode->Next=NULL;
  anode->objectHandle=0;
  anode->classHandle=0;
  anode->eventRetractionHandle=0;
  //  anode->AHVPset= (RTI::AttributeHandleValuePairSet)malloc(sizeof(RTI::AttributeHandleValuePairSet));
  anode->theTime= RTI::FedTime::FedTime();
  //  anode->theAttributes=new RTI::AttributeHandleSet;
  anode->tag=NULL;
  anode->name=NULL;
}
*/

Queue_t *discoverObjectInstance_q= new Queue_t;
Queue_t *reflectAttributeValues_q= new Queue_t;
Queue_t *removeObjectInstance_q= new Queue_t;
Queue_t *provideAttributeValueUpdate_q= new Queue_t;
Queue_t *attributeOwnershipDivestitureNotification_q= new Queue_t;
Queue_t *attributeOwnershipAcquisitionNotification_q= new Queue_t;
Queue_t *attributeOwnershipUnavailable_q= new Queue_t;
Queue_t *timeRegulationEnabled_q= new Queue_t;
Queue_t *timeConstrainedEnabled_q= new Queue_t;
Queue_t *timeAdvanceGrant_q= new Queue_t;
/*
Queue_t *discoverObjectInstance_q;
Queue_t *reflectAttributeValues_q;
Queue_t *removeObjectInstance_q;
Queue_t *provideAttributeValueUpdate_q;
Queue_t *attributeOwnershipDivestitureNotification_q;
Queue_t *attributeOwnershipAcquisitionNotification_q;
Queue_t *attributeOwnershipUnavailable_q;
Queue_t *timeRegulationEnabled_q;
Queue_t *timeConstrainedEnabled_q;
Queue_t *timeAdvanceGrant_q;
*/
void makeQueue(Queue_t *Q)
{
  Q->head=NULL;
  Q->tail=NULL;
  Q->size=0;
 }

void enqueue(Queue_t *Q, NODE_t *newNode)
{
  if(Q->head ==NULL){
    Q->head = Q->tail = newNode;
    cout<<"Adding to empty list";
  }else{
    Q->tail->Next = newNode;
    Q->tail = newNode;
  }
  Q->size +=1;
}
/*  
void dequeue(Queue_t *Q, NODE *storageNode){
    storageNode = Q->head;
    Q->head=Q->head->Next;
    Q->size-=1;
  }
*/

NODE_t *dequeue(Queue_t *Q){
  NODE_t *temp = new NODE_t;
  temp = Q->head;
  if(Q->size==0){
    cout<<"Dequeue empty queue"<<endl;
    return NULL;
  }
  else{
    Q->head=Q->head->Next;
  Q->size-=1;
  if(Q->size==0)
    Q->head = NULL;
  return temp;
  }
}

void initialiseQueues(){
  /*Initialise pointers in queues*/
  makeQueue(discoverObjectInstance_q);
  makeQueue(reflectAttributeValues_q); 
  makeQueue(removeObjectInstance_q);
  makeQueue(provideAttributeValueUpdate_q);
  makeQueue(attributeOwnershipDivestitureNotification_q);
  makeQueue(attributeOwnershipAcquisitionNotification_q);
  makeQueue(attributeOwnershipUnavailable_q);
  makeQueue(timeRegulationEnabled_q);
  makeQueue(timeConstrainedEnabled_q);
  makeQueue(timeAdvanceGrant_q);
}


NODE_t *get_discoverObjectInstance()
{
  return dequeue(discoverObjectInstance_q);  
}
NODE_t *get_reflectAttributeValues()
{
  return dequeue(reflectAttributeValues_q);  
}
NODE_t *get_removeObjectInstance()
{
  return dequeue(removeObjectInstance_q);  
}
NODE_t *get_provideAttributeValueUpdate()
{
  return dequeue(provideAttributeValueUpdate_q);  
}
NODE_t *get_attributeOwnershipDivestitureNotification()
{
  return dequeue(attributeOwnershipDivestitureNotification_q);  
}
NODE *get_attributeOwnershipAcquisitionNotification()
{
  return dequeue(attributeOwnershipAcquisitionNotification_q);  
}
NODE_t *get_attributeOwnershipUnavailable()
{
  return dequeue(attributeOwnershipUnavailable_q);  
}
NODE_t *get_timeRegulationEnabled()
{
  return dequeue(timeRegulationEnabled_q);  
}
NODE_t *get_timeConstrainedEnabled()
{
  return dequeue(timeConstrainedEnabled_q);  
}
NODE_t *get_timeAdvanceGrant()
{
  return dequeue(timeAdvanceGrant_q);  
}


void printQueueSizes()
{
 cout<<"Queue sizes: "<<endl;
    cout<<"timeConstrainedEnabled "<<timeConstrainedEnabled_q->size<<endl;
    cout<<"reflectAttributeValues "<<reflectAttributeValues_q->size<<endl;
    cout<<"discoverObjectInstance "<<discoverObjectInstance_q->size<<endl;
    cout<<"removeObjectInstance "<<removeObjectInstance_q->size<<endl;
    cout<<"provideAttributeValueUpdate "<<provideAttributeValueUpdate_q->size<<endl;
    cout<<"attributeOwnershipDivestitureNotification "<<attributeOwnershipDivestitureNotification_q->size<<endl;
    cout<<"attributeOwnershipAcquisitionNotification "<<attributeOwnershipAcquisitionNotification_q->size<<endl;
    cout<<"attributeOwnershipUnavailable "<<attributeOwnershipUnavailable_q->size<<endl;
    cout<<"timeRegulationEnabled "<<timeRegulationEnabled_q->size<<endl;
    cout<<"timeAdvanceGrant "<<timeAdvanceGrant_q->size<<endl;
}

/////////////////////////////////////////////////////
//
// This converts the RTI::AttributeHandleValuePairSet class into a
// common format which can be passed back and forth to sim_agent
//
// An attribute handle value pair set is a collection of the following
// tuples,
//        (unsigned long HANDLE, char * VALUE, unsigned long VALUESIZE)
//
// This is used to convey updates of attribute values, ie. the HANDLE
// represents the attribute to change and VALUE represents the new
// value
//
/////////////////////////////////////////////////////

void ConvertAHVPset(RTI::AttributeHandleValuePairSet *ahvpset){
  char *value;
  //Get the number of attribute handle pairs in the set
  RTI::ULong count =ahvpset->size();

  //Loop through all the attribute handle pairs
  for(RTI::ULong index =0;index<count;++index){
    //get the handle
    RTI::AttributeHandle h = ahvpset->getHandle(index);
    //get the value length
    RTI::ULong length = ahvpset->getValueLength(index);
    if (!(value = (char *) malloc (length) )){
      /* no memory left, die */
      exit(1);
    }
    //get the value
    ahvpset->getValue(index,value,length);
    free(value);
  }
}
/////////////////////////////////////////////////////
// Same as above except it is just a collection of handles. This is
// used when declaring publish & subscribe interests
/////////////////////////////////////////////////////

void ConvertAHset(RTI::AttributeHandleSet *ahset){

  //Get the number of attribute handle pairs in the set
  RTI::ULong count =ahset->size();

  //Loop through all the attribute handle pairs
  for(RTI::ULong index =0;index<count;++index){
    //get the handle
    RTI::AttributeHandle h = ahset->getHandle(index);
    //get the value length
  }
}

  
/*----------------------------------
//Code for testing linked list implementation
void main()
{
  Queue_t *myQueue;
  makeQueue(myQueue);
  NODE *item1 = new NODE;
  NODE *item2 = new NODE;
  NODE *item3 = new NODE;
  
  NODE *result;

  item1->objectHandle=1;
  item1->classHandle=1;
  item2->objectHandle=2;
  item2->classHandle=2;
  item3->objectHandle=3;
  item3->classHandle=3;
   
  enqueue(myQueue, item1);
  enqueue(myQueue, item2);
  enqueue(myQueue, item3);
  
  for(int x=0;x<3;x++){
    result=dequeue(myQueue);
    printf("%d\n", result->objectHandle);
  }
}
*/
