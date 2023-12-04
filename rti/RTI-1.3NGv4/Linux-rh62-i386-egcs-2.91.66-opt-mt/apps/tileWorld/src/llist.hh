#ifndef llist_h
#define llist_h
//#include <stdlib.h>
//#include <RTI.hh>
#include "globals.hh"




void makeNODE();


extern "C" NODE_t *get_discoverObjectInstance();
extern "C" NODE_t *get_reflectAttributeValues();
extern "C" NODE_t *get_removeObjectInstance();
extern "C" NODE_t *get_provideAttributeValueUpdate();
extern "C" NODE_t *get_attributeOwnershipDivestitureNotification();
extern "C" NODE_t *get_attributeOwnershipAcquisitionNotification();
extern "C" NODE_t *get_attributeOwnershipUnavailable();
extern "C" NODE_t *get_timeRegulationEnabled();
extern "C" NODE_t *get_timeConstrainedEnabled();
extern "C" NODE_t *get_timeAdvanceGrant();
extern "C" void initialiseQueues(void);

void printQueueSizes();
void makeQueue(Queue_t *Q);
void enqueue(Queue_t *Q, NODE *newNode);
NODE_t *dequeue(Queue_t *Q); 

#endif
