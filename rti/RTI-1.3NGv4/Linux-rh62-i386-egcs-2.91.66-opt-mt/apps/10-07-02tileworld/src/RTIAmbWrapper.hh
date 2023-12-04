#ifndef RTIAmbWrapper_h
#define RTIAmbWrapper_h
#endif

#include <RTI.hh>
#include "globals.hh"
// Structure used to pass the attribute handle pairs between poplog and C

/*
struct AttributeHandleValuePairSet
{
  int num_ahvp;  //number of pairs in the array
  AttributeHandleValuePair *hvpset; //the array of pairs
}

struct AttributeHandleSet
{
  int num_handles;    //number of elements in the array
  unsigned long *handleSet; //The array of handles
}
*/
/*
   Normally the handleset would be a C++ class but due to the 
   fact that poplog cannot handle C++ classes the list of 
   handles is passed as NULL terminated array.
*/
//int sizeOfHandleSet(unsigned long *attributeArray);
//int sizeOfHandleValuePairSet(AttributeHandleValuePair *ahvpset);


////////////////////////////////////
// Federation Management Services //
////////////////////////////////////
//
// Methods currently wrapped;
//
// 4.2 createFederationExecution
// 4.3 destroyFederationExecution
// 4.4 joinFederationExecution
// 4.5 resignFederationExecution
//
////////////////////////////////////

void createFederationExecution(const char* executionName, const char* FED);
void destroyFederationExecution(const char* executionName);
unsigned long  joinFederationExecution( const char *yourName,
					const char *executionName,
					RTI::FederateAmbassadorPtr fedAmbRef);
void resignFederationExecution();

/////////////////////////////////////
// Declaration Management Services //
/////////////////////////////////////
//
// Methods currently wrapped;
//
// 5.2 publishObjectClass
// 5.6 subscribeObjectClassAttributes
//
/////////////////////////////////////

void publishObjectClass(RTI::ObjectClassHandle classHandle, 
			unsigned long *attributeArray);


void subscribeObjectClassAttributes(RTI::ObjectClassHandle classHandle, 
				    unsigned long *attributeArray);


////////////////////////////////
// Object Management Services //
////////////////////////////////
//
// Methods currently wrapped;
//
// 6.2 registerObjectInstance
// 6.4 updateAttributeValues
// 6.8 deleteObjectInstance
// 6.15 -requestClassAttributeValueUpdate
//      -requestObjectAttributeValueUpdate
//
/////////////////////////////////////

unsigned long registerObjectInstance(RTI::ObjectClassHandle classHandle);

void updateAttributeValues(RTI::ObjectHandle instanceId,
			   AttributeHandleValuePair *ahvpset,
			   RTI::FedTime& theTime);

void deleteObjectInstance(RTI::ObjectHandle instanceId,
			  RTI::FedTime& theTime);
  
void requestClassAttributeValueUpdate(RTI::ObjectClassHandle classHandle,
				      unsigned long *attributeArray);


void requestObjectAttributeValueUpdate(RTI::ObjectHandle instanceId,
				       unsigned long *attributeArray);

///////////////////////////////////
// Ownership Management Services //
///////////////////////////////////
//
// Methods currently wrapped;
//
// 7.2 unconditionalAttributeOwnershipDivesture
// 7.8 attributeOwnershipAcquisitionIfAvailable
// 
///////////////////////////////////

void unconditionalAttributeOwnershipDivesture(RTI::ObjectHandle instanceId,
					      unsigned long *attributeArray);

void attributeOwnershipAcquisitionIfAvailable(RTI::ObjectHandle instanceId,
					      unsigned long *attributeArray);


//////////////////////////////
// Time Management Services //
//////////////////////////////
//
// Methods currently wrapped;
//
// 8.2 enableTimeRegulation
// 8.5 EnableTimeConstrained
// 8.8 timeAdvanceRequest
//
//////////////////////////////


void enableTimeRegulation(RTI::FedTime& theFederateTime,
			  RTI::FedTime& theLookahead);
void timeAdvanceRequest(RTI::FedTime& theTime);

void enableTimeConstrained();


//////////////////////////////
//  RTI Support Services    //
//////////////////////////////
//
// Methods currently wrapped...
//
// 10.2 getObjectClassHandle
// 10.3 getObjectClassName
// 10.4 getAttributeHandle
// 10.5 getAttributeName
// **.** RTI::tick
//
// May need to implement...
//
// 10.10 getObjectInstanceHandle
// 10.11 getObjectInstanceName
// 10.17 getObjectClass
//
//////////////////////////////

RTI::ObjectClassHandle getObjectClassHandle(const char *theName);

char* getObjectClassName(RTI::ObjectClassHandle instanceId);
RTI::AttributeHandle getAttributeHandle(const char *theName,
					RTI::ObjectClassHandle fromClass);

char* getAttributeName(RTI::ObjectClassHandle AttributeId,
		       RTI::ObjectClassHandle fromClass);

