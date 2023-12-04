//-----------------------------------------------------------------
// Project Include Files
//-----------------------------------------------------------------
#include "SimFederateAmbassador.hh"

//-----------------------------------------------------------------
//The following include contains all the appropriate storage datatypes
//and functions. It also contains the 'get' functions poplog calls to
//get any callbacks
//-----------------------------------------------------------------
#include "tileWorld.hh"

#include "llist.cpp"
//-----------------------------------------------------------------
// System Include Files
//-----------------------------------------------------------------
#ifndef _MSC_VER
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#else
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#endif

//-----------------------------------------------------------------
// FILE: FedAmbWrapper
//
// This File is used in the SIM_AGENT-HLA interface.  It defines a c++
// federate ambassador class similar to any other.  When a Callback is
// made to the federate ambassador the callback is stored in some c++
// format. This is to avoid direct call backs to poplog. It is up to
// the poplog side to call external C functions ('get' style
// functions) when updates are required. Therefore when the RTI makes
// the FederateAmbassador callbacks all the federate ambassador must
// do is store the appropriate details of the callback in some C data
// structure.
// 
// Created 04/04/02
// Author - Michael Lees (mhl@cs.nott.ac.uk)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// The current 'datatypes' that need to be stored
//
// Each callback has a certain number of parameters it passes, so for
// example a discovery callback (when the RTI informs the appropriate
// federates of a new object instance) passes the Object Handle, the
// Object Name and the class handle (to which the object belongs). As
// each callback may pass different parameters the requirements for
// temporary storage will be different for different types of
// callback(although 2 different callbacks may pass the same
// paramaters).  The following is a list of the different sets of
// parameters that are passed by all the callbacks implemented in this
// file. 
//
//
// 1.-Object Handle - unsigned long,
//    Object Name - string (char*),
//    Class Handle - unsigned long.
//
// 2.-Object Handle - unsigned long
//    AHVPSet - An instance of the class?
//    The time - FedTime object
//    the tag - string (char*)
//    Retraction Handle - unsigned long
//
// 3.-Object Handle - unsigned long
//    the tag - string (char*)
//
// 4.-Object Handle - unsigned long
//    AHSet - An instance of the class?
//
// 5.-The time - FedTime object
//
// The question is whether to make 5 different datatypes one for each
// set of parameters or make one datatype which is the union of all
// possible parameters(any paramters not passed would be NULL or 0).
// The first method saves on memory but implementation is slightly
// messy. The second wastes memory but one generic datatype may be more
// appealing when adding other callbacks and hence parameters.
//
// Another question is then how to store multiple copies of these
// callbacks. An obvious way to store multiple copies would be through
// some implementation of a linked list of the appropiate datatypes.
// The question is whether to have a single linked list containing all
// callbacks or to have a linked list for each type of callback. The
// second option would allow federates to (quickly) request callbacks
// of a particular type.
//
//----------------------------------------------------------------

//----------------------------------------------------------------
//Here we define (globally) various linked lists which store each type
//of callback. It may be the case that at some point one list is used
//for all types of callback rather than a different linked list for
//each type of callback. The basic idea is that the callback methods
//defined in this file will simply add a new item (NODE) to the linked
//list adding the relevant info to the item. For example the callback
//discoverObjectInstance passes the following information
//ObjectHandle, ObjectClassHandle and the objectsName. The code should
//therefore add a new node to the discoverObjectInstance list and
//enter the three fore mentioned values into the item.
//Poplog would then get the callback by calling the relevant get method
//for the particular type of callback.
//
//----------------------------------------------------------------

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

//////////////////////////////
//
// Now we have the get functions, one for each
// linked list. The get functions simply dequeue
// the appropriate list.
//
//////////////////////////////

NODE *get_discoverObjectInstance()
{
  return dequeue(discoverObjectInstance_q);  
}
NODE *get_reflectAttributeValues()
{
  return dequeue(reflectAttributeValues_q);  
}
NODE *get_removeObjectInstance()
{
  return dequeue(removeObjectInstance_q);  
}
NODE *get_provideAttributeValueUpdate()
{
  return dequeue(provideAttributeValueUpdate_q);  
}
NODE *get_attributeOwnershipDivestitureNotification()
{
  return dequeue(attributeOwnershipDivestitureNotification_q);  
}
NODE *get_attributeOwnershipAcquisitionNotification()
{
  return dequeue(attributeOwnershipAcquisitionNotification_q);  
}
NODE *get_attributeOwnershipUnavailable()
{
  return dequeue(attributeOwnershipUnavailable_q);  
}
NODE *get_timeRegulationEnabled()
{
  return dequeue(timeRegulationEnabled_q);  
}
NODE *get_timeConstrainedEnabled()
{
  return dequeue(timeConstrainedEnabled_q);  
}
NODE *get_timeAdvanceGrant()
{
  return dequeue(timeAdvanceGrant_q);  
}


SimFederateAmbassador::SimFederateAmbassador()
{
}

SimFederateAmbassador::~SimFederateAmbassador()
throw(RTI::FederateInternalError)
{
   cerr << "FED_HW: TwFederateAmbassador::~TwFederateAmbassador destructor called in FED" << endl;
}

////////////////////////////////
// Object Management Services //
////////////////////////////////
//
// Methods currently implemented
//
// 6.3 discoverObjectInstance
// 6.5 reflectAttributeValues
// 6.9 removeObjectInstance (*2)
// 6.16 provideAttributeValueUpdate
// 
////////////////////////////////



void SimFederateAmbassador::discoverObjectInstance (
  RTI::ObjectHandle          theObject,      
  RTI::ObjectClassHandle     theObjectClass, 
  const char *          theObjectName)  
throw (
  RTI::CouldNotDiscover,
  RTI::ObjectClassNotKnown,
  RTI::FederateInternalError)
{
  // This function should create a new object of the specified type
  // However this should be done on the poplog side.
  // All the c++ function should do is store the message parameters in some
  // datatype which poplog can access

  //Parameters to be stored-
  //1. Object Handle - unsigned long
  //2. Class Handle - unsigned long
  //3. Object Name - string (char*)

  //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->objectHandle=theObject;
  theCallBack->classHandle=theObjectClass;
  theCallBack->name=theObjectName;

  //enqueue this callback onto the list for this type of callback
  enqueue(discoverObjectInstance_q,theCallBack);

}

void SimFederateAmbassador::reflectAttributeValues (
        RTI::ObjectHandle                 theObject,     
  const RTI::AttributeHandleValuePairSet& theAttributes, 
  const RTI::FedTime&                     theTime,       
  const char                             *theTag,        
        RTI::EventRetractionHandle        theHandle)     
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateOwnsAttributes,
  RTI::InvalidFederationTime,
  RTI::FederateInternalError)
{
  //Parameters to be stored-
  //1. Object Handle - unsigned long
  //2. AHVPSet - An instance of the class?
  //3. The time - FedTime object
  //4. the tag - string (char*)
  //5. Retraction Handle - unsigned long

  //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback
  theCallBack->objectHandle=theObject;
  theCallBack->AHVPset=theAttributes;
  theCallBack->theTime=theTime;
  theCallBack->tag=theTag;


  // theCallBack->eventRetractionHandle=(unsigned long)theHandle;
  
  //enqueue this callback onto the list for this type of callback
  enqueue(reflectAttributeValues_q,theCallBack);
}

void SimFederateAmbassador::removeObjectInstance (
        RTI::ObjectHandle          theObject, 
  const RTI::FedTime&              theTime,   
  const char                      *theTag,    
        RTI::EventRetractionHandle theHandle) 
throw (
  RTI::ObjectNotKnown,
  RTI::InvalidFederationTime,
  RTI::FederateInternalError)
{
   //-----------------------------------------------------------------
   // Call the other removeObject method since this should probably
   //  be implemented using default parameter values.
   //-----------------------------------------------------------------
   this->removeObjectInstance( theObject, theTag );
}

void SimFederateAmbassador::removeObjectInstance (
        RTI::ObjectHandle          theObject, 
  const char                      *theTag)   
throw (
  RTI::ObjectNotKnown,
  RTI::FederateInternalError)
{
  //Parameters to be stored-
  //1. Object Handle - unsigned long
  //2. the tag - string (char*)

  //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->objectHandle=theObject;
  theCallBack->tag=theTag;

  //enqueue this callback onto the list for this type of callback
  enqueue(removeObjectInstance_q,theCallBack);
}


void SimFederateAmbassador::provideAttributeValueUpdate (
        RTI::ObjectHandle        theObject,     
  const RTI::AttributeHandleSet& theAttributes)
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError)
{
  //Parameters to be stored-
  //1. Object Handle - unsigned long
  //2. AHSet - An instance of the class?  

  //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->objectHandle=theObject;
  theCallBack->theAttributes=theAttributes;

  //enqueue this callback onto the list for this type of callback
  enqueue(provideAttributeValueUpdate_q,theCallBack);

 
}


////////////////////////////////
// Ownership Management       //
////////////////////////////////
//
// Methods currently implemented
//
// 7.5 attributeOwnershipDivestitureNotification
// 7.6 attributeOwnershipAcquisitionNotification
// 7.9 attributeOwnershipUnavailable
//
/////////////////////////////////


void SimFederateAmbassador::attributeOwnershipDivestitureNotification(
        RTI::ObjectHandle        theObject,     
  const RTI::AttributeHandleSet& theAttributes) 
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeNotOwned,
  RTI::AttributeDivestitureWasNotRequested,
  RTI::FederateInternalError
  )
{
  //Parameters to be stored-
  //1. Object Handle - unsigned long
  //2. AHSet - An instance of the class?  
  
  //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->objectHandle=theObject;
  theCallBack->theAttributes=theAttributes;
  
  //enqueue this callback onto the list for this type of callback
  enqueue(attributeOwnershipDivestitureNotification_q,theCallBack);
}


void SimFederateAmbassador::attributeOwnershipAcquisitionNotification(
	RTI::ObjectHandle        theObject,   
	const RTI::AttributeHandleSet& theAttributes) 
throw (								     
 RTI::ObjectNotKnown,
 RTI::AttributeNotKnown,
 RTI::AttributeAcquisitionWasNotRequested,
 RTI::AttributeAlreadyOwned,
 RTI::AttributeNotPublished,
 RTI::FederateInternalError
 )
{
  //Parameters to be stored-
  //1. Object Handle - unsigned long
  //2. AHSet - An instance of the class?  

  
    //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->objectHandle=theObject;
  theCallBack->theAttributes=theAttributes;

  //enqueue this callback onto the list for this type of callback
  enqueue(attributeOwnershipAcquisitionNotification_q,theCallBack);
}


void SimFederateAmbassador::attributeOwnershipUnavailable(
	RTI::ObjectHandle        theObject,    
	const RTI::AttributeHandleSet& theAttributes) 
throw (								     
 RTI::ObjectNotKnown,
 RTI::AttributeNotKnown,
 RTI::AttributeAlreadyOwned,
 RTI::AttributeAcquisitionWasNotRequested,
 RTI::FederateInternalError
 )
{
  //Parameters to be stored-
  //1. Object Handle - unsigned long
  //2. AHSet - An instance of the class?  

     //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->objectHandle=theObject;
  theCallBack->theAttributes=theAttributes;

  //enqueue this callback onto the list for this type of callback
  enqueue(attributeOwnershipUnavailable_q,theCallBack);
}

////////////////////////////////
// Time Management            //
////////////////////////////////
//
// Methods currently implemented
// 
// 8.3 timeRegulationEnabled 
// 8.6 timeConstrainedEnabled
// 8.13 timeAdvanceGrant
//
/////////////////////////////////


void SimFederateAmbassador::timeRegulationEnabled (
 const  RTI::FedTime& theFederateTime) 
throw (
  RTI::InvalidFederationTime,
  RTI::EnableTimeRegulationWasNotPending,
  RTI::FederateInternalError)
{
  //Parameters to be stored-
  //1. The time - FedTime object
 
  //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->theTime=theFederateTime;

  //enqueue this callback onto the list for this type of callback
  enqueue(timeRegulationEnabled_q,theCallBack);
}

void SimFederateAmbassador::timeConstrainedEnabled (
  const RTI::FedTime& theFederateTime) 
throw (
  RTI::InvalidFederationTime,
  RTI::EnableTimeConstrainedWasNotPending,
  RTI::FederateInternalError)
{
  //Parameters to be stored-
  //1. The time - FedTime object

    //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->theTime=theFederateTime;

  //enqueue this callback onto the list for this type of callback
  enqueue(timeConstrainedEnabled_q,theCallBack);
}

void SimFederateAmbassador::timeAdvanceGrant (
  const RTI::FedTime& theTime) 
throw (
  RTI::InvalidFederationTime,
  RTI::TimeAdvanceWasNotInProgress,
  RTI::FederateInternalError)
{
 //Parameters to be stored-
  //1. The time - FedTime object

    //Allocate space for callback storage
  NODE *theCallBack = new NODE;
  
  //Store values of this callback 
  theCallBack->theTime=theTime;

  //enqueue this callback onto the list for this type of callback
  enqueue(timeAdvanceGrant_q,theCallBack);
}
