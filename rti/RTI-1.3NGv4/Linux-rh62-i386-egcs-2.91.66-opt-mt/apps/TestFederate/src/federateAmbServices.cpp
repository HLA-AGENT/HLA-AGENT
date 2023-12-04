#if !defined(WIN32) && !defined(VXWORKS) && !defined(__KCC) && (__SUNPRO_CC_COMPAT != 5)
#  include <stream.h>
#  include <strstream.h>
#else
#  include <iostream>
#  include <strstream>
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::ostrstream;
#endif
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <RTI.hh>
#include <datatypes.h>
#include <fedAmbImpl.hh>
#include <fedtime.hh>

// Last serial number used is 15028

extern void outputMsg (char* msg);
extern void  updateStatus(void);
extern int   countListElements (const char* const listStr);
extern unsigned short convertShortID (const char* const s);
extern RTI::RTIambassador myRTIamb;

extern RTI::FederateHandle myHandle;
extern char federateHandle[6];

////////////////////////////////////
// Federation Management Services //
////////////////////////////////////

// 2.?
void FedAmbImpl::synchronizationPointRegistrationSucceeded (
   const char *label) // supplied C4
throw (
  RTI::FederateInternalError)
{
  char message[100];

  try
  {
    sprintf (message, "Synchronization Point Registration Succeeded:  %.*s", 
             50, label);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15027, "Unexpected exception");
  }
}

void FedAmbImpl::synchronizationPointRegistrationFailed (
   const char *label) // supplied C4
throw (
  RTI::FederateInternalError)
{
  char message[100];

  try
  {
    sprintf (message, "Synchronization Point Registration Failed:  %.*s", 
             50, label);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15027, "Unexpected exception");
  }
}

// 2.6
void FedAmbImpl::announceSynchronizationPoint (
  const char *label, // supplied C4
  const char *tag) 
throw (
  RTI::FederateInternalError)
{
   char message[100];

  try
  {
    sprintf (message, "Announce Synchronization Point:  %.*s", 50, label);
    outputMsg (message);
    sprintf (message, "  Tag:  %.*s", 50, tag);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15027, "Unexpected exception");
  }
}

// 2.8
void FedAmbImpl::federationSynchronized (
   const char *label) // supplied C4
throw (
  RTI::FederateInternalError)
{
  char message[100];

  try
  {
    sprintf (message, "Federation Synchronized:  %.*s", 50, label);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15027, "Unexpected exception");
  }
}

// 2.10
void FedAmbImpl::initiateFederateSave (
  const char *label) // supplied C4
throw (
  RTI::UnableToPerformSave,
  RTI::FederateInternalError)
{
  char message[100];
  
  try
  {
    sprintf (message, "Initiate Federate Save Request:  %.*s", 50, label);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

// 2.13
void FedAmbImpl::federationSaved ()
throw (
  RTI::FederateInternalError)
{
  try
  {
    outputMsg ("Federation Saved");
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

void FedAmbImpl::federationNotSaved ()
throw (
  RTI::FederateInternalError)
{
  try
  {
    outputMsg ("Federation Not Saved");
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

// 2.?
void FedAmbImpl::requestFederationRestoreSucceeded (
  const char *label) // supplied C4
throw (
  RTI::FederateInternalError)
{
  char message[100];
  
  try
  {
    sprintf (message, "Request Federation Restore Succeeded: Label :%.*s", 50,label);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

void FedAmbImpl::requestFederationRestoreFailed (
    const char *label,  // supplied C4
    const char *reason) // supplied C4
  throw (
    RTI::FederateInternalError)
{
  char message[100];
  
  try
  {
    sprintf (message, "Request Federation Restore Failed: Label :%.*s, Reason :%.*s", 50,label,50,reason);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

// 2.15
void FedAmbImpl::federationRestoreBegun ()
    throw (
      RTI::FederateInternalError)
{
 try
  {
    outputMsg ("Federation Restore Begun");
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

// 2.16
void FedAmbImpl::initiateFederateRestore (
  const char *label,
  RTI::FederateHandle handle)// supplied C4
throw (
  RTI::SpecifiedSaveLabelDoesNotExist,
  RTI::CouldNotRestore,
  RTI::FederateInternalError)
{
  char message[100];
  
  try
  {
    sprintf (message, "Initiate Restore:  Label :%.*s, FedHandle: %i", 50, label,handle);
    outputMsg (message);
    myHandle = handle;
    sprintf (federateHandle, "%i", myHandle);
    updateStatus();
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

// 2.18
void FedAmbImpl::federationRestored ()
throw (
  RTI::FederateInternalError)
{
  try
  {
    outputMsg ("Federation Restored");
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

void FedAmbImpl::federationNotRestored ()
throw (
  RTI::FederateInternalError)
{
  try
  {
    outputMsg ("Federation Not Restored");
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

/////////////////////////////////////
// Declaration Management Services //
/////////////////////////////////////

// 3.9
void FedAmbImpl::startRegistrationForObjectClass (
        RTI::ObjectClassHandle   theObjectClass)  // supplied C1
throw (
  RTI::ObjectClassNotPublished,
  RTI::FederateInternalError)
{
  char                  handleStr[6];
  char                  buf[512];
  ostrstream            myStream (buf, 512);
  char                 *objClassName;
  RTI::ULong            attrCnt;
  RTI::Boolean          first;

  try
  {
    objClassName = myRTIamb.getObjectClassName (theObjectClass);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    objClassName = new char[1];
    objClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    objClassName = new char[1];
    objClassName[0] = '\0';
  }

  try
  {
    sprintf (buf,
             "Start Registration received for Object Class: %s (%i)",
             objClassName, theObjectClass);
    outputMsg (buf);
    delete [] objClassName;
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15000, "Unexpected exception");
  }
}

// 3.10
void FedAmbImpl::stopRegistrationForObjectClass (
        RTI::ObjectClassHandle   theObjectClass) // supplied C1
throw (
  RTI::ObjectClassNotPublished,
  RTI::FederateInternalError)
{
  char                  handleStr[6];
  char                  buf[512];
  ostrstream            myStream (buf, 512);
  char                 *objClassName;
  RTI::ULong            attrCnt;
  RTI::Boolean          first;

  try
  {
    objClassName = myRTIamb.getObjectClassName (theObjectClass);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    objClassName = new char[1];
    objClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    objClassName = new char[1];
    objClassName[0] = '\0';
  }

  try
  {
    sprintf (buf,
             "Stop Registration Received for Object Class: %s (%i)",
             objClassName, theObjectClass);
    outputMsg (buf);
    delete [] objClassName;
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15001, "Unexpected exception");
  }
}

// 3.11
void FedAmbImpl::turnInteractionsOn (
  RTI::InteractionClassHandle theHandle) // supplied C1
throw (
  RTI::InteractionClassNotPublished,
  RTI::FederateInternalError)
{
  char                       buf[512];
  ostrstream                 myStream (buf, 512);
  char                      *interactionClassName;

  try
  {
    interactionClassName = myRTIamb.getInteractionClassName (theHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }

  try
  {
    sprintf (buf,
 "Turn Interactions On received for Interaction Class: %s (%i)",
        interactionClassName, theHandle);
    outputMsg (buf);
    delete [] interactionClassName;
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15002, "Unexpected exception");
  }
}

// 3.12
void FedAmbImpl::turnInteractionsOff (
  RTI::InteractionClassHandle theHandle) // supplied C1
throw (
  RTI::InteractionClassNotPublished,
  RTI::FederateInternalError)
{
  char                       buf[512];
  ostrstream                 myStream (buf, 512);
  char                      *interactionClassName;

  try
  {
    interactionClassName = myRTIamb.getInteractionClassName (theHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }

  try
  {
    sprintf (buf,
      "Turn Interactions Off received for Interaction Class: %s (%i)",
      interactionClassName, theHandle);
    outputMsg (buf);
    delete [] interactionClassName;
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15003, "Unexpected exception");
  }
}

// 3.13
void FedAmbImpl::turnUpdatesOnForObjectInstance (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError)
{
  char                  handleStr[6];
  char                  buf[512];
  ostrstream            myStream (buf, 512);
  char                 *objName;
  RTI::ULong            attrCnt;
  RTI::Boolean          first;

#if 0
  try
  {
    objName = myRTIamb.getObjectName (theObject);
  }
  catch (Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    objName = new char[1];
    objName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    objName = new char[1];
    objName[0] = '\0';
  }
#endif
  
  try
  {
    sprintf (buf,
             "Turn Updates On received for Object Instance: %i",theObject);
    outputMsg (buf);
    //delete [] objName;
    
    strcpy (buf, "  Attributes:  ");
    attrCnt = theAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", theAttributes.getHandle (i));
      
      if (first)
        first = RTI::RTI_FALSE;
      else
        strcat (buf, ",");
      
      strcat (buf, handleStr);
    }
    
    outputMsg (buf);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15000, "Unexpected exception");
  }
}

// 3.14
void FedAmbImpl::turnUpdatesOffForObjectInstance (
        RTI::ObjectHandle        theObject,      // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError)
{
  char                  handleStr[6];
  char                  buf[512];
  ostrstream            myStream (buf, 512);
  char                 *objName;
  RTI::ULong            attrCnt;
  RTI::Boolean          first;

#if 0  
  try
  {
    objName = myRTIamb.getObjectName (theObject);
  }
  catch (Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    objName = new char[1];
    objName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    objName = new char[1];
    objName[0] = '\0';
  }
#endif
  
  try
  {
    sprintf (buf,
             "Turn Updates Off received for Object Instance: %i",theObject);
    outputMsg (buf);
    //delete [] objName;
    
    strcpy (buf, "  Attributes:  ");
    attrCnt = theAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", theAttributes.getHandle (i));
      
      if (first)
        first = RTI::RTI_FALSE;
      else
        strcat (buf, ",");
      
      strcat (buf, handleStr);
    }
    
    outputMsg (buf);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15001, "Unexpected exception");
  }
}

////////////////////////////////
// Object Management Services //
////////////////////////////////

// 4.4
void FedAmbImpl::discoverObjectInstance (
  RTI::ObjectHandle          theObject,      // supplied C1
  RTI::ObjectClassHandle     theObjectClass, // supplied C1
  char const *          theObjectName)
throw (
  RTI::CouldNotDiscover,
  RTI::ObjectClassNotKnown,
  RTI::FederateInternalError)
{
  char                  buf[512];
  ostrstream            myStream (buf, 512);
  char                 *objClassName;

  try
  {
    objClassName = myRTIamb.getObjectClassName (theObjectClass);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    objClassName = new char[1];
    objClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    objClassName = new char[1];
    objClassName[0] = '\0';
  }

  try
  {
    sprintf (buf,
             "Discovered Object %i of Class: %s (%i)\n",
             theObject, objClassName, theObjectClass);
    outputMsg (buf);
    delete [] objClassName;
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15004, "Unexpected exception");
  }
}

// 4.5
void FedAmbImpl::reflectAttributeValues (
        RTI::ObjectHandle                 theObject,     // supplied C1
  const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
  const RTI::FedTime&                     theTime,       // supplied C1
  const char                        *theTag,        // supplied C4
        RTI::EventRetractionHandle        theHandle)     // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateOwnsAttributes,
  RTI::InvalidFederationTime,
  RTI::FederateInternalError)
{
  RTI::ULong attrCnt;
  RTI::ULong valueLength;
  char       *attrValue;
  char       message[500];
  char       hxString[500];
  ostrstream myStream (message, 500);
  
  try
  {
    sprintf (message, "Reflect Attribute Values for Object Handle: %i", theObject);
    outputMsg (message);
    myStream << "  Federation Time: " << theTime << '\0';
    outputMsg (message);
    sprintf(message, "  User Supplied Tag: %s",theTag);
    outputMsg (message);

    attrCnt = theAttributes.size ();    
    for (int i = 0; i < attrCnt; i++)
    {
      valueLength = theAttributes.getValueLength(i);
      attrValue = new char[valueLength];
      theAttributes.getValue (i, attrValue, valueLength);

      // Print the handle value pair
      printHandleValuePair(attrValue, theAttributes.getHandle(i), valueLength);
      
      delete [] attrValue;
    }
    
    sprintf (message,
             "  Event Retraction Handle:"
             "  Serial Number:  %i  Federate Handle:  %i",
             theHandle.theSerialNumber,
             theHandle.sendingFederate);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15005, "Unexpected exception");
  }
}

// 4.5
void FedAmbImpl::reflectAttributeValues (
        RTI::ObjectHandle                 theObject,     // supplied C1
  const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
  const char                        *theTag)        // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateOwnsAttributes,
  RTI::FederateInternalError)
{
  RTI::ULong attrCnt;
  RTI::ULong valueLength;
  char       *attrValue;
  char       message[250];
  char       hxString[500];


  try
  {
    sprintf (message,
             "Reflect Attribute Values for Object Handle: %i\n"
             "User Supplied Tag:  %s",
             theObject, theTag);
    outputMsg (message);
    
    attrCnt = theAttributes.size ();
    for (int i = 0; i < attrCnt; i++)
    {
      valueLength = theAttributes.getValueLength(i);
      attrValue = new char[valueLength];
      theAttributes.getValue (i, attrValue, valueLength);

      // Print the handle value pair
      printHandleValuePair(attrValue, theAttributes.getHandle(i), valueLength);      
      delete [] attrValue;
    }
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15005, "Unexpected exception");
  }
}


// 4.7
void FedAmbImpl::receiveInteraction (
        RTI::InteractionClassHandle       theInteraction, // supplied C1
  const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
  const RTI::FedTime&                     theTime,        // supplied C1
  const char                        *theTag,         // supplied C4
        RTI::EventRetractionHandle        theHandle)      // supplied C1
throw (
  RTI::InteractionClassNotKnown,
  RTI::InteractionParameterNotKnown,
  RTI::InvalidFederationTime,
  RTI::FederateInternalError)
{
  RTI::ULong                 parmCnt;
  RTI::ULong                 valueLength;
  float                      responderOverheadTime;
  char                       *parmValue;
  char                       buf[512];
  char                       hxString[500];
  ostrstream                 myStream (buf, 512);
  char                      *interactionClassName;
  char                      *parmName = NULL;

  try
  {
    interactionClassName = myRTIamb.getInteractionClassName (theInteraction);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }

  try
  {
    sprintf (buf, "Received Interaction for: %s (%i)",
             interactionClassName, theInteraction);
    outputMsg (buf);
    myStream << "  Federation Time: " << theTime << '\0';
    outputMsg (buf);
    sprintf(buf, "  User Supplied Tag: %s",theTag);
    outputMsg (buf);
    delete [] interactionClassName;

    parmCnt = theParameters.size ();
    for (int i = 0; i < parmCnt; i++)
    {
      try
      {
        parmName = myRTIamb.getParameterName (theParameters.getHandle (i),
                                              theInteraction);
        valueLength = theParameters.getValueLength(i);
        parmValue = new char[valueLength];
        theParameters.getValue (i, parmValue, valueLength);

        // Print the handle value pair
        printHandleValuePair(parmValue, theParameters.getHandle(i), valueLength);

        delete [] parmValue;
        
      }
      catch (RTI::Exception& ex)
      {
        myStream << "Error:  " << &ex << '\0';
        outputMsg (buf);
      }
      catch (...)
      {
        outputMsg ("Caught unknown exception");
      }
      
      if (parmName != NULL)
      {
        delete [] parmName;
        parmName = NULL;
      }
    }
    
    sprintf (buf,
             "  Event Retraction Handle: "
             "  Serial Number:  %i  Federate Handle:  %i",
             theHandle.theSerialNumber,
             theHandle.sendingFederate);
    outputMsg (buf);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15006, "Unexpected exception");
  }
}

// 4.7
void FedAmbImpl::receiveInteraction (
        RTI::InteractionClassHandle       theInteraction, // supplied C1
  const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
  const char                        *theTag)         // supplied C4
throw (
  RTI::InteractionClassNotKnown,
  RTI::InteractionParameterNotKnown,
  RTI::FederateInternalError)
{
  RTI::ULong                 parmCnt;
  RTI::ULong                 valueLength;
  float                      responderOverheadTime;
  char                       *parmValue;
  char                       buf[512];
  char                       hxString[5];
  ostrstream                 myStream (buf, 512);
  char                      *interactionClassName;
  char                      *parmName = NULL;
  
  try
  {
    interactionClassName = myRTIamb.getInteractionClassName (theInteraction);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }

  try
  {
    sprintf (buf,
             "Received Interaction for Interaction Class: %s (%i)\n"
             " User Supplied Tag:  %s",
             interactionClassName, theInteraction, theTag);
    outputMsg (buf);
    delete [] interactionClassName;

    
    parmCnt = theParameters.size ();
    for (int i = 0; i < parmCnt; i++)
    {
      try
      {
        parmName = myRTIamb.getParameterName (theParameters.getHandle (i),
                                              theInteraction);
        valueLength = theParameters.getValueLength(i);
        parmValue = new char[valueLength];
        theParameters.getValue (i, parmValue, valueLength);
        
        printHandleValuePair(parmValue, theParameters.getHandle(i), valueLength);

        // Delete parmValue
        delete [] parmValue;
        
      }
      catch (RTI::Exception& ex)
      {
        myStream << "Error:  " << &ex << '\0';
        outputMsg (buf);
      }
      catch (...)
      {
        outputMsg ("Caught unknown exception");
      }
      
      if (parmName != NULL)
      {
        delete [] parmName;
        parmName = NULL;
      }
    }
    
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15006, "Unexpected exception");
  }
}

// 4.9
void FedAmbImpl::removeObjectInstance (
        RTI::ObjectHandle          theObject, // supplied C1
  const RTI::FedTime&              theTime,   // supplied C1
  const char                 *theTag,    // supplied C4
        RTI::EventRetractionHandle theHandle) // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::InvalidFederationTime,
  RTI::FederateInternalError)
{
  char message[100];
  ostrstream myStream (message, 100);

  try
  {
    sprintf(message, "Received Remove Object Instanct for:%i",theObject);
    outputMsg (message);
    myStream << "  Federation Time: " << theTime << '\0';
    outputMsg (message);
    sprintf(message, "  User Supplied Tag: ", theTag);
    outputMsg (message);
    
    sprintf (message,
      "  Event Retraction Handle: "
      "  Serial Number:  %i  Federate Handle:  %i",
       theHandle.theSerialNumber,
      theHandle.sendingFederate);
    outputMsg (message);
    
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15007, "Unexpected exception");
  }
}

// 4.9
void FedAmbImpl::removeObjectInstance (
        RTI::ObjectHandle          theObject, // supplied C1
  const char                 *theTag)    // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::FederateInternalError)
{
  char message[100];

  try
  {
    sprintf (message,
      "Deleted Object Handle: %i",theObject);
    outputMsg (message);

    sprintf (message, "  User Supplied Tag:  %s", theTag);
    outputMsg (message);
    
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15007, "Unexpected exception");
  }
}

// 4.14
void FedAmbImpl::attributesInScope (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::FederateInternalError)
{
  char                  handleStr[6];
  char                  buf[512];
  ostrstream            myStream (buf, 512);
  char                 *objName;
  RTI::ULong            attrCnt;
  RTI::Boolean          first;

#if 0  
  try
  {
    objName = myRTIamb.getObjectName (theObject);
  }
  catch (Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    objName = new char[1];
    objName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    objName = new char[1];
    objName[0] = '\0';
  }
#endif
  
  try
  {
    sprintf (buf,
             "Attributes In Scope received for Object Instance: %i", theObject);
    outputMsg (buf);
    //delete [] objName;
    
    strcpy (buf, "  Attributes:  ");
    attrCnt = theAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", theAttributes.getHandle (i));
      
      if (first)
        first = RTI::RTI_FALSE;
      else
        strcat (buf, ",");
      
      strcat (buf, handleStr);
    }
    
    outputMsg (buf);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15001, "Unexpected exception");
  }
}

// 4.15
void FedAmbImpl::attributesOutOfScope (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::FederateInternalError)
{
  char                  handleStr[6];
  char                  buf[512];
  ostrstream            myStream (buf, 512);
  char                 *objName;
  RTI::ULong            attrCnt;
  RTI::Boolean          first;

#if 0  
  try
  {
    objName = myRTIamb.getObjectName (theObject);
  }
  catch (Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    objName = new char[1];
    objName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    objName = new char[1];
    objName[0] = '\0';
  }
#endif
  
  try
  {
    sprintf (buf,
             "Attributes Out of Scope received for Object Instance: %i",theObject);
    outputMsg (buf);
    //delete [] objName;
    
    strcpy (buf, "  Attributes:  ");
    attrCnt = theAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", theAttributes.getHandle (i));
      
      if (first)
        first = RTI::RTI_FALSE;
      else
        strcat (buf, ",");
      
      strcat (buf, handleStr);
    }
    
    outputMsg (buf);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15001, "Unexpected exception");
  }
}

// 4.16
void FedAmbImpl::interactionInScope (
  RTI::InteractionClassHandle theHandle)     // supplied C1
  throw (
    RTI::InteractionClassNotKnown,
    RTI::InteractionClassNotSubscribed,
    RTI::FederateInternalError)
{
  char                       buf[512];
  ostrstream                 myStream (buf, 512);
  char                      *interactionClassName;

  try
  {
    interactionClassName = myRTIamb.getInteractionClassName (theHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }

  try
  {
    sprintf (buf,
             "Interaction In Scope received for Interaction Class: %s (%i)",
             interactionClassName, theHandle);
    outputMsg (buf);
    delete [] interactionClassName;
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15003, "Unexpected exception");
  }
}

// 4.17
void FedAmbImpl::interactionOutOfScope (
  RTI::InteractionClassHandle theHandle)     // supplied C1
  throw (
    RTI::InteractionClassNotKnown,
    RTI::InteractionClassNotSubscribed,
    RTI::FederateInternalError)
{
   char                       buf[512];
  ostrstream                 myStream (buf, 512);
  char                       *interactionClassName;

  try
  {
    interactionClassName = myRTIamb.getInteractionClassName (theHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");

    interactionClassName = new char[1];
    interactionClassName[0] = '\0';
  }

  try
  {
    sprintf (buf,
             "Interaction Out of Scope received for Interaction Class: %s (%i)",
             interactionClassName, theHandle);
    outputMsg (buf);
    delete [] interactionClassName;
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15003, "Unexpected exception");
  }
}


// 4.15
void FedAmbImpl::provideAttributeValueUpdate (
        RTI::ObjectHandle            theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
  char         handleStr[6];
  char         message[250];
  RTI::ULong   attrCnt;
  RTI::Boolean first;

  try
  {
    sprintf (message,
             "Provide Attribute Value Update received for Object Handle %i",
             theObject);
    outputMsg (message);
    
    strcpy (message, "  Attributes:  ");
    attrCnt = theAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", theAttributes.getHandle (i));
      
      if (first)
        first = RTI::RTI_FALSE;
      else
        strcat (message, ",");
      
      strcat (message, handleStr);
    }
    
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15009, "Unexpected exception");
  }
}

// 4.17
void FedAmbImpl::requestRetraction (
  RTI::EventRetractionHandle theHandle) // supplied C1
throw (
  RTI::EventNotKnown,
  RTI::FederateInternalError)
{
    char         message[200];
    
    try
    {
        outputMsg ("Request Retraction received");
        
        sprintf (message,
                 "  Event Retraction Handle: "
                 "  Serial Number:  %i  Federate Handle:  %i",
                  theHandle.theSerialNumber,
                 theHandle.sendingFederate);
        outputMsg (message);
    }
    catch (...)
    {
        throw RTI::FederateInternalError (15010, "Unexpected exception");
    }
}

///////////////////////////////////
// Ownership Management Services //
///////////////////////////////////

// 5.3
void FedAmbImpl::requestAttributeOwnershipAssumption (
        RTI::ObjectHandle        theObject,         // supplied C1
  const RTI::AttributeHandleSet& offeredAttributes, // supplied C4
  const char               *theTag)            // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeAlreadyOwned,
  RTI::AttributeNotPublished,
  RTI::FederateInternalError)
{
  RTI::ULong attrCnt;
  RTI::Boolean first;
  char       handleStr[6];
  char       message[250];
  
  try
  {
    sprintf (message,
             "Request Attribute Ownership Assumption Received for Object: %i\n"
             " User Supplied Tag:  %s\n Attributes: ",
             theObject, theTag);
    
    attrCnt = offeredAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", offeredAttributes.getHandle (i));
      
      if (first)
        first = RTI::RTI_FALSE;
      else
        strcat (message, ",");
      
      strcat (message, handleStr);
    }
    outputMsg (message);
    
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15005, "Unexpected exception");
  }
}

// 5.4
void FedAmbImpl::attributeOwnershipDivestitureNotification (
        RTI::ObjectHandle        theObject,          // supplied C1
  const RTI::AttributeHandleSet& releasedAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeNotOwned,
  RTI::AttributeDivestitureWasNotRequested,
  RTI::FederateInternalError)
{
  char         handleStr[6];
  char         message[80];
  RTI::ULong   attrCnt;
  RTI::Boolean first;

  try
  {
    outputMsg ("Attribute Ownership Divestiture Notification received for");

    sprintf (message, "  Object Handle: %i, Attributes:  ", theObject);

    attrCnt = releasedAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", releasedAttributes.getHandle (i));

      if (first)
        first = RTI::RTI_FALSE;
      else
        strcat (message, ",");
      
      strcat (message, handleStr);
    }
    
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15019, "Unexpected exception");
  }
}


// 5.5
void FedAmbImpl::attributeOwnershipAcquisitionNotification (
        RTI::ObjectHandle            theObject,         // supplied C1
  const RTI::AttributeHandleSet& securedAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeAcquisitionWasNotRequested,
  RTI::AttributeAlreadyOwned,
  RTI::AttributeNotPublished,
  RTI::FederateInternalError)
{
  char         handleStr[6];
  char         message[80];
  RTI::ULong   attrCnt;
  RTI::Boolean first;

  try
  {
    outputMsg ("Attribute Ownership Acquisition Notification received for");

    sprintf (message, "Object Handle: %i, Attributes:  ", theObject);

    attrCnt = securedAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", securedAttributes.getHandle (i));

      if (first)
 first = RTI::RTI_FALSE;
      else
 strcat (message, ",");

      strcat (message, handleStr);
    }

    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15020, "Unexpected exception");
  }
}


// 5.8
void FedAmbImpl::attributeOwnershipUnavailable (
        RTI::ObjectHandle        theObject,         // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::AttributeAlreadyOwned,
    RTI::AttributeAcquisitionWasNotRequested,
    RTI::FederateInternalError)
{
  char         handleStr[6];
  char         message[80];
  RTI::ULong   attrCnt;
  RTI::Boolean first;
  
  try
  {
    outputMsg ("Attribute Ownership Unavailable received for");

    sprintf (message, "Object Handle: %i, Attributes:  ", theObject);

    attrCnt = theAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", theAttributes.getHandle (i));

      if (first)
 first = RTI::RTI_FALSE;
      else
 strcat (message, ",");

      strcat (message, handleStr);
    }

    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15020, "Unexpected exception");
  }
}


// 5.9
void FedAmbImpl::requestAttributeOwnershipRelease (
        RTI::ObjectHandle        theObject,           // supplied C1
  const RTI::AttributeHandleSet& candidateAttributes, // supplied C4
  const char               *theTag)              // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError)
{
  char         handleStr[6];
  char         message[80];
  RTI::ULong   attrCnt;
  RTI::Boolean first;
  
  try
  {
    outputMsg ("Request Attribute Ownership Release received for");
    sprintf (message, "Object Handle: %i, Attributes:  ", theObject);

    attrCnt = candidateAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", candidateAttributes.getHandle (i));

      if (first)
 first = RTI::RTI_FALSE;
      else
 strcat (message, ",");

      strcat (message, handleStr);
    }

    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15020, "Unexpected exception");
  }
}

// 5.13
void FedAmbImpl::confirmAttributeOwnershipAcquisitionCancellation (
        RTI::ObjectHandle        theObject,         // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::AttributeAlreadyOwned,
    RTI::AttributeAcquisitionWasNotCanceled,
    RTI::FederateInternalError)
{
  char         handleStr[6];
  char         message[80];
  RTI::ULong   attrCnt;
  RTI::Boolean first;
  
  try
  {
    outputMsg ("Confirm Attribute Ownership Acquisition Cancellation received for");

    sprintf (message, "Object Handle: %i, Attributes:  ", theObject);

    attrCnt = theAttributes.size ();
    first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", theAttributes.getHandle (i));

      if (first)
 first = RTI::RTI_FALSE;
      else
 strcat (message, ",");

      strcat (message, handleStr);
    }

    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15020, "Unexpected exception");
  }
}
  

// 5.15
void FedAmbImpl::informAttributeOwnership (
  RTI::ObjectHandle    theObject,    // supplied C1
  RTI::AttributeHandle theAttribute, // supplied C1
  RTI::FederateHandle  theOwner)     // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
  char message[60];

  try
  {
    sprintf (message,
      "Attribute %i of Object Handle %i is owned by Federate %i",
      theAttribute, theObject, theOwner);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15025, "Unexpected exception");
  }
}

void FedAmbImpl::attributeIsNotOwned (
  RTI::ObjectHandle    theObject,    // supplied C1
  RTI::AttributeHandle theAttribute) // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
  char message[60];

  try
  {
    sprintf (message,
      "Attribute %i of Object Handle %i is not owned by any Federate",
      theAttribute, theObject);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15025, "Unexpected exception");
  }
}

void FedAmbImpl::attributeOwnedByRTI (
  RTI::ObjectHandle    theObject,    // supplied C1
  RTI::AttributeHandle theAttribute) // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
  char message[60];

  try
  {
    sprintf (message,
      "Attribute %i of Object Handle %i is owned by RTI",
      theAttribute, theObject);
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15025, "Unexpected exception");
  }
}

//////////////////////////////
//////////////////////////////
// Time Management Services //
//////////////////////////////
// 6.2
void FedAmbImpl::timeRegulationEnabled (
  const RTI::FedTime& theFederateTime) // supplied C1
  throw (
    RTI::InvalidFederationTime,
    RTI::EnableTimeRegulationWasNotPending,
    RTI::FederateInternalError)
{
  char message[200];
  ostrstream myStream (message, 200);
  
  try
  {
    myStream << "Time Regulation Enabled Received, Time: " <<
      theFederateTime << '\0';
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

// 6.5
void FedAmbImpl::timeConstrainedEnabled (
  const RTI::FedTime& theFederateTime) // supplied C1
  throw (
    RTI::InvalidFederationTime,
    RTI::EnableTimeConstrainedWasNotPending,
    RTI::FederateInternalError)
{
  char message[200];
  ostrstream myStream (message, 200);

  
  try
  {
    myStream << "Time Constrained Enabled Received, Time: " <<
      theFederateTime << '\0'; 
    outputMsg (message);
  }
  catch (...)
  {
    throw RTI::FederateInternalError (15028, "Unexpected exception");
  }
}

// 6.10
void FedAmbImpl::timeAdvanceGrant (
  const RTI::FedTime& theTime) // supplied C1
  throw (
  RTI::InvalidFederationTime,
  RTI::TimeAdvanceWasNotInProgress,
  RTI::FederationTimeAlreadyPassed,
  RTI::FederateInternalError)
{
  char message[50];
  ostrstream myStream (message, 50);
  
  myStream << "Time advanced granted to: " << theTime << '\0';
  outputMsg (message); 
}

/////////////////////////////////////////
// Formatted Output Utility function   //
////////////////////////////////////////
void FedAmbImpl::printHandleValuePair (
  char        *value,
  RTI::Handle  theHandle,
  RTI::ULong   valueLength)
{
  char                       buf[512];
  char                       hxString[5];
  ostrstream                 myStream (buf, 512);
  int                        isString;
  
  // Determine if data is really string as best as we can.
  isString = 0;
  
  if (value[valueLength - 1] == '\0' )
  {
    isString = 1;
    for (int count = 0; count < valueLength - 1; count++)
    {
      if(value[count] == '\0' || !isprint(value[count]))
      {
        isString = 0;
        break;
      }
    }
  }
  
  if(isString)
  {
    if (valueLength > 60)
      sprintf (buf, "  Handle %i has value: %60s...", theHandle, value);
    else
      sprintf (buf, "  Handle %i has value: %s", theHandle, value);
    
    outputMsg (buf);
  }
  else
  {
  
    // print the HEX value
    sprintf (buf, "  Handle %i has Hex value: 0x", theHandle);
    
    unsigned char hxValue;
    for (int index = 0; index < valueLength; index++)
    {
      hxValue = (unsigned char)*(value + index);
      sprintf(hxString,"%02x",hxValue);
      strcat(buf, hxString);
    }
    outputMsg (buf);
  }
}



