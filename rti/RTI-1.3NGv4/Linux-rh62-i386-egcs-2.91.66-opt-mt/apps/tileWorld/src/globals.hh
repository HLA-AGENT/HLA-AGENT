#ifndef globals_h
#include <stdio.h>
#include <stdlib.h>

#define globals_h

extern int num_attributes;


/*

 The following datatypes are a C version/replacement for the classes
 RTI::AttributeHandleValuePairSet and
 RTI::AttributeHandleSet. They're used for subscription/pulication,
 ownership management and attribute updating.
*/

/*
 An Attribute handle pair isn't actually a pair, it stores the
 Attribute Id the value of the attribute (in Byte form) and the
 length of the value (in bytes)
*/
typedef struct AttributeHandleValueP
{
  unsigned long Id;
  char *value;
  unsigned long length;
}AttributeHandleValuePair;
/*
 This is the replacment for RTI::AttributeHandleValuePairSet it
 stores an array of the above type and the number of items in the array
*/
typedef struct AttributeHandleValuePairSet
{
  AttributeHandleValuePair *ahvp;
  unsigned long size;
}AttributeHandleValuePairSet_t;
/*
 This is the replacment for RTI::AttributeHandleSet it stores an
 array of attribute Ids/Handles (unsigned long) and the number of
 items in the array
*/
typedef struct AttributeHandleSet
{
  unsigned long *attributes;
  unsigned long size;
}AttributeHandleSet_t;

/*
This is the NODE of the linked list used to store callbacks from the
RTI. This datatype is used in SimFederateAmbassador.cpp
*/

typedef struct NODE
{
  struct NODE *Next; /*The next NODE in the linked list*/
  unsigned long objectHandle;
  unsigned long classHandle;
  unsigned long eventRetractionHandle;
  AttributeHandleValuePairSet_t* AHVPset; /*pointer to  type defined above*/
  double theTime;
  AttributeHandleSet_t *theAttributes; /*pointer to above type*/
  const char* tag;
  const char* name;  
}NODE_t;

typedef struct Queue{
  struct NODE *head; 
  struct NODE *tail;
  int size;
}Queue_t;

/*
// These are the queues in which the callbacks are stored. One for
// each type of callback
*/
extern Queue_t *discoverObjectInstance_q;
extern Queue_t *reflectAttributeValues_q;
extern Queue_t *removeObjectInstance_q;
extern Queue_t *provideAttributeValueUpdate_q;
extern Queue_t *attributeOwnershipDivestitureNotification_q;
extern Queue_t *attributeOwnershipAcquisitionNotification_q;
extern Queue_t *attributeOwnershipUnavailable_q;
extern Queue_t *timeRegulationEnabled_q;
extern Queue_t *timeConstrainedEnabled_q;
extern Queue_t *timeAdvanceGrant_q;


#endif



