#ifndef RTIAmbWrapper_h
#define RTIAmbWrapper_h
#endif

#include <RTI.hh>
#include "globals.hh"
// Structure used to pass the attribute handle pairs between poplog and C
//RTI::RTIambassador       rtiAmb;
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

extern "C" int foo(int x);

extern "C" void createFederationExecution(const char* executionName, const char* FED);
extern "C" void destroyFederationExecution(const char* executionName);
extern "C" unsigned long  joinFederationExecution( const char *yourName,
					const char *executionName,
					unsigned  *Ref);
extern "C" void resignFederationExecution();

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

extern "C" void publishObjectClass(RTI::ObjectClassHandle classHandle, 
			AttributeHandleSet_t *attributeArray);


extern "C" void subscribeObjectClassAttributes(RTI::ObjectClassHandle classHandle, 
				    AttributeHandleSet_t *attributeArray);


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

extern "C" unsigned long registerObjectInstance(RTI::ObjectClassHandle classHandle);

extern "C" void updateAttributeValues(RTI::ObjectHandle instanceId,
			   AttributeHandleValuePairSet_t *ahvpset,
			   unsigned long theTime);

extern "C" void deleteObjectInstance(RTI::ObjectHandle instanceId, unsigned long theTime);
  
extern "C" void requestClassAttributeValueUpdate(RTI::ObjectClassHandle classHandle,
				       AttributeHandleSet_t *attributeArray);


extern "C" void requestObjectAttributeValueUpdate(RTI::ObjectHandle instanceId,
				       AttributeHandleSet_t *attributeArray);

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

extern "C" void unconditionalAttributeOwnershipDivesture(RTI::ObjectHandle instanceId,
					      AttributeHandleSet_t  *attributeArray);

extern "C" void attributeOwnershipAcquisitionIfAvailable(RTI::ObjectHandle instanceId,
					      AttributeHandleSet_t  *attributeArray);


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


extern "C" void enableTimeRegulation(unsigned long theFederateTime,
			  unsigned long theLookahead);
extern "C" void timeAdvanceRequest(unsigned long theTime);

extern "C" void enableTimeConstrained();


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

extern "C" RTI::ObjectClassHandle getObjectClassHandle(const char *theName);

extern "C" char* getObjectClassName(RTI::ObjectClassHandle instanceId);
extern "C" RTI::AttributeHandle getAttributeHandle(const char *theName,
					RTI::ObjectClassHandle fromClass);

extern "C" char* getAttributeName(RTI::ObjectClassHandle AttributeId,
		       RTI::ObjectClassHandle fromClass);

extern "C" void tick(double minimum, double maximum);
