#ifndef RTI_AMB_INTERFACE_H
#define RTI_AMB_INTERFACE_H

/* 2. */
void do_createFederationExecution (char**, int);
void do_destroyFederationExecution (char**, int);
void do_joinFederationExecution (char**, int);
void do_resignFederationExecution (char**, int);
void do_registerSyncPoint (char**, int);
void do_syncPointAchieved (char**, int);
void do_requestFederationSave1 (char**, int);
void do_requestFederationSave2 (char**, int);
void do_federateSaveBegun (char**, int);
void do_federateSaveAchieved (char**, int);
void do_federateSaveNotAchieved (char**, int);
void do_requestRestore (char**, int);
void do_restoreAchieved (char**, int);
void do_restoreNotAchieved (char**, int);


/* 3. */
void do_publishObjectClass (char**, int);
void do_unpublishObjectClass (char**, int);
void do_publishInteractionClass (char**, int);
void do_unpublishInteractionClass (char**, int);
void do_subscribeObjectClassAttributes (char**, int); 
void do_unsubscribeObjectClass (char**, int);
void do_subscribeInteractionClass (char**, int);
void do_unsubscribeInteractionClass (char**, int);

/* 4. */
void do_registerObject1 (char**, int);
void do_registerObject2 (char**, int);
void do_updateAttributeValues (char**, int);
void do_sendInteraction (char**, int);
void do_deleteObject (char**, int);
void do_localDeleteObject (char**, int);
void do_changeAttributeTransportType (char**, int);
void do_changeAttributeOrderType (char**, int);
void do_changeInteractionTransportType (char**, int);
void do_changeInteractionOrderType (char**, int);
void do_requestObjectAttributeValueUpdate (char**, int);
void do_requestClassAttributeValueUpdate (char**, int);
void do_retract (char**, int);

/* 5. */
void do_unconditionalAttributeOwnershipDivestiture (char**, int);
void do_negotiatedAttributeOwnershipDivestiture (char**, int);
void do_attributeOwnershipAcquisition (char**, int);
void do_attributeOwnershipReleaseResponse (char**, int);
void do_cancelNegotiatedAttributeOwnershipDivestiture (char**, int);
void do_cancelAttributeOwnershipAcquisition (char**, int);
void do_attributeOwnershipAcquisitionIfAvailable (char**, int);
void do_queryAttributeOwnership (char**, int);
void do_isAttributeOwnedByFederate (char**, int);

/* 6. */
void do_enableTimeRegulation (char**, int);
void do_disableTimeRegulation (char**, int);
void do_enableTimeConstrained (char**, int);
void do_disableTimeConstrained (char**, int);
void do_requestLBTS (char**, int);
void do_requestFederateTime (char**, int);
void do_requestMinNextEventTime (char**, int);
void do_changeLookahead (char**, int);
void do_requestLookahead (char**, int);
void do_timeAdvanceRequest (char**, int);
void do_timeAdvanceRequestAvailable (char**, int);
void do_nextEventRequest (char**, int);
void do_nextEventRequestAvailable (char**, int);
void do_flushQueueRequest (char**, int);
void do_enableAsynchronousDelivery (char**, int);
void do_disableAsynchronousDelivery (char**, int);

/* 7. */
void do_createRegion (char**, int);
void do_setRangeLowerBound (char**, int);
void do_setRangeUpperBound (char**, int);
void do_getRangeLowerBound (char**, int);
void do_getRangeUpperBound (char**, int);
void do_getSpaceHandleForRegion (char**, int);
void do_getNumberOfExtents (char**, int);
void do_getRangeLowerBoundNotificationLimit (char**, int);
void do_getRangeUpperBoundNotificationLimit (char**, int);
void do_notifyRegionModification(char**, int);
void do_deleteRegion (char**, int);
void do_registerObjectWithRegion1 (char**, int);
void do_registerObjectWithRegion2 (char**, int);
void do_associateRegionForUpdates (char**, int);
void do_unassociateRegionForUpdates (char**, int);
void do_subscribeObjectClassAttributesWithRegion (char**, int);
void do_unsubscribeObjectClassWithRegion (char**, int);
void do_subscribeInteractionClassWithRegion (char**, int);
void do_unsubscribeInteractionClassWithRegion (char**, int);
void do_sendInteractionWithRegion (char**, int);
void do_requestClassAttributeValueUpdateWithRegion (char**, int);

/* 8. */
void do_getObjectClassHandle (char**, int);
void do_getObjectClassName (char**, int);
void do_getAttributeHandle (char**, int);
void do_getAttributeName (char**, int);
void do_getInteractionClassHandle (char**, int);
void do_getInteractionClassName (char**, int);
void do_getParameterHandle (char**, int);
void do_getParameterName (char**, int);
void do_getObjectHandle (char**, int);
void do_getObjectName (char**, int);
void do_getSpaceHandle (char**, int);
void do_getDimensionHandle (char**, int);
void do_getDimensionName (char**, int);
void do_getSpaceName (char**, int);
void do_getTransportationHandle (char**, int);
void do_getTransportationName (char**, int);
void do_getOrderingHandle (char**, int);
void do_getOrderingName (char**, int);
void do_getAttributeSpaceHandle (char**, int);
void do_getObjectClass (char**, int);
void do_getInteractionSpaceHandle (char **, int);
void do_enableClassRelevanceAdvisory (char**, int);
void do_disableClassRelevanceAdvisory (char**, int);
void do_enableAttributeRelevanceAdvisory (char**, int);
void do_disableAttributeRelevanceAdvisory (char**, int);
void do_enableAttributeScopeAdvisory (char**, int);
void do_disableAttributeScopeAdvisory (char**, int);
void do_enableInteractionRelevanceAdvisory (char**, int);
void do_disableInteractionRelevanceAdvisory (char**, int);
void do_tick1 (char**, int);
void do_tick2 (char**, int);
void do_autoTick (char**, int);

#endif




