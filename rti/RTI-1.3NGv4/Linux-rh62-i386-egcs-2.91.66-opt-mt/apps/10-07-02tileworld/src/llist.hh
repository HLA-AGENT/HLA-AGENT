#ifndef llist_h
#define llist_h
//#include <stdlib.h>
//#include <RTI.hh>
#include "globals.hh"


void initialiseQueues();
void makeNODE();

NODE *get_discoverObjectInstance();
NODE *get_reflectAttributeValues();
NODE *get_removeObjectInstance();
NODE *get_provideAttributeValueUpdate();
NODE *get_attributeOwnershipDivestitureNotification();
NODE *get_attributeOwnershipAcquisitionNotification();
NODE *get_attributeOwnershipUnavailable();
NODE *get_timeRegulationEnabled();
NODE *get_timeConstrainedEnabled();
NODE *get_timeAdvanceGrant();
void printQueueSizes();
void makeQueue(Queue_t *Q);
void enqueue(Queue_t *Q, NODE *newNode);
NODE *dequeue(Queue_t *Q); 

#endif
