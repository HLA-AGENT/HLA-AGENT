//-----------------------------------------------------------------
// Project Include Files
//-----------------------------------------------------------------
#include "TwFederateAmbassador.hh"
#include "tileWorld.hh"

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
// Bad C like global variables being externed - bad boy!!!
//-----------------------------------------------------------------
extern RTI::Boolean        timeAdvGrant;
extern RTI::Boolean        TimeRegulation;
extern RTI::Boolean        TimeConstrained;
extern RTI::FedTime  &          grantTime;


TwFederateAmbassador::TwFederateAmbassador()
{
}

TwFederateAmbassador::~TwFederateAmbassador()
throw(RTI::FederateInternalError)
{
   cerr << "FED_HW: TwFederateAmbassador::~TwFederateAmbassador destructor called in FED" << endl;
}

////////////////////////////////////
// Federation Management Services //
////////////////////////////////////

void TwFederateAmbassador::synchronizationPointRegistrationSucceeded (
  const char *label) // supplied C4)
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: synchronizationPointRegistrationSucceeded not supported in FED"
        << endl;
}

void TwFederateAmbassador::synchronizationPointRegistrationFailed (
  const char *label) // supplied C4)
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: synchronizationPointRegistrationFailed not supported in FED"
        << endl;
}

void TwFederateAmbassador::announceSynchronizationPoint (
  const char *label, // supplied C4
  const char *tag)   // supplied C4
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: announceSynchronizationPoint not supported in FED" << endl;
}

void TwFederateAmbassador::federationSynchronized (
  const char *label) // supplied C4)
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: federationSynchronized not supported in FED" << endl;
}


void TwFederateAmbassador::initiateFederateSave (
  const char *label) // supplied C4
throw (
  RTI::UnableToPerformSave,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: initiateFederateSave not supported in FED" << endl;
}


void TwFederateAmbassador::federationSaved ()
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: federationSaved not supported in FED" << endl;
}


void TwFederateAmbassador::federationNotSaved ()
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: federationNotSaved not supported in FED" << endl;
}


void TwFederateAmbassador::requestFederationRestoreSucceeded (
  const char *label) // supplied C4
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: requestFederationRestoreSucceeded not supported in FED" << endl;
}


void TwFederateAmbassador::requestFederationRestoreFailed (
  const char *label) // supplied C4
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: requestFederationRestoreFailed not supported in FED" << endl;
}


void TwFederateAmbassador::requestFederationRestoreFailed (
  const char *label,
  const char *reason) // supplied C4
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: requestFederationRestoreFailed not supported in FED" << endl;
}


void TwFederateAmbassador::federationRestoreBegun ()
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: federationRestoreBegun not supported in FED" << endl;
}


void TwFederateAmbassador::initiateFederateRestore (
  const char               *label,   // supplied C4
        RTI::FederateHandle handle)  // supplied C1
throw (
  RTI::SpecifiedSaveLabelDoesNotExist,
  RTI::CouldNotRestore,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: initiateFederateRestore not supported in FED" << endl;
}


void TwFederateAmbassador::federationRestored ()
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: federationRestored not supported in FED" << endl;
}


void TwFederateAmbassador::federationNotRestored ()
throw (
  RTI::FederateInternalError)
{
   cerr << "FED_HW: federationNotRestored not supported in FED" << endl;
}


/////////////////////////////////////
// Declaration Management Services //
/////////////////////////////////////

void TwFederateAmbassador::startRegistrationForObjectClass (
        RTI::ObjectClassHandle   theClass)      // supplied C1
throw (
  RTI::ObjectClassNotPublished,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: startRegistrationForObjectClass not supported in FED" << endl;
}


void TwFederateAmbassador::stopRegistrationForObjectClass (
        RTI::ObjectClassHandle   theClass)      // supplied C1
throw (
  RTI::ObjectClassNotPublished,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: stopRegistrationForObjectClass not supported in FED" << endl;
}


void TwFederateAmbassador::turnInteractionsOn (
  RTI::InteractionClassHandle theHandle) // supplied C1
throw (
  RTI::InteractionClassNotPublished,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: turnInteractionsOn not supported in FED" << endl;
}

void TwFederateAmbassador::turnInteractionsOff (
  RTI::InteractionClassHandle theHandle) // supplied C1
throw (
  RTI::InteractionClassNotPublished,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: turnInteractionsOff not supported in FED" << endl;
}

////////////////////////////////
// Object Management Services //
////////////////////////////////

void TwFederateAmbassador::discoverObjectInstance (
  RTI::ObjectHandle          theObject,      // supplied C1
  RTI::ObjectClassHandle     theObjectClass, // supplied C1
  const char *          theObjectName)  // supplied C4
throw (
  RTI::CouldNotDiscover,
  RTI::ObjectClassNotKnown,
  RTI::FederateInternalError)
{
   cout << "FED_HW: Discovered object " << theObject << endl;

   if ( theObjectClass == Tile::GetTileRtiId() )
   {
      //-----------------------------------------------------------------
      // Instantiate a local Country object to hold the state of the
      // remote object we just discovered.  This instance will get
      // stored in the static extent member data - it will be destructed
      // either when it is removed or when the application exits.
      //-----------------------------------------------------------------
      Tile *tmpPtr = new Tile( theObject );
   }
   else
   {
      //-----------------------------------------------------------------
      // If not Country type then don't know what to do because all I
      // know about is Country objects.
      //-----------------------------------------------------------------
      cerr << "FED_HW: Discovered object class unknown to me." << endl;
   }
}

void TwFederateAmbassador::reflectAttributeValues (
        RTI::ObjectHandle                 theObject,     // supplied C1
  const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
  const RTI::FedTime&                     theTime,       // supplied C1
  const char                             *theTag,        // supplied C4
        RTI::EventRetractionHandle        theHandle)     // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateOwnsAttributes,
  RTI::InvalidFederationTime,
  RTI::FederateInternalError)
{
   //-----------------------------------------------------------------
   // Find the Country instance this update is for.  If we can't find
   // it then I am getting data I didn't ask for.
   //-----------------------------------------------------------------
   Tile *pTile = Tile::Find( theObject );

   if ( pTile )
   {
      //-----------------------------------------------------------------
      // Set the new attribute values in this country instance.
      //-----------------------------------------------------------------
      pTile->Update( theAttributes );
      pTile->SetLastTime( theTime );
   }
   else
           throw RTI::ObjectNotKnown("received reflection for unknown OID");
}

void TwFederateAmbassador::reflectAttributeValues (
        RTI::ObjectHandle                 theObject,     // supplied C1
  const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
  const char                             *theTag)        // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateOwnsAttributes,
  RTI::FederateInternalError)
{
   //-----------------------------------------------------------------
   // Find the Country instance this update is for.  If we can't find
   // it then I am getting data I didn't ask for.
   //-----------------------------------------------------------------
   Tile *pTile = Tile::Find( theObject );

   if ( pTile )
   {
      //-----------------------------------------------------------------
      // Set the new attribute values in this country instance.
      //-----------------------------------------------------------------
      pTile->Update( theAttributes );
      RTI::FedTime * zero_time = RTI::FedTimeFactory::makeZero();
      pTile->SetLastTime(*zero_time );
      delete zero_time;
   }
}

void TwFederateAmbassador::receiveInteraction (
        RTI::InteractionClassHandle       theInteraction, // supplied C1
  const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
  const RTI::FedTime&                     theTime,        // supplied C4
  const char                             *theTag,         // supplied C4
        RTI::EventRetractionHandle        theHandle)      // supplied C1
throw (
  RTI::InteractionClassNotKnown,
  RTI::InteractionParameterNotKnown,
  RTI::InvalidFederationTime,
  RTI::FederateInternalError)
{
   this->receiveInteraction( theInteraction, theParameters, theTag );
}

void TwFederateAmbassador::receiveInteraction (
        RTI::InteractionClassHandle       theInteraction, // supplied C1
  const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
  const char                             *theTag)         // supplied C4
throw (
  RTI::InteractionClassNotKnown,
  RTI::InteractionParameterNotKnown,
  RTI::FederateInternalError)
{
   //  Pass the interaction off to the Country class
   // so that it can be processed.
   Tile::Update( theInteraction, theParameters );
}

void TwFederateAmbassador::removeObjectInstance (
        RTI::ObjectHandle          theObject, // supplied C1
  const RTI::FedTime&              theTime,   // supplied C4
  const char                      *theTag,    // supplied C4
        RTI::EventRetractionHandle theHandle) // supplied C1
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

void TwFederateAmbassador::removeObjectInstance (
        RTI::ObjectHandle          theObject, // supplied C1
  const char                      *theTag)    // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::FederateInternalError)
{
   cout << "FED_HW: Removed object " << theObject << endl;

   Tile* pTile = Tile::Find( theObject );

   if ( pTile )
   {
      delete pTile;
   }
}

void TwFederateAmbassador::attributesInScope (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: attributesInScope not supported in FED" << endl;
}

void TwFederateAmbassador::attributesOutOfScope (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: attributesOutOfScope not supported in FED" << endl;
}

void TwFederateAmbassador::provideAttributeValueUpdate (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError)
{
   //-----------------------------------------------------------------
   // Find the Country instance this request is for.
   //-----------------------------------------------------------------
   Tile *pTile = Tile::Find( theObject );

   if ( pTile )
   {
      //-----------------------------------------------------------------
      // Touch the appropriate attribute values in this country
      // instance so that the get updated next cycle.
      //-----------------------------------------------------------------
      RTI::AttributeHandle attrHandle;

      //-----------------------------------------------------------------
      // We need to iterate through the AttributeHandleSet
      // to extract each AttributeHandle.  Based on the type
      // specified ( the value returned by getHandle() ) we need to
      // set the status of whether we should send this type of data.
      //-----------------------------------------------------------------
      for (unsigned int i = 0; i < theAttributes.size(); i++ )
      {
         attrHandle = theAttributes.getHandle( i );
         if ( attrHandle == Tile::GetXposRtiId() )
         {
            // Touch population so that it gets update next cycle
            pTile->SetXpos( pTile->GetXpos() );
         }
	 else if ( attrHandle == Tile::GetYposRtiId() )
         {
            // Touch population so that it gets update next cycle
            pTile->SetYpos( pTile->GetYpos() );
         }


         else if ( attrHandle == Tile::GetNameRtiId() )
         {
            // Touch name so that it gets update next cycle
            pTile->SetName( pTile->GetName() );
         }

	 else if ( attrHandle == Tile::GetObjectRtiId() )
         {
            // Touch name so that it gets update next cycle
            pTile->SetObject( pTile->GetObject() );
         }
      }
   }
}

void TwFederateAmbassador::turnUpdatesOnForObjectInstance (
        RTI::ObjectHandle        theObject,     // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: turnUpdatesOnForObjectInstance not supported in FED" << endl;
}

void TwFederateAmbassador::turnUpdatesOffForObjectInstance (
        RTI::ObjectHandle        theObject,      // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: turnUpdatesOffForObjectInstance not supported in FED" << endl;
}

///////////////////////////////////
// Ownership Management Services //
///////////////////////////////////

void TwFederateAmbassador::requestAttributeOwnershipAssumption (
        RTI::ObjectHandle        theObject,         // supplied C1
  const RTI::AttributeHandleSet& offeredAttributes, // supplied C4
  const char                    *theTag)            // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeAlreadyOwned,
  RTI::AttributeNotPublished,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: requestAttributeOwnershipAssumption not supported in FED" << endl;
}

void TwFederateAmbassador::attributeOwnershipDivestitureNotification (
        RTI::ObjectHandle        theObject,          // supplied C1
  const RTI::AttributeHandleSet& releasedAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeNotOwned,
  RTI::AttributeDivestitureWasNotRequested,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: attributeOwnershipDivestitureNotification not supported in FED"
        << endl;
}

void TwFederateAmbassador::attributeOwnershipAcquisitionNotification (
        RTI::ObjectHandle        theObject,         // supplied C1
  const RTI::AttributeHandleSet& securedAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeAcquisitionWasNotRequested,
  RTI::AttributeAlreadyOwned,
  RTI::AttributeNotPublished,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: attributeOwnershipAcquisitionNotification not supported in FED"
        << endl;
}

void TwFederateAmbassador::attributeOwnershipUnavailable (
        RTI::ObjectHandle        theObject,         // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeAlreadyOwned,
  RTI::AttributeAcquisitionWasNotRequested,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: attributeOwnershipUnavailable not supported in FED" << endl;
}

void TwFederateAmbassador::requestAttributeOwnershipRelease (
        RTI::ObjectHandle        theObject,           // supplied C1
  const RTI::AttributeHandleSet& candidateAttributes, // supplied C4
  const char                    *theTag)              // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeNotOwned,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: requestAttributeOwnershipRelease not supported in FED" << endl;
}

void TwFederateAmbassador::confirmAttributeOwnershipAcquisitionCancellation (
        RTI::ObjectHandle        theObject,         // supplied C1
  const RTI::AttributeHandleSet& theAttributes) // supplied C4
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::AttributeAlreadyOwned,
  RTI::AttributeAcquisitionWasNotCanceled,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: confirmAttributeOwnershipAcquisitionCancellation not"
        << " supported in FED" << endl;
}

void TwFederateAmbassador::informAttributeOwnership (
  RTI::ObjectHandle    theObject,    // supplied C1
  RTI::AttributeHandle theAttribute, // supplied C1
  RTI::FederateHandle  theOwner)     // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: informAttributeOwnership not supported in FED" << endl;
}

 void TwFederateAmbassador::attributeIsNotOwned (
  RTI::ObjectHandle    theObject,    // supplied C1
  RTI::AttributeHandle theAttribute) // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: attributeIsNotOwned not supported in FED" << endl;
}

void TwFederateAmbassador::attributeOwnedByRTI (
  RTI::ObjectHandle    theObject,    // supplied C1
  RTI::AttributeHandle theAttribute) // supplied C1
throw (
  RTI::ObjectNotKnown,
  RTI::AttributeNotKnown,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: attributeOwnedByRTI not supported in FED" << endl;
}

//////////////////////////////
// Time Management Services //
//////////////////////////////

void TwFederateAmbassador::timeRegulationEnabled (
 const  RTI::FedTime& theFederateTime) // supplied C4
throw (
  RTI::InvalidFederationTime,
  RTI::EnableTimeRegulationWasNotPending,
  RTI::FederateInternalError)
{
   cout << "FED_HW: Time granted (timeRegulationEnabled) to: "
        << theFederateTime << endl;
   grantTime = theFederateTime;
   timeAdvGrant = RTI::RTI_TRUE;
   TimeRegulation = RTI::RTI_TRUE;
}

void TwFederateAmbassador::timeConstrainedEnabled (
  const RTI::FedTime& theFederateTime) // supplied C4
throw (
  RTI::InvalidFederationTime,
  RTI::EnableTimeConstrainedWasNotPending,
  RTI::FederateInternalError)
{
   cout << "FED_HW: Time granted (timeConstrainedEnabled) to: "
        << theFederateTime << endl;
   grantTime = theFederateTime;
   timeAdvGrant = RTI::RTI_TRUE;
   TimeConstrained = RTI::RTI_TRUE;
}

void TwFederateAmbassador::timeAdvanceGrant (
  const RTI::FedTime& theTime) // supplied C4
throw (
  RTI::InvalidFederationTime,
  RTI::TimeAdvanceWasNotInProgress,
  RTI::FederateInternalError)
{
   cout << "FED_HW: Time granted (timeAdvanceGrant) to: "
        << theTime << endl;
   grantTime = theTime;
   timeAdvGrant = RTI::RTI_TRUE;
}

void TwFederateAmbassador::requestRetraction (
  RTI::EventRetractionHandle theHandle) // supplied C1
throw (
  RTI::EventNotKnown,
  RTI::FederateInternalError)
{
   cerr << "FED_HW: requestRetraction not supported in FED" << endl;
}

