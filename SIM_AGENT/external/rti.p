/*----------------------------------------------------------
FILE: RTI.P
AUTHOR: Michael Lees
REVISIONS: Created 09/05/02

This file is part of an interface between the C++ RTI implementation
and pop11. The original reason for the interface was making SIM_AGENT
HLA compliant. However, the code is more general and allows RTI calls
to be made and RTI callbacks received.

CONTENTS

Federate Ambassador
-------------------



RTI Ambassador
------------------
-1. FEDERATION MANAGEMENT
--1.1 createFederationExecution(executionName,Fedfile):void,
--1.2 destroyFederationExecution(executionName):void,
--1.3 joinFederationExecution(yourName, executionName, fedAmbRef):ulong,
--1.4 resignFederationExecution():void,

-2. DECLARATION MANAGEMENT
--2.1 publishObjectClass(classHandle, AttributeHandleSet):void,
--2.2 subscribeObjectClassAttributes(classHandle, AttributeHandleSet):void,

-3. OBJECT MANAGEMENT
--3.1 registerObjectInstance(classHandle):ulong,
--3.2 updateAttributeValues(instanceId, AttributeHandleValuePairSet,
                            theTime):void,
--3.3 deleteObjectInstance(instanceId, theTime):void,
--3.4 requestClassAttributeValueUpdate(classHandle, AttributeHandleSet):void,
--3.5 requestObjectAttributeValueUpdate(instanceId, AttributeHandleSet):void,

-4. OWNERSHIP MANAGEMENT
--4.1 unconditionalAttributeOwnershipDivesture(instanceId,
                                               AttributeHandleSet):void,
--4.2 attributeOwnershipAcquisitionIfAvailable(instanceId,
                                               AttributeHandleSet):void,

-5. TIME MANAGEMENT
--5.1 enableTimeRegulation(theFederateTime, theLookahead):void,
--5.2 timeAdvanceRequest(theTime):void,
--5.3 enableTimeConstrained():void,

-6. SUPPORT SERVICES
--6.1 getObjectClassHandle(theName):ulong,
--6.2 getObjectClassName(instanceId):char *
--6.3 getAttributeName(AttributeId, fromClass):char *
--6.4 getAttributeHandle(theName, fromClass):ulong,

-----------------------------------------------------------*/


;;; Once rti.so is loaded use external_show(); to list all external
;;; load history 

uses newexternal;


external declare rti in c;

typedef struct _another_callback
{
 unsigned long Id;
 char *value;
 unsigned long length;
}AttributeHandleValuePair;


;;; This is the replacment for RTI::AttributeHandleValuePairSet it
;;; stores an array of the above type and the number of items in the array
typedef struct AttributeHandleValuePairSet
{
  AttributeHandleValuePair *ahvp;
  unsigned long size;
}AttributeHandleValuePairSet_t;

;;; This is the replacment for RTI::AttributeHandleSet it stores an
;;; array of attribute Ids/Handles (unsigned long) and the number of
;;; items in the array
typedef struct AttributeHandleSet
{
  unsigned long *attributes;
  unsigned long size;
}AttributeHandleSet_t;


;;;This is the NODE of the linked list used to store callbacks from the
;;;RTI. This datatype is used in SimFederateAmbassador.cpp
typedef struct NODE{
 		    ;;;NODE_t *Next;		    ;;; NODE *Next; ;;;The next NODE in the linked list
 unsigned long objectHandle;
 unsigned long classHandle;
 unsigned long eventRetractionHandle;
 AttributeHandleValuePairSet_t* AHVPset; ;;;pointer to  type defined above
 unsigned long theTime;
 AttributeHandleSet_t *theAttributes; ;;;pointer to above type
 char* tag;
 char* name;  
}NODE_t;

typedef struct Queue{
  NODE_t *head; 
  NODE_t *tail;
  int size;
}Queue_t;


/* The get functions for each type of callback - the C versions
   of these functions can be found in FedAmbWrapper            */


    NODE_t *get_discoverObjectInstance()
    {}

    NODE_t *get_reflectAttributeValues()
    {}

    NODE_t *get_removeObjectInstance()
    {}

    NODE_t *get_provideAttributeValueUpdate()
    {}

    NODE_t *get_attributeOwnershipDivestitureNotification()
    {}

    NODE_t*get_attributeOwnershipAcquisitionNotification()
    {}

    NODE_t *get_attributeOwnershipUnavailable()
    {}

    NODE_t *get_timeRegulationEnabled()
    {}

    NODE_t *get_timeConstrainedEnabled()
    {}

    NODE_t *get_timeAdvanceGrant()
    {}


    /*----------------------
    All functions are C wrappers for the RTIAmb methods.
    They can be found in the RTIAmbWrapper.cpp file
    -----------------------*/
    
    void dlinit(librarypath)
    char *librarypath;
    {}

    /*--------------------------------------------------------------------*/
    /*1. FEDERATION MANAGEMENT*/
    /*--------------------------------------------------------------------*/


    /*--------------------------------
    1.1 createFederationExecution
    call syntax eg.: createFederationExecution('MyExecution','File.fed')
    ----------------------------------*/
    void createFederationExecution(executionName,Fedfile)
     char *executionName;
     char *Fedfile;
    {}

    /*--------------------------------
    1.2 destroyFederationExecution
    call syntax eg.: destroyFederationExecution('MyExecution')
    ----------------------------------*/
    void destroyFederationExecution(executionName)
     char *executionName;
    {}


/*----SORT OUT fedAmbRef-------*/
    /*--------------------------------
    1.3 joinFederationExecution
    call syntax eg.: joinFederationExecution('FederateName','MyExecution',
                                              fedAmbRef)->federateId;
    fedAmbRef - this is an external pointer to the federate Ambassador
                it can be obtained by calling eg. getFedAmbRef()->fedAmbRef
    federateId - this is the unique Id for the federate once joined
    ----------------------------------*/
    unsigned long joinFederationExecution(federateName,executionName,fedAmbRef)
     char *federateName;
     char *executionName;
     unsigned *fedAmbRef; /*bsl suggestion*/
    {}

    /*--------------------------------
    1.4 resignFederationExecution
    call syntax eg.: resignFederationExecution()
    ----------------------------------*/
    void resignFederationExecution()
    {}


    /*--------------------------------------------------------------------*/
    /*2. DECLARATION MANAGEMENT*/
    /*--------------------------------------------------------------------*/

    
    /*--------------------------------
    2.1 publishObjectClass
    call syntax eg.: publishObjectClass(classHandle, AttributeHandleSet)
    classHandle - this is an unsigned long representing the class
    whose attributes we will be publishing
    AttributeHandleSet - Is an array of unsigned longs representing
    the attributes which we will be publishing(see types above)
    ----------------------------------*/
    void publishObjectClass(classHandle,AttributeHandleSet)
     unsigned long classHandle;
     AttributeHandleSet_t *AttributeHandleSet;
    {}

    /*--------------------------------
    2.2 subscribeObjectClassAttributes
    call syntax eg.: subscribeObjectClass(classHandle, AttributeHandleSet)
    classHandle - this is an unsigned long representing the class
    whose attributes we will be subscribing to
    AttributeHandleSet - Is an array of unsigned longs representing
    the attributes which we will be subscribing to
    ----------------------------------*/
    void subscribeObjectClassAttributes(classHandle,AttributeHandleSet)
     unsigned long classHandle;
     AttributeHandleSet_t *AttributeHandleSet;
    {}


    /*--------------------------------------------------------------------*/
    /*3. OBJECT MANAGEMENT*/
    /*--------------------------------------------------------------------*/


    /*--------------------------------
    3.1 registerObjectInstance
    call syntax eg.: registerObjectInstance(classHandle)->instanceId
    classHandle - this is an unsigned long representing the class to
    which the object we are registering belongs
    InstanceId - this is an unsigned long which is unique Id
    representing the instance of the object we have just registered
    ----------------------------------*/
    unsigned long registerObjectInstance(classHandle)
     unsigned long classHandle;
    {}

/*---------Sort out how to pass and return time-----------*/

    /*--------------------------------
    3.2 updateAttributeValues
    call syntax eg.:updateAttributeValues(instanceId,
                                          AttributeHandleValuePairSet, theTime)
    instanceId - unsigned long indicating the instance whose
    attributes we are updating
    AttributeHandleValuePairSet - An array of the user defined type
    AttributeHandleValuePair(see above)
    theTime - Some representation of time
    ----------------------------------*/
    void updateAttributeValues(instanceId, AttributeHandleValuePairSet,theTime)
     unsigned long instanceId;
     AttributeHandleValuePair *AttributeHandleValuePairSet;
     unsigned long theTime;  /*work around*/
    {}

    /*--------------------------------
    3.3 deleteObjectInstance
    call syntax eg.:deleteObjectInstace(instanceId, theTime)
    instanceId - unsigned long indicating the instance we're deleting
    theTime - Some representation of time
    ----------------------------------*/
    void deleteObjectInstance(instanceId, theTime)
     unsigned long instanceId;
     unsigned long theTime;  
    {}

    /*--------------------------------
    3.4 requestClassAttributeValueUpdate
    call syntax eg.: requestClassAttributeValueUpdate(classHandle,
                                                      AttributeHandleSet)
    classHandle - this is an unsigned long representing the class
    whose attributes we're interested in
    AttributeHandleSet - Is an array of unsigned longs representing
    the attributes which we want updates for
    ----------------------------------*/
    void requestClassAttributeValueUpdate(classHandle,AttributeHandleSet)
     unsigned long classHandle;
     unsigned long *AttributeHandleSet;
    {}

    /*--------------------------------
    3.5 requestObjectAttributeValueUpdate
    call syntax eg.: requestObjectAttributeValueUpdate(instanceId, 
                                                       AttributeHandleSet)
    instanceId - this is an unsigned long representing the instance
    whose attributes we're interested in
    AttributeHandleSet - Is an array of unsigned longs representing
    the attributes which we want updates for
    ----------------------------------*/
    void requestObjectAttributeValueUpdate(instanceId,AttributeHandleSet)
     unsigned long instanceId;
     unsigned long *AttributeHandleSet;
    {}


    /*--------------------------------------------------------------------*/
    /*4. OWNERSHIP MANAGEMENT*/
    /*--------------------------------------------------------------------*/


    /*--------------------------------
    4.1 unconditionalAttributeOwnershipDivesture
    call syntax eg.: unconditionalAttributeOwnershipDivesture(instanceId, 
                                                         AttributeHandleSet)
    instanceId - this is an unsigned long representing the instance
    whose attributes we own and want to get rid of
    AttributeHandleSet - Is an array of unsigned longs representing
    the attributes which we want to give up ownership of
    ----------------------------------*/
    void unconditionalAttributeOwnershipDivesture(instanceId,
						  AttributeHandleSet)
    unsigned long instanceId;
    unsigned long *AttributeHandleSet;
    {}

    /*--------------------------------
    4.2 attributeOwnershipAcquisitionIfAvailable
    call syntax eg.: attributeOwnershipAcquisitionIfAvailable(instanceId, 
                                                            AttributeHandleSet)
    instanceId - this is an unsigned long representing the instance
    whose attributes we want to own
    AttributeHandleSet - Is an array of unsigned longs representing
    the attributes which we want to get ownership of
    ----------------------------------*/
    void attributeOwnershipAcquisitionIfAvailable(instanceId,
						  AttributeHandleSet)
     unsigned long instanceId;
     unsigned long *AttributeHandleSet;
    {}



/*---------SORT OUT TIME AS ABOVE---------*/
    
    /*--------------------------------------------------------------------*/
    /*5. TIME MANAGEMENT*/
    /*--------------------------------------------------------------------*/

    /*--------------------------------
    5.1 enableTimeRegulation
    call syntax eg.: enableTimeRegulation(theFederateTime, theLookahead)
    theFederateTime - Some representation of the current time
    theLookahead - Some representation of lookahead
    ----------------------------------*/
    void enableTimeRegulation(theFederateTime, theLookahead)
     unsigned long theFederateTime;
     unsigned long theLookahead;
    {}

    /*--------------------------------
    5.2 timeAdvanceRequest
    call syntax eg.: timeAdvanceRequest(theTime)
    theTime - Some representation of the requested time
    ----------------------------------*/
    void timeAdvanceRequest(theTime)
     unsigned long theTime;
    {}

    /*--------------------------------
    5.3 enableTimeConstrained
    call syntax eg.: enableTimeConstrained()
    ----------------------------------*/
    void enableTimeConstrained()
    {}


    /*--------------------------------------------------------------------*/
    /*6. SUPPORT SERVICES*/
    /*--------------------------------------------------------------------*/


    /*--------------------------------
    6.1 getObjectClassHandle
    call syntax eg.: getObjectClassHandle(theName)->classHandle
    theName - string representing (in FOM) the name of the class which
    we want the handle of
    classHandle - Is an unsigned long representing the class Id
    ----------------------------------*/
    unsigned long getObjectClassHandle(theName)
     char *theName;
    {}

    /*--------------------------------
    6.2 getObjectClassName
    call syntax eg.: getObjectClassName(instanceId)->theName
    theName - string representing (in FOM) the name of the class to
    which the object with ID=instanceId belongs
    instanceId - unsigned long representing the instance whose class we want
    ----------------------------------*/
    char *getObjectClassName(instanceId)
     unsigned long instanceId;
    {}

    /*--------------------------------
    6.3 getAttributeName
    call syntax eg.: getAttributeName(attrId,classHandle)->theName
    attrId - unsigned long representing the attribute whose name we want
    classHandle - unsigned long representing the class to which the
    attribute belongs.
    theName - the attributes name (FOM)
    ----------------------------------*/
    char *getAttributeName(attrId,classHandle)
     unsigned long attrId;
     unsigned long classHandle;
    {}

    
    /*--------------------------------
    6.4 getAttributeHandle
    call syntax eg.: getAttributeHandle(theName, classHandle)->attrId
    theName - string representing (in FOM) the name of the attribute
    which we want the handle of
    classHandle - Is an unsigned long representing the class Id to
    which the attribute belongs
    attrId - unsigned long representing The attributes ID
    ----------------------------------*/
    unsigned long getAttributeHandle(theName, classHandle)
     char *theName;
     unsigned long classHandle;
    {}

    /*--------------------------------
    **.** tick
    call syntax eg.: getAttributeHandle(1.0, 1.1)
    Give control to RTI for between 1 second and 1.1 seconds.
    ----------------------------------*/
    void tick(minimum, maximum)
     double minimum;
     double maximum;
    {}

endexternal;


external load rti;
'/home/mhl/SIM_AGENT/external/dlinit.so'
endexternal;


dlinit('/home/mhl/rti/RTI-1.3NGv4/Linux-rh62-i386-egcs-2.91.66-opt-mt/apps/tileWorld/data/librti.so');
