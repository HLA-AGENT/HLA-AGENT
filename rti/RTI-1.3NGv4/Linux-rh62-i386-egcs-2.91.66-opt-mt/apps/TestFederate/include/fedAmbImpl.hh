// File fedAmbImpl.hh
// Derived version of FedAmb for stubs to build test federate.

#ifndef FED_AMB_IMPL_H
#define FED_AMB_IMPL_H

#include "RTI.hh"

//                RTI Parameter Passing Memory Conventions
//
// C1  In parameter by value.
// C2  Out parameter by reference.
// C3  Function return by value.
// C4  In parameter by const reference.  Caller provides memory.
//     Caller may free memory or overwrite it upon completion of
//     the call.  Callee must copy during the call anything it
//     wishes to save beyond completion of the call.  Parameter
//     type must define const accessor methods.
// C5  Out parameter by reference.  Caller provides reference to object.
//     Callee constructs an instance on the heap (new) and returns.
//     The caller destroys the instance (delete) at its leisure.
// C6  Function return by reference.  Callee constructs an instance on
//     the heap (new) and returns a reference.  The caller destroys the
//     instance (delete) at its leisure.

////////////////////////////////////
// Federation Management Services //
////////////////////////////////////

class FedAmbImpl : public RTI::FederateAmbassador
{
public:
  // 2.?
  virtual void synchronizationPointRegistrationSucceeded (
    const char *label)
    throw (
      RTI::FederateInternalError);

  virtual void synchronizationPointRegistrationFailed (
    const char *label)
    throw (
      RTI::FederateInternalError);

  // 2.6
  virtual void announceSynchronizationPoint (
    const char *label, // supplied C4
    const char *tag)
    throw (
      RTI::FederateInternalError);
  
  // 2.8
  virtual void federationSynchronized (
    const char *label)
    throw (
      RTI::FederateInternalError);
  
  // 2.10
  virtual void initiateFederateSave (
    const char *label) // supplied C4
  throw (
    RTI::UnableToPerformSave,
    RTI::FederateInternalError);
  
  // 2.13
  virtual void federationSaved ()
    throw (
      RTI::FederateInternalError);
  
  virtual void federationNotSaved ()
    throw (
      RTI::FederateInternalError);
  
  // 2.?
  virtual void requestFederationRestoreSucceeded (
    const char *label) // supplied C4
  throw (
    RTI::FederateInternalError);

  virtual void requestFederationRestoreFailed (
    const char *label,  // supplied C4
    const char *reason) // supplied C4
  throw (
    RTI::FederateInternalError);

  // 2.15
  virtual void federationRestoreBegun ()
    throw (
      RTI::FederateInternalError);

  // 2.16
  virtual void initiateFederateRestore (
    const char *label, // supplied C4
    RTI::FederateHandle  handle)
  throw (
    RTI::SpecifiedSaveLabelDoesNotExist,
    RTI::CouldNotRestore,
    RTI::FederateInternalError);
  
  // 2.18
  virtual void federationRestored ()
    throw (
      RTI::FederateInternalError) ;
  
  virtual void federationNotRestored ()
    throw (
      RTI::FederateInternalError);
  
  /////////////////////////////////////
  // Declaration Management Services //
  /////////////////////////////////////
  
// 3.9
virtual void startRegistrationForObjectClass (
        RTI::ObjectClassHandle   theClass)      // supplied C1
throw (
  RTI::ObjectClassNotPublished,
  RTI::FederateInternalError);

// 3.10
virtual void stopRegistrationForObjectClass (
        RTI::ObjectClassHandle   theClass)      // supplied C1
throw (
  RTI::ObjectClassNotPublished,
  RTI::FederateInternalError);

// 3.11
virtual void turnInteractionsOn (
  RTI::InteractionClassHandle theHandle) // supplied C1
throw (
  RTI::InteractionClassNotPublished,
  RTI::FederateInternalError);

// 3.12
virtual void turnInteractionsOff (
  RTI::InteractionClassHandle theHandle) // supplied C1
throw (
  RTI::InteractionClassNotPublished,
  RTI::FederateInternalError);

// 3.13
virtual void turnUpdatesOnForObjectInstance (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError);

// 3.14
virtual void turnUpdatesOffForObjectInstance (
        RTI::ObjectHandle        theObject,      // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError);
  
  ////////////////////////////////
  // Object Management Services //
  ////////////////////////////////
  
  // 4.4
  virtual void discoverObjectInstance (
    RTI::ObjectHandle          theObject,      // supplied C1
    RTI::ObjectClassHandle     theObjectClass, // supplied C1
    char const *               theObjectName)
  throw (
    RTI::CouldNotDiscover,
    RTI::ObjectClassNotKnown,
    RTI::FederateInternalError);
  
  // 4.5
  virtual void reflectAttributeValues (
          RTI::ObjectHandle                 theObject,     // supplied C1
    const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
    const RTI::FedTime&               theTime,       // supplied C1
    const char                             *theTag,        // supplied C4
          RTI::EventRetractionHandle        theHandle)     // supplied C1
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::FederateOwnsAttributes,
    RTI::InvalidFederationTime,
    RTI::FederateInternalError);

   virtual void reflectAttributeValues (
          RTI::ObjectHandle                 theObject,     // supplied C1
    const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
    const char                              *theTag)        // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::FederateOwnsAttributes,
    RTI::FederateInternalError);
  
  // 4.7
  virtual void receiveInteraction (
          RTI::InteractionClassHandle       theInteraction, // supplied C1
    const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
    const RTI::FedTime&                    theTime, 
    const char                             *theTag,         // supplied C4
          RTI::EventRetractionHandle        theHandle) 
  throw (
    RTI::InteractionClassNotKnown,
    RTI::InteractionParameterNotKnown,
    RTI::InvalidFederationTime,
    RTI::FederateInternalError);

   virtual void receiveInteraction (
          RTI::InteractionClassHandle       theInteraction, // supplied C1
    const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
    const char                             *theTag)        // supplied C4
  throw (
    RTI::InteractionClassNotKnown,
    RTI::InteractionParameterNotKnown,
    RTI::FederateInternalError);
  
  // 4.9
  virtual void removeObjectInstance (
          RTI::ObjectHandle          theObject, // supplied C1
    const RTI::FedTime&              theTime,   // supplied C1
    const char                      *theTag,    // supplied C4
          RTI::EventRetractionHandle theHandle) // supplied C1
  throw (
    RTI::ObjectNotKnown,
    RTI::InvalidFederationTime,
    RTI::FederateInternalError);

  virtual void removeObjectInstance (
          RTI::ObjectHandle          theObject, // supplied C1
    const char                      *theTag)    // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::FederateInternalError);

  // 4.14
  virtual void attributesInScope (
    RTI::ObjectHandle        theObject,     // supplied C1
    const RTI::AttributeHandleSet& theAttributes) // supplied C4
    throw (
      RTI::ObjectNotKnown,
      RTI::AttributeNotKnown,
      RTI::FederateInternalError);
  
// 4.15
virtual void attributesOutOfScope (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError);

// 4.16
virtual void interactionInScope (
  RTI::InteractionClassHandle theHandle)     // supplied C1
throw (
  RTI::InteractionClassNotKnown,
  RTI::InteractionClassNotSubscribed,
  RTI::FederateInternalError);

// 4.17
virtual void interactionOutOfScope (
  RTI::InteractionClassHandle theHandle)     // supplied C1
throw (
  RTI::InteractionClassNotKnown,
  RTI::InteractionClassNotSubscribed,
  RTI::FederateInternalError);
  
  // 4.19
  virtual void provideAttributeValueUpdate (
          RTI::ObjectHandle            theObject,     // supplied C1
    const RTI::AttributeHandleSet& theAttributes) // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::FederateInternalError);
  
  // 4.21
  virtual void requestRetraction (
    RTI::EventRetractionHandle theHandle) // supplied C1
  throw (
    RTI::EventNotKnown,
    RTI::FederateInternalError);
  
  ///////////////////////////////////
  // Ownership Management Services //
  ///////////////////////////////////
  
  // 5.3
  virtual void                            // returned C6
  requestAttributeOwnershipAssumption (
          RTI::ObjectHandle        theObject,         // supplied C1
    const RTI::AttributeHandleSet& offeredAttributes, // supplied C4
    const char                    *theTag)            // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::AttributeAlreadyOwned,
    RTI::AttributeNotPublished,
    RTI::FederateInternalError);
  
  // 5.4
  virtual void attributeOwnershipDivestitureNotification (
          RTI::ObjectHandle            theObject,          // supplied C1
    const RTI::AttributeHandleSet&     releasedAttributes) // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::AttributeNotOwned,
    RTI::AttributeDivestitureWasNotRequested,
    RTI::FederateInternalError);
  
  // 5.5
  virtual void attributeOwnershipAcquisitionNotification (
          RTI::ObjectHandle            theObject,         // supplied C1
    const RTI::AttributeHandleSet&     securedAttributes) // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::AttributeAcquisitionWasNotRequested,
    RTI::AttributeAlreadyOwned,
    RTI::AttributeNotPublished,
    RTI::FederateInternalError);
  
  // 5.8
  virtual void attributeOwnershipUnavailable (
          RTI::ObjectHandle        theObject,         // supplied C1
    const RTI::AttributeHandleSet& theAttributes) // supplied C4
    throw (
      RTI::ObjectNotKnown,
      RTI::AttributeNotKnown,
      RTI::AttributeAlreadyOwned,
      RTI::AttributeAcquisitionWasNotRequested,
      RTI::FederateInternalError);
  
  // 5.9
  virtual void requestAttributeOwnershipRelease (
          RTI::ObjectHandle        theObject,           // supplied C1
    const RTI::AttributeHandleSet& candidateAttributes, // supplied C4
    const char                    *theTag)              // supplied C4
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::AttributeNotOwned,
    RTI::FederateInternalError);

  // 5.13
  virtual void confirmAttributeOwnershipAcquisitionCancellation (
          RTI::ObjectHandle        theObject,         // supplied C1
    const RTI::AttributeHandleSet& theAttributes) // supplied C4
    throw (
      RTI::ObjectNotKnown,
      RTI::AttributeNotKnown,
      RTI::AttributeAlreadyOwned,
      RTI::AttributeAcquisitionWasNotCanceled,
      RTI::FederateInternalError);
  
  // 5.15
  virtual void informAttributeOwnership (
    RTI::ObjectHandle    theObject,    // supplied C1
    RTI::AttributeHandle theAttribute, // supplied C1
    RTI::FederateHandle  theOwner)     // supplied C1
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::FederateInternalError);
  
  virtual void attributeIsNotOwned (
    RTI::ObjectHandle    theObject,    // supplied C1
    RTI::AttributeHandle theAttribute) // supplied C1
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::FederateInternalError);

   virtual void attributeOwnedByRTI (
    RTI::ObjectHandle    theObject,    // supplied C1
    RTI::AttributeHandle theAttribute) // supplied C1
  throw (
    RTI::ObjectNotKnown,
    RTI::AttributeNotKnown,
    RTI::FederateInternalError);
  
  //////////////////////////////
  // Time Management Services //
  //////////////////////////////

  // 6.2
  virtual void timeRegulationEnabled (
    const RTI::FedTime& theFederateTime) // supplied C1
    throw (
      RTI::InvalidFederationTime,
      RTI::EnableTimeRegulationWasNotPending,
      RTI::FederateInternalError);
  
// 6.5
  virtual void timeConstrainedEnabled (
    const RTI::FedTime& theFederateTime) // supplied C1
    throw (
      RTI::InvalidFederationTime,
      RTI::EnableTimeConstrainedWasNotPending,
      RTI::FederateInternalError);
  
  // 6.18
  virtual void timeAdvanceGrant (
    const RTI::FedTime& theTime) // supplied C1
  throw (
    RTI::InvalidFederationTime,
    RTI::TimeAdvanceWasNotInProgress,
    RTI::FederationTimeAlreadyPassed,
    RTI::FederateInternalError);

  
private:

  // Utility function
  void printHandleValuePair (
    char        *value,
    RTI::Handle  theHandle,
    RTI::ULong   valueLength);
};


#endif


