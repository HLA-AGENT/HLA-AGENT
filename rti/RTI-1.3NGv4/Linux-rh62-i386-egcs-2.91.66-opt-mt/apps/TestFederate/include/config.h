
#ifndef TF_CONFIG_H
#define TF_CONFIG_H

#include <sys/types.h>
#include "datatypes.h"
#include "rtiAmbInterface.hh"

/* Naming Conventions used throughout this file:

   For the most part, names closely correspond to the RTI Ambassador functions they
   support, although parts of the names may be abbreviated in some cases (e.g.,
   createFedEx for createFederationExecution).

   Extended form prompt strings (found in Part A) are named by appending
   "_extended" to the normal form of the command.  For example,
   "subscribeObjClassAttr_extended" contains the extended form prompts, while
   "subscribeObjClassAttr" contains the normal form prompts, of the command
   "Subscribe Object Class Attributes".

   Extended form PromptStruct records (found in Part B) are named by appending "_s"
   to the normal form of the command.  This harks back to the quasi-standard of
   appending "_t" to the end of variables that are pointers to a data type.  The
   "_s" means that the variable we are defining is a data structure.  This is a
   little lame, since the PromptStruct records are the only place I use this
   convention.  Oh well.
*/


/* PART A -- Prompt Strings

   Define the prompts for all the services.  Each string provides the label for a
   prompt used in a prompt dialog box.  Strings containing carets (^) will be set
   up as option menus.  The substring between the two carets will become the label
   for the option menu, while the comma- separated list that follows the substring
   will make up the option menu items.
*/
char *createFedEx[] = {
"Federation Execution Name:",
"FedEx Data Designator:"
};
char *destroyFedEx[] = {
"Federation Execution Name:"
};
char *joinFedEx[] = {
"Federate Type:",
"Federation Execution Name:"
};
char *resignFedEx[] = {
"^Resignation Action:^Release Attributes,Delete Objects,Delete & Release,No Action,Invalid"
};
char *registerSyncPoint[] = {
"Synchronization Label:",
"User Supplied Tag:",
"Federate Set:"
};
char *syncPointAchieved[] = {
"Synchronization Label:"
};
char *requestFederationSave[] = {
"Save Label:"
};
char *requestFederationSave_extended[] = {
"Save Label:", "Federation Time:"
};
char *federateSaveBegun[] = {""};
char *federateSaveAchieved[] = {""};
char *federateSaveNotAchieved[] = {""};
char *requestRestore[] = {
  "Save Label:"
};
char *restoreAchieved[] = {""};
char *restoreNotAchieved[] = {""};
char *publishObjClass[] = {
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *unpublishObjClass[] = {
"Object Class Handle (or Name):"
};
char *publishInteractionClass[] = {
"Interaction Class Handle (or Name):"
};
char *unpublishInteractionClass[] = {
"Interaction Class Handle (or Name):"
};
char *subscribeObjClassAttr[] = {
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *unsubscribeObjClassAttr[] = {
"Object Class Handle (or Name):"
};
char *subscribeInteractionClass[] = {
"Interaction Class Handle (or Name):"
};
char *unsubscribeInteractionClass[] = {
"Interaction Class Handle (or Name):"
};
char *registerObject[] = {
"Object Class Handle (or Name):", 
"Object Name:"
};
char *registerObject_extended[] = {
"Object Class Handle (or Name):" 
};
char *updateAttrValues[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Federation Time:",
"User Supplied Tag:",
"Attribute Handle (or Name):", "                     Value:",
"Attribute Handle (or Name):", "                     Value:",
"Attribute Handle (or Name):", "                     Value:",
"Attribute Handle (or Name):", "                     Value:"
};
char *sendInteraction[] = {
"Interaction Class Handle (or Name):",
"Federation Time:", "User Supplied Tag:",
"Parameter Handle (or Name):", "                     Value:",
"Parameter Handle (or Name):", "                     Value:",
"Parameter Handle (or Name):", "                     Value:",
"Parameter Handle (or Name):", "                     Value:"
};
char *deleteObj[] = {
"Object Handle (or Name):",
"Federation Time:",
"User Supplied Tag:"
};
char *localDeleteObj[] = {
"Object Handle (or Name):"
};
char *changeAttrTransportType[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):", 
"Attribute Handle (or Name) List:",
"^Transport Type:^Reliable,Best Effort,Invalid"
};
char *changeAttrOrderType[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:",
"^Order Type:^Receive,Timestamp,Invalid"
};
char *changeInteractionTransportType[] = {
"Interaction Class Handle (or Name):",
"^Transport Type:^Reliable,Best Effort,Invalid"
};
char *changeInteractionOrderType[] = {
"Interaction Class Handle (or Name):",
"^Order Type:^Receive,Timestamp,Invalid"
};
char *requestObjAttrValueUpdate[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *requestClassAttrValueUpdate[] = {
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *retract[] = {
"Serial Number:"
};
char *unconditionalAttrOwnershipDivestiture[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *negotiatedAttrOwnershipDivestiture[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:",
"User Supplied Tag:",
};
char *attrOwnershipAcquisition[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:",
"User Supplied Tag:"
};
char *attributeOwnershipReleaseResponse[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *cancelNegotiatedAttributeOwnershipDivestiture[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *cancelAttributeOwnershipAcquisition[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *attributeOwnershipAcquisitionIfAvailable[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *queryAttrOwnership[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name):"
};
char *isAttrOwnedByFederate[] = {
"Object Handle (or Name):",
"Object Class Handle (or Name):",
"Attribute Handle (or Name):"
};
char *enableTimeRegulation[] = {
"Federation Time:",
"Lookahead:"
};
char *disableTimeRegulation[] = {""};
char *enableTimeConstrained[] = {""};
char *disableTimeConstrained[] = {""};
char *requestLBTS[] = {""};
char *requestFederateTime[] = {""};
char *requestMinNextEventTime[] = {""};
char *changeLookahead[] = {
"Lookahead:"
};
char *requestLookahead[] = {""};
char *timeAdvanceRequest[] = {
"Federation Time:"
};
char *timeAdvanceRequestAvailable[] = {
"Federation Time:"
};
char *nextEventRequest[] = {
"Federation Time:"
};
char *nextEventRequestAvailable[] = {
"Federation Time:"
};
char *flushQueueRequest[] = {
"Federation Time:"
};
char *enableAsynchronousDelivery[] = {""};
char *disableAsynchronousDelivery[] = {""};
char *createRegion[] = {
"Space Handle (or Name):",
"Number of Extents:"
};
char *setRangeLowerBound[] = {
"Region:",
"ExtentIndex:",
"Dimension Handle (or Name):",
"Lower Bound:"
};
char *setRangeUpperBound[] = {
"Region:",
"ExtentIndex:",
"Dimension Handle (or Name):",
"Upper Bound:"
};
char *getRangeLowerBound[] = {
"Region:",
"ExtentIndex:",
"Dimension Handle (or Name):"
};
char *getRangeUpperBound[] = {
"Region:",
"ExtentIndex:",
"Dimension Handle (or Name):"
};
char *getSpaceHandleForRegion[] = {
"Region:"
};
char *getNumberOfExtents[] = {
"Region:"
};
char *getRangeLowerBoundNotificationLimit[] = {
"Region:",
"ExtentIndex:",
"Dimension Handle (or Name):"
};
char *getRangeUpperBoundNotificationLimit[] = {
"Region:",
"ExtentIndex:",
"Dimension Handle (or Name):"
};
char *notifyRegionModification[] = {
"Region:"
};
char *deleteRegion[] = {
"Region:"
};
char *registerObjWithRegion[] = {
"Object Class Handle (or Name):",
"Object  Name:",
"Attribute Handle List:",
"Region List:",
"Number of Handles:"
};
char *registerObjWithRegion_extended[] = {
"Object Class Handle (or Name):",
"Attribute Handle List:",
"Region List:",
"Number of Handles:"
};
char *associateRegionForUpdates[] = {
"Region:",
"Object Handle (or Name):",
"Attribute Handle (or Name) List:"
};
char *unassociateRegionForUpdates[] = {
"Region:",
"Object Handle (or Name):"
};
char *subscribeObjClassAttributesWithRegion[] = {
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:",
"Region:"
};
char *unsubscribeObjClassWithRegion[] = {
"Object Class Handle (or Name):",
"Region:"
};
char *subscribeInteractionClassWithRegion[] = {
"Interaction Class Handle (or Name):",
"Region:"
};
char *unsubscribeInteractionClassWithRegion[] = {
"Interaction Class Handle (or Name):",
"Region:"
};
char *sendInteractionWithRegion[] = {
"Interaction Class Handle (or Name):",
"Region",
"Federation Time:", "User Supplied Tag:",
"Parameter Handle (or Name):", "                     Value:",
"Parameter Handle (or Name):", "                     Value:",
"Parameter Handle (or Name):", "                     Value:",
"Parameter Handle (or Name):", "                     Value:"
};
char *requestClassAttributeValueUpdateWithRegion[] = {
"Object Class Handle (or Name):",
"Attribute Handle (or Name) List:",
"Region:"
};
char *getObjectClassHandle[] = {
"Object Class Name:"
};
char *getObjectClassName[] = {
"Object Class Handle:"
};
char *getAttrHandle[] = {
"Attribute Name:",
"Object Class Handle (or Name):"
};
char *getAttrName[] = {
"Attribute Handle:",
"Object Class Handle (or Name):"
};
char *getInteractionClassHandle[] = {
"Interaction Class Name:"
};
char *getInteractionClassName[] = {
"Interaction Class Handle:"
};
char *getParmHandle[] = {
"Parameter Name:",
"Interaction Class Handle (or Name):"
};
char *getParmName[] = {
"Parameter Handle:",
"Interaction Class Handle (or Name):"
};
char *getObjectHandle[] = {
"Object Name:",
};
char *getObjectName[] = {
"Object Handle:",
};
char *getSpaceHandle[] = {
"Space Name:"
};
char *getSpaceName[] = {
"Space Handle:"
};
char *getDimensionHandle[] = {
"Dimension Name:",
"Space Handle (or Name):"
};
char *getDimensionName[] = {
"Dimension Handle:",
"Space Handle (or Name):"
};
char *getTransportationHandle[] = {
"Transportation Name:"
};
char *getTransportationName[] = {
"Transportation Handle:"
};
char *getOrderingHandle[] = {
"Ordering Name:"
};
char *getOrderingName[] = {
"Ordering Handle:"
};
char *getAttributeSpaceHandle[] = {
"Attribute Handle (or Name):",
"Object Class Handle (or Name):"
};
char *getObjectClass[] = {
  "Object Handle (or Name):"
};
char *getInteractionSpaceHandle[] = {
"Interaction Class Handle (or Name):"
};
char *enableClassRelevanceAdvisory[] = {""};
char *disableClassRelevanceAdvisory[] = {""};
char *enableAttributeRelevanceAdvisory[] = {""};
char *disableAttributeRelevanceAdvisory[] = {""};
char *enableAttributeScopeAdvisory[] = {""};
char *disableAttributeScopeAdvisory[] = {""};
char *enableInteractionRelevanceAdvisory[] = {""};
char *disableInteractionRelevanceAdvisory[] = {""};
char *tick[] = {""};
char *tick_extended[] = {
  "Tick Minimum:", "Tick Maximum:"
};
char *autoTick[] = {
"Tick Interval (in milliseconds):"
};
char *autoTick_extended[] = {
"Tick Minimum:", "Tick Maximum:",
"Tick Interval (in milliseconds):"
};

/* PART B -- Extended Form PromptStructs

   Define the prompt structures used for 'extended form' services.
   This data structure is defined in datatypes.h                  
*/
PromptStruct requestFederationSave_s =
  { requestFederationSave_extended,
    XtNumber(requestFederationSave_extended),
    "", NONE, do_requestFederationSave2, NULL };
PromptStruct registerObject_s =
  { registerObject_extended,
    XtNumber(registerObject_extended),
    "", NONE, do_registerObject2, NULL };
PromptStruct registerObjWithRegion_s =
  { registerObjWithRegion_extended,
    XtNumber(registerObjWithRegion_extended),
    "", NONE, do_registerObjectWithRegion2, NULL };
PromptStruct tick_s =
  { tick_extended,
    XtNumber(tick_extended),
    "", NONE, do_tick2, NULL };
PromptStruct autoTick_s =
  { autoTick_extended,
    XtNumber(autoTick_extended),
    "", NONE, do_autoTick, NULL };


/* PART C -- The promptRecs array.

   Define the prompt structures for most of the services.
   This data structure is defined in datatypes.h.

   The order of these records is very important.  They
   should appear here in the same order as defined by the
   ServiceID enumerated type, also defined in datatypes.h.
*/
PromptStruct promptRecs[] = {
  { createFedEx,                     XtNumber(createFedEx),
    "CreateFederationExecution", NONE,
    do_createFederationExecution, NULL },

  { destroyFedEx,                    XtNumber(destroyFedEx),
    "DestroyFederationExecution", NONE,
    do_destroyFederationExecution, NULL },

  { joinFedEx,                       XtNumber(joinFedEx),
    "JoinFederationExecution", NONE,
    do_joinFederationExecution, NULL },

  { resignFedEx,                     XtNumber(resignFedEx),
    "ResignFederationExecution", NONE,
    do_resignFederationExecution, NULL },

  { registerSyncPoint,                    XtNumber(registerSyncPoint),
    "RegisterFederationSynchronizationPoint", NONE,
    do_registerSyncPoint, NULL },

  { syncPointAchieved,                   XtNumber(syncPointAchieved),
    "SynchronizationPointAchieved", NONE,
    do_syncPointAchieved, NULL },

  { requestFederationSave,           XtNumber(requestFederationSave),
    "RequestFederationSave", NONE,
    do_requestFederationSave1, &requestFederationSave_s },

  { federateSaveBegun,               XtNumber(federateSaveBegun),
    "FederateSaveBegun", NONE,
    do_federateSaveBegun, NULL },

  { federateSaveAchieved,            XtNumber(federateSaveAchieved),
    "FederateSaveAchieved", NONE,
    do_federateSaveAchieved, NULL },

  { federateSaveNotAchieved,         XtNumber(federateSaveNotAchieved),
    "FederateSaveAchieved", NONE,
    do_federateSaveNotAchieved, NULL },

  { requestRestore,                  XtNumber(requestRestore),
    "RequestRestore", NONE,
    do_requestRestore, NULL },

  { restoreAchieved,                 XtNumber(restoreAchieved),
    "RestoreAchieved", NONE,
    do_restoreAchieved, NULL },

  { restoreNotAchieved,              XtNumber(restoreNotAchieved),
    "RestoreAchieved", NONE,
    do_restoreNotAchieved, NULL },

  { publishObjClass,                 XtNumber(publishObjClass),
    "PublishObjectClass", NONE,
    do_publishObjectClass, NULL },

  { unpublishObjClass,               XtNumber(unpublishObjClass),
    "PublishObjectClass", NONE,
    do_unpublishObjectClass, NULL },

  { publishInteractionClass,         XtNumber(publishInteractionClass),
    "PublishInteractionClass", NONE,
    do_publishInteractionClass, NULL },

  { unpublishInteractionClass,       XtNumber(unpublishInteractionClass),
    "PublishInteractionClass", NONE,
    do_unpublishInteractionClass, NULL },

  { subscribeObjClassAttr,           XtNumber(subscribeObjClassAttr),
    "SubscribeObjectClassAttribute", NONE,
    do_subscribeObjectClassAttributes, NULL },

  { unsubscribeObjClassAttr,         XtNumber(unsubscribeObjClassAttr),
    "SubscribeObjectClassAttribute", NONE,
    do_unsubscribeObjectClass, NULL },

  { subscribeInteractionClass,       XtNumber(subscribeInteractionClass),
    "SubscribeInteractionClass", NONE,
    do_subscribeInteractionClass, NULL },

  { unsubscribeInteractionClass,     XtNumber(unsubscribeInteractionClass),
    "SubscribeInteractionClass", NONE,
    do_unsubscribeInteractionClass, NULL },
  
  { registerObject,                  XtNumber(registerObject),
    "RegisterObject", NONE,
    do_registerObject1, &registerObject_s },

  { updateAttrValues,                XtNumber(updateAttrValues),
    "UpdateAttributeValues", NONE,
    do_updateAttributeValues, NULL },

  { sendInteraction,                 XtNumber(sendInteraction),
    "SendInteraction", NONE,
    do_sendInteraction, NULL },

  { deleteObj,                       XtNumber(deleteObj),
    "DeleteObject", NONE,
    do_deleteObject, NULL },

  { localDeleteObj,                  XtNumber(localDeleteObj),
    "LocalDeleteObject", NONE,
    do_localDeleteObject, NULL },

  { changeAttrTransportType,         XtNumber(changeAttrTransportType),
    "ChangeAttributeTransportationType", NONE,
    do_changeAttributeTransportType, NULL },

  { changeAttrOrderType,             XtNumber(changeAttrOrderType),
    "ChangeAttributeOrderType", NONE,
    do_changeAttributeOrderType, NULL },

  { changeInteractionTransportType,  XtNumber(changeInteractionTransportType),
    "ChangeInteractionTransportationType", NONE,
    do_changeInteractionTransportType, NULL },

  { changeInteractionOrderType,      XtNumber(changeInteractionOrderType),
    "ChangeInteractionOrderType", NONE,
    do_changeInteractionOrderType, NULL },

  { requestObjAttrValueUpdate,       XtNumber(requestObjAttrValueUpdate),
    "RequestAttributeValueUpdate", NONE,
    do_requestObjectAttributeValueUpdate, NULL },

  { requestClassAttrValueUpdate,     XtNumber(requestClassAttrValueUpdate),
    "RequestAttributeValueUpdate", NONE,
    do_requestClassAttributeValueUpdate, NULL },

  { retract,                         XtNumber(retract),
    "Retract", NONE,
    do_retract, NULL },
  
  { unconditionalAttrOwnershipDivestiture,
    XtNumber(unconditionalAttrOwnershipDivestiture),
    "UnconditionalAttrOwnershipDivestiture", NONE,
    do_unconditionalAttributeOwnershipDivestiture,
    NULL },

  { negotiatedAttrOwnershipDivestiture,
    XtNumber(negotiatedAttrOwnershipDivestiture),
    "NegotiatedAttrOwnershipDivestiture", NONE,
    do_negotiatedAttributeOwnershipDivestiture,
    NULL },

  { attrOwnershipAcquisition, XtNumber(attrOwnershipAcquisition),
    "AttributeOwnershipAcquisition", NONE,
    do_attributeOwnershipAcquisition, NULL },

  { attributeOwnershipReleaseResponse, XtNumber(attributeOwnershipReleaseResponse),
    "AttributeOwnershipReleaseResponse", NONE,
    do_attributeOwnershipReleaseResponse, NULL },

  { cancelNegotiatedAttributeOwnershipDivestiture,
    XtNumber(cancelNegotiatedAttributeOwnershipDivestiture),
    "CancelNegotiatedAttributeOwnershipDivestiture", NONE,
    do_cancelNegotiatedAttributeOwnershipDivestiture, NULL },

  { cancelAttributeOwnershipAcquisition,
    XtNumber(cancelAttributeOwnershipAcquisition),
    "CancelAttributeOwnershipAcquisition", NONE,
    do_cancelAttributeOwnershipAcquisition, NULL },

  { attributeOwnershipAcquisitionIfAvailable,
    XtNumber(attributeOwnershipAcquisitionIfAvailable),
    "AttributeOwnershipAcquisitionIfAvailable", NONE,
    do_attributeOwnershipAcquisitionIfAvailable, NULL },
  
  { queryAttrOwnership,              XtNumber(queryAttrOwnership),
    "QueryAttributeOwnership", NONE,
    do_queryAttributeOwnership, NULL },

  { isAttrOwnedByFederate, XtNumber(isAttrOwnedByFederate),
    "IsAttributeOwnedByFederate", NONE,
    do_isAttributeOwnedByFederate, NULL },

  { enableTimeRegulation,           XtNumber(enableTimeRegulation),
    "EnableTimeRegulation", NONE,
    do_enableTimeRegulation, NULL },

  { disableTimeRegulation,           XtNumber(disableTimeRegulation),
    "DisableTimeRegulation", NONE,
    do_disableTimeRegulation, NULL },

  { enableTimeConstrained,           XtNumber(enableTimeConstrained),
    "EnableTimeConstrained", NONE,
    do_enableTimeConstrained, NULL },

  { disableTimeConstrained,           XtNumber(disableTimeConstrained),
    "DisableTimeConstrained", NONE,
    do_disableTimeConstrained, NULL },
  
  { requestLBTS,                     XtNumber(requestLBTS),
    "RequestLBTS", NONE,
    do_requestLBTS, NULL },

  { requestFederateTime,             XtNumber(requestFederateTime),
    "RequestFederateTime", NONE,
    do_requestFederateTime, NULL },

  { requestMinNextEventTime,         XtNumber(requestMinNextEventTime),
    "RequestMinimumNextEventTime", NONE,
    do_requestMinNextEventTime, NULL },

  { changeLookahead,                    XtNumber(changeLookahead),
    "ChangeLookahead", NONE,
    do_changeLookahead, NULL },

  { requestLookahead,                XtNumber(requestLookahead),
    "RequestLookahead", NONE,
    do_requestLookahead, NULL },

  { timeAdvanceRequest,              XtNumber(timeAdvanceRequest),
    "TimeAdvanceRequest", NONE,
    do_timeAdvanceRequest, NULL },

  { timeAdvanceRequestAvailable,     XtNumber(timeAdvanceRequest),
    "TimeAdvanceRequest", NONE,
    do_timeAdvanceRequestAvailable, NULL },

  { nextEventRequest,                XtNumber(nextEventRequest),
    "NextEventRequest", NONE,
    do_nextEventRequest, NULL },

  { nextEventRequestAvailable,       XtNumber(nextEventRequest),
    "NextEventRequest", NONE,
    do_nextEventRequestAvailable, NULL },

  { flushQueueRequest,               XtNumber(flushQueueRequest),
    "FlushQueueRequest", NONE,
    do_flushQueueRequest, NULL },

  { enableAsynchronousDelivery, XtNumber(enableAsynchronousDelivery),
    "EnableAsynchronousDelivery", NONE,
    do_enableAsynchronousDelivery, NULL },
  
  { disableAsynchronousDelivery, XtNumber(disableAsynchronousDelivery),
    "DisableAsynchronousDelivery", NONE,
    do_disableAsynchronousDelivery, NULL },
  
  { createRegion,       XtNumber(createRegion),
    "CreateRegion", NONE,
    do_createRegion, NULL },

  { setRangeLowerBound,       XtNumber(setRangeLowerBound),
    "SetRangeLowerBound", NONE,
    do_setRangeLowerBound, NULL },

  { setRangeUpperBound,       XtNumber(setRangeUpperBound),
    "SetRangeUpperBound", NONE,
    do_setRangeUpperBound, NULL },

  { getRangeLowerBound,       XtNumber(getRangeLowerBound),
    "GetRangeLowerBound", NONE,
    do_getRangeLowerBound, NULL },
  
  { getRangeUpperBound,       XtNumber(getRangeUpperBound),
    "GetRangeUpperBound", NONE,
    do_getRangeUpperBound, NULL },
  
  { getSpaceHandleForRegion,   XtNumber(getSpaceHandleForRegion),
    "GetSpaceHandleForRegion", NONE,
    do_getSpaceHandleForRegion, NULL },
  
  { getNumberOfExtents,   XtNumber(getNumberOfExtents),
    "GetNumberOfExtents", NONE,
    do_getNumberOfExtents, NULL },

   { getRangeLowerBoundNotificationLimit,
     XtNumber(getRangeLowerBoundNotificationLimit),
     "GetRangeLowerBoundNotificationLimit", NONE,
     do_getRangeLowerBoundNotificationLimit, NULL },
  
  { getRangeUpperBoundNotificationLimit,
    XtNumber(getRangeUpperBoundNotificationLimit),
    "GetRangeUpperBoundNotificationLimit", NONE,
    do_getRangeUpperBoundNotificationLimit, NULL },
  
  { notifyRegionModification, XtNumber(notifyRegionModification),
    "notifyAboutRegionModification", NONE,
    do_notifyRegionModification, NULL },

  { deleteRegion,       XtNumber(deleteRegion),
    "DeleteRegion", NONE,
    do_deleteRegion, NULL },

  { registerObjWithRegion,      XtNumber(registerObjWithRegion),
    "RegisterObjectWithRegion", NONE,
    do_registerObjectWithRegion1, &registerObjWithRegion_s },

  { associateRegionForUpdates,      XtNumber(associateRegionForUpdates),
    "AssociateRegionForUpdates", NONE,
    do_associateRegionForUpdates, NULL },
  
  { unassociateRegionForUpdates,     XtNumber(unassociateRegionForUpdates),
    "AssociateRegionForUpdates", NONE,
    do_unassociateRegionForUpdates, NULL },
  
  { subscribeObjClassAttributesWithRegion,
    XtNumber(subscribeObjClassAttributesWithRegion),
    "SubscribeObjectClassAttributesWithRegion", NONE,
    do_subscribeObjectClassAttributesWithRegion, NULL },
  
  { unsubscribeObjClassWithRegion,   XtNumber(unsubscribeObjClassWithRegion),
    "UnsubscribeObjectClassWithRegion", NONE,
    do_unsubscribeObjectClassWithRegion, NULL },
  
  { subscribeInteractionClassWithRegion,
    XtNumber(subscribeInteractionClassWithRegion),
    "SubscribeInteractionClassWithRegion", NONE,
    do_subscribeInteractionClassWithRegion, NULL },
  
  { unsubscribeInteractionClassWithRegion,
    XtNumber(unsubscribeInteractionClassWithRegion),
    "UnsubscribeInteractionClassWithRegion", NONE,
    do_unsubscribeInteractionClassWithRegion, NULL },
  
  { sendInteractionWithRegion,       XtNumber(sendInteractionWithRegion),
    "SendInteractionWithRegion", NONE,
    do_sendInteractionWithRegion, NULL },
  
  { requestClassAttributeValueUpdateWithRegion,
    XtNumber(requestClassAttributeValueUpdateWithRegion),
    "RequestClassAttributeValueUpdateWithRegion", NONE,
    do_requestClassAttributeValueUpdateWithRegion, NULL },
  
  { getObjectClassHandle,               XtNumber(getObjectClassHandle),
    "getObjectClassHandle", NONE,
    do_getObjectClassHandle, NULL },

  { getObjectClassName,                 XtNumber(getObjectClassName),
    "getObjectClassName", NONE,
    do_getObjectClassName, NULL },
  
  { getAttrHandle,                   XtNumber(getAttrHandle),
    "getAttributeHandle", NONE,
    do_getAttributeHandle, NULL },
  
  { getAttrName,                     XtNumber(getAttrName),
    "getAttributeName", NONE,
    do_getAttributeName, NULL },

  { getInteractionClassHandle,       XtNumber(getInteractionClassHandle),
    "getInteractionClassHandle", NONE,
    do_getInteractionClassHandle, NULL },

  { getInteractionClassName,         XtNumber(getInteractionClassName),
    "getInteractionClassName", NONE,
    do_getInteractionClassName, NULL },
  
  { getParmHandle,                   XtNumber(getParmHandle),
    "getParameterHandle", NONE,
    do_getParameterHandle, NULL },

  { getParmName,                     XtNumber(getParmName),
    "getParameterName", NONE,
    do_getParameterName, NULL },

  { getObjectHandle,                   XtNumber(getObjectHandle),
    "getObjectHandle", NONE,
    do_getObjectHandle, NULL },
  
  { getObjectName,                     XtNumber(getObjectName),
    "getObjectName", NONE,
    do_getObjectName, NULL },
  
  { getSpaceHandle,                  XtNumber(getSpaceHandle),
    "getSpaceHandle", NONE,
    do_getSpaceHandle, NULL },

  { getSpaceName,                    XtNumber(getSpaceName),
    "getSpaceName", NONE,
    do_getSpaceName, NULL },

  { getDimensionHandle,              XtNumber(getDimensionHandle),
    "getDimensionHandle", NONE,
    do_getDimensionHandle, NULL },
  
  { getDimensionName,              XtNumber(getDimensionName),
    "getDimensionName", NONE,
    do_getDimensionName, NULL},

  { getTransportationHandle, XtNumber(getTransportationHandle),
    "getTransportationHandle", NONE,
    do_getTransportationHandle, NULL },

  { getTransportationName, XtNumber(getTransportationName),
    "getTransportationName", NONE,
    do_getTransportationName, NULL },

  { getOrderingHandle, XtNumber(getOrderingHandle),
    "getOrderingHandle", NONE,
    do_getOrderingHandle, NULL },

  { getOrderingName, XtNumber(getOrderingName),
    "getOrderingName", NONE,
    do_getOrderingName, NULL },
  
  { getAttributeSpaceHandle,       XtNumber(getAttributeSpaceHandle),
    "getAttributeSpaceHandle", NONE,
    do_getAttributeSpaceHandle, NULL},
  
  { getObjectClass,       XtNumber(getObjectClass),
    "getObjectClass", NONE,
    do_getObjectClass, NULL},
  
  { getInteractionSpaceHandle,       XtNumber(getInteractionSpaceHandle),
    "getInteractionSpaceHandle", NONE,
    do_getInteractionSpaceHandle, NULL},
  
  { enableClassRelevanceAdvisory,   XtNumber(enableClassRelevanceAdvisory),
    "EnableClassRelevanceAdvisorySwitch", NONE,
    do_enableClassRelevanceAdvisory, NULL },

  { disableClassRelevanceAdvisory,   XtNumber(disableClassRelevanceAdvisory),
    "DisableClassRelevanceAdvisorySwitch", NONE,
    do_disableClassRelevanceAdvisory, NULL },

  { enableAttributeRelevanceAdvisory, 
    XtNumber(enableAttributeRelevanceAdvisory),
    "EnableAttributeRelevanceAdvisorySwitch", NONE,
    do_enableAttributeRelevanceAdvisory, NULL },

  { disableAttributeRelevanceAdvisory,
    XtNumber(disableAttributeRelevanceAdvisory),
    "DisableAttributeRelevanceAdvisorySwitch", NONE,
    do_disableAttributeRelevanceAdvisory, NULL },

  { enableAttributeScopeAdvisory,    XtNumber(enableAttributeScopeAdvisory),
    "EnableAttributeScopeAdvisorySwitch", NONE,
    do_enableAttributeScopeAdvisory, NULL },

  { disableAttributeScopeAdvisory,   XtNumber(disableAttributeScopeAdvisory),
    "DisableAttributeScopeAdvisorySwitch", NONE,
    do_disableAttributeScopeAdvisory, NULL },

  { enableInteractionRelevanceAdvisory, 
    XtNumber(enableInteractionRelevanceAdvisory),
    "EnableInteractionRelevanceAdvisorySwitch", NONE,
    do_enableInteractionRelevanceAdvisory, NULL },

  { disableInteractionRelevanceAdvisory,
    XtNumber(disableInteractionRelevanceAdvisory),
    "DisableInteractionRelevanceAdvisorySwitch", NONE,
    do_disableInteractionRelevanceAdvisory, NULL },

  { tick,                            XtNumber(tick),
    "tick", NONE,
    do_tick1, &tick_s },

  { autoTick,                        XtNumber(autoTick),
    "tick", NONE,
    do_autoTick, &autoTick_s }
};

#endif
