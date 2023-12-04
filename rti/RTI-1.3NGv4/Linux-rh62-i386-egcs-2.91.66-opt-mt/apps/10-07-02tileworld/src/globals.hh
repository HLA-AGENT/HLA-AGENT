#ifndef globals_h
#include <fedtime.hh>
#define globals_h
extern RTI::RTIambassador       rtiAmb;
extern int num_attributes;
extern RTI::Boolean        timeAdvGrant;
//The NODE type used in the linked list queue of callbacks
struct AttributeHandleValuePair
{
  unsigned long Id;
  char *value;
  unsigned long length;
};

typedef struct NODE{
  NODE *Next; //The next NODE in the linked list
  unsigned long objectHandle;
  unsigned long classHandle;
  unsigned long eventRetractionHandle;
  AttributeHandleValuePair* AHVPset; //Array of type defined above
  RTI::Double theTime;
  unsigned long *theAttributes; //Array of unsigned long(AttributeIds)
  const char* tag;
  const char* name;  
}NODE_t;

typedef struct Queue{
  NODE_t *head; 
  NODE_t *tail;
  int size;
}Queue_t;



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



