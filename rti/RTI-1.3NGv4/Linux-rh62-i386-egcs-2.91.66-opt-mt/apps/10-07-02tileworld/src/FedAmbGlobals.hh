#ifndef FEDAMBGLOBALS_INCLUDED
#define FEDAMBGLOBALS_INCLUDED
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


void initialiseQueues();

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

void makeQueue(Queue_t *Q);
void enqueue(Queue_t *Q, NODE *newNode);
NODE *dequeue(Queue_t *Q);

#endif
