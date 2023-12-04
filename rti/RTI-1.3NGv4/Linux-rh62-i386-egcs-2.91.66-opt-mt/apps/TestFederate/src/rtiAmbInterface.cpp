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
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <RTI.hh>
#include <fedtime.hh>
#include <fedAmbImpl.hh>
#include <rtiAmbInterface.hh>
#include <datatypes.h>

extern void outputMsg (char *);
extern void updateStatus (void);
extern void scheduleNextTick (char *);
extern void autoTickEngaged (void);

int  countListElements (const char* const listString);

static RTI::Boolean checkRequiredParameters (
        char **fields,
  const int fieldCnt);

static void buildAttributeList (
  const char* const             attrString,
        RTI::AttributeHandle    attrList[],
  const int                     attrListLength,
        char*                   objClassHandleStr,
        RTI::ObjectClassHandle& objClassHandle);

static void buildAttributeList (
  const char* const             attrString,
  RTI::AttributeHandle    attrList[],
  const int                     attrListLength,
        RTI::ObjectClassHandle& objClassHandle);

static void buildParameterList (
  const char* const                  parmString,
        RTI::AttributeHandle         parmList[],
  const int                          parmListLength,
        char*                        interactionClassHandleStr,
        RTI::InteractionClassHandle& interactionClassHandle);

static void buildFederateList (
  const char* const          federateString,
        RTI::FederateHandle  federateList[],
  const int                  federateCnt);

static RTI::Boolean buildRegionList (
  const char* const          regionString,
        RTI::Region         *regionList[],
  const int                  regionCnt);

static int determineUpdateAttrTestCharacteristics (
  const char* const             attributeCnt,
  const char* const             charactersPerValue,
  const char* const             objClassName,
  const RTI::ObjectClassHandle  objClassHandle,
        int&                    attrCnt,
        int&                    attrValueStringLength);

static int determineRoundTripTestCharacteristics (
  const char* const                  parameterCnt,
  const char* const                  charactersPerValue,
  const char* const                  interactionClassName,
  const RTI::InteractionClassHandle  interactionClassHandle,
        int&                         parmCnt,
        int&                         parmValueStringLength);

unsigned long  convertLongID (const char* const s);
unsigned short convertShortID (const char* const s);

extern char              federateHandle[6];
extern char              federateName[MAX_NAME_LENGTH];
extern char              federationName[MAX_NAME_LENGTH];

RTI::RTIambassador             myRTIamb;
static FedAmbImpl              myFederateAmb;
RTI::FederateHandle     myHandle;

// Region database
#define MAX_NUM_REGIONS 100
RTI::Region *regionDB[MAX_NUM_REGIONS];
int nextRegionID = 0;

const unsigned short INVALID_ID = 65535;

// User ranges for entering bounds
#define USER_MIN_EXTENT 0
#define USER_MAX_EXTENT 10000

////////////////////////////////////////////////////////////////////////////////
// Federation Management Services
////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
// 2.1
void do_createFederationExecution (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);
  char *FED;

  enum { FEDEX_NAME, FED_FILE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    myRTIamb.createFederationExecution (fields[FEDEX_NAME], fields[FED_FILE]);
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
}


//-------------------------------------------------------------------------------
// 2.2
void do_destroyFederationExecution (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { FEDEX_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    myRTIamb.destroyFederationExecution (fields[FEDEX_NAME]);
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
}


//-------------------------------------------------------------------------------
// 2.3
void do_joinFederationExecution (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { FED_NAME, FEDEX_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    myHandle = myRTIamb.joinFederationExecution (fields[FED_NAME],
                                                 fields[FEDEX_NAME], 
                                                 &myFederateAmb); 
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Error:  Caught unknown exception");
    return;
  }

  strncpy (federateName, fields[FED_NAME], MAX_NAME_LENGTH);
  sprintf (federateHandle, "%i", myHandle);
  strncpy (federationName, fields[FEDEX_NAME], MAX_NAME_LENGTH);

  updateStatus ();
}


//-------------------------------------------------------------------------------
// 2.4
void do_resignFederationExecution (char **fields, int fieldCnt)
{ 
  char            buf[200];
  ostrstream      myStream (buf, 200);

  enum { RESIGN_ACTION };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    myRTIamb.resignFederationExecution ((RTI::ResignAction)
                                        (atoi (fields[RESIGN_ACTION])));
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  strcpy (federationName, "");
  updateStatus ();
}

//-------------------------------------------------------------------------------
// 2.5
void do_registerSyncPoint (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { SYNC_LABEL, USER_TAG, FED_LIST };

  int                    fedCnt;
  RTI::FederateHandle    fedList[MAX_FEDERATE];
  RTI::FederateHandleSet *fedSet;

  try
  {
    if (strlen(fields[FED_LIST]) > 0) {
      fedCnt  = countListElements (fields[FED_LIST]);
      fedSet = RTI::FederateHandleSetFactory::create (fedCnt);
    
      buildFederateList (fields[FED_LIST],
                         fedList, fedCnt);
    
      for (int i = 0; i < fedCnt; i++)
        fedSet->add (fedList[i]);

      myRTIamb.registerFederationSynchronizationPoint (
        fields[SYNC_LABEL], fields[USER_TAG], *fedSet);

      delete fedSet;
    }
    else {
      myRTIamb.registerFederationSynchronizationPoint (
        fields[SYNC_LABEL], fields[USER_TAG]);
    }
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }
}


//-------------------------------------------------------------------------------
// 2.7
void do_syncPointAchieved (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { SYNC_LABEL };

  try
  {
    myRTIamb.synchronizationPointAchieved (fields[SYNC_LABEL]);
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
}


//-------------------------------------------------------------------------------
// 2.11
void do_requestFederationSave1 (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { SAVE_LABEL };

  try
  { 
    myRTIamb.requestFederationSave (fields[SAVE_LABEL]);
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
}

//-------------------------------------------------------------------------------
void do_requestFederationSave2 (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { SAVE_LABEL, FED_TIME };

  try
  {
    RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
    
    myRTIamb.requestFederationSave (fields[SAVE_LABEL], *theTime);
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
}


//-------------------------------------------------------------------------------
// 2.13
void do_federateSaveBegun (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);
  
  try
  { 
    myRTIamb.federateSaveBegun ();
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
}

//-------------------------------------------------------------------------------
// 2.14
void do_federateSaveAchieved (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  try
  { 
    myRTIamb.federateSaveComplete ();
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
}

//-------------------------------------------------------------------------------
void do_federateSaveNotAchieved (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  try
  { 
    myRTIamb.federateSaveNotComplete ();
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
}


//-------------------------------------------------------------------------------
// 2.15
void do_requestRestore (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { SAVE_LABEL };

  try
  { 
    myRTIamb.requestFederationRestore (fields[SAVE_LABEL]);
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
}


//-------------------------------------------------------------------------------
// 2.17
void do_restoreAchieved (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  try
  { 
    myRTIamb.federateRestoreComplete ();
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
}

//-------------------------------------------------------------------------------
void do_restoreNotAchieved (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  try
  { 
    myRTIamb.federateRestoreNotComplete ();
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
}

////////////////////////////////////////////////////////////////////////////////
// Declaration Management Services
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
// 3.1
void do_publishObjectClass (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i;
  int                      attrCnt;
  RTI::AttributeHandle     attrList[MAX_ATTRIBUTES_PER_CLASS];
  RTI::ObjectClassHandle   objClassHandle;
  RTI::AttributeHandleSet *pubAttrs;

  enum { OBJ_CLASS_HANDLE, ATTR_LIST };

  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    pubAttrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      pubAttrs->add (attrList[i]);
    
    myRTIamb.publishObjectClass (objClassHandle, *pubAttrs);
    
    delete pubAttrs;
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
  
}

//-------------------------------------------------------------------------------
// 3.2
void do_unpublishObjectClass (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ObjectClassHandle  objClassHandle;
  
  enum { OBJ_CLASS_HANDLE };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    myRTIamb.unpublishObjectClass (objClassHandle);
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
}


//-------------------------------------------------------------------------------
// 3.3
void do_publishInteractionClass (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;
  
  enum { INTERACTION_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us an interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);
    
    myRTIamb.publishInteractionClass (interactionClassHandle);
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
}

//-------------------------------------------------------------------------------
// 3.4
void do_unpublishInteractionClass (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;
  
  enum { INTERACTION_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us an interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);
    
    myRTIamb.unpublishInteractionClass (interactionClassHandle);
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
}


//-------------------------------------------------------------------------------
// 3.5
void do_subscribeObjectClassAttributes (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i;
  int                      attrCnt;
  RTI::ObjectClassHandle   objClassHandle;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *subAttrs;
  
  enum { OBJ_CLASS_HANDLE, ATTR_LIST };

  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    subAttrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      subAttrs->add (attrList[i]);
    
    myRTIamb.subscribeObjectClassAttributes (objClassHandle, *subAttrs);
    
    delete subAttrs;
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
}

//-------------------------------------------------------------------------------
// 3.6
void do_unsubscribeObjectClass (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ObjectClassHandle  objClassHandle;
  
  enum { OBJ_CLASS_HANDLE };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    myRTIamb.unsubscribeObjectClass (objClassHandle);
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
}

//-------------------------------------------------------------------------------
// 3.7
void do_subscribeInteractionClass (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;
  
  enum { INTERACTION_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us an interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);
    
    myRTIamb.subscribeInteractionClass (interactionClassHandle);
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
}

//-------------------------------------------------------------------------------
// 3.8
void do_unsubscribeInteractionClass (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;
  
  enum { INTERACTION_CLASS_HANDLE };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us an interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);
    
    myRTIamb.unsubscribeInteractionClass (interactionClassHandle);
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
}


////////////////////////////////////////////////////////////////////////////////
// Object Management Services
////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 4.2
void do_registerObject1 (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ObjectClassHandle  objClassHandle;
  RTI::ObjectHandle       objHandle;
  
  enum { OBJ_CLASS_HANDLE, OBJ_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    objHandle = myRTIamb.registerObjectInstance (objClassHandle, fields[OBJ_NAME]);
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
  myStream << "registerObject returned Object Handle: " << objHandle << '\0';
  outputMsg (buf);
}

//-----------------------------------------------------------------------------
// 4.2
void do_registerObject2 (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ObjectClassHandle  objClassHandle;
  RTI::ObjectHandle       objHandle;
  
  enum { OBJ_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    objHandle = myRTIamb.registerObjectInstance (objClassHandle);
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
  myStream << "registerObject returned Object Handle: " << objHandle << '\0';
  outputMsg (buf);
}

//-----------------------------------------------------------------------------
// 4.3
void do_updateAttributeValues (char **fields, int fieldCnt)
{
  char                              buf[200];
  ostrstream                        myStream (buf, 200);
  char                              attrString[256];
  int                               attrStringCnt;
  int                               i, attrCnt;
  RTI::ObjectClassHandle            objClassHandle = 0;
  RTI::AttributeHandle              attrList[4];
  RTI::AttributeHandleValuePairSet *attrVals;
  RTI::EventRetractionHandle        theHandle;
  RTI::Boolean                      first = RTI::RTI_TRUE;
  long                              signedLongVal;
  short                             signedShortVal;
  float                             floatVal;
  double                            doubleVal;
  unsigned char                     hexVal[500];
  char                              hexChar[5];
  char                              hexString[1000];
  unsigned long                     hexStringLength;
  char                              formatCode;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, FED_TIME, USER_TAG,
         ATTR_NAME1, ATTR_VAL1, ATTR_NAME2, ATTR_VAL2, 
         ATTR_NAME3, ATTR_VAL3, ATTR_NAME4, ATTR_VAL4 };

  try
  {
    // Get object handle
    RTI::ObjectHandle objHandle = strtoul(fields[OBJECT_HANDLE], 0, 10);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
  
    // In the code that follows, you will see expressions that look like:
    //    ATTR_NAME1 + i * 2
    //    ATTR_VAL1  + i * 2
    //
    //  The incoming fields are placed into the fields array in the order
    //  given by the enumeration above.  When we want to iterate through
    //  the attribute names, we use the expression:  ATTR_NAME1 + i * 2.
    //  As i changes, we will retrieve ATTR_NAME1, ATTR_NAME2, etc.  This
    //  also applies to the attribute values with ATTR_VAL1, ATTR_VAL2, etc.

    // Convert the individual attribute names into a comma-delimited
    // string of attributes names.  This will be needed by buildAttributeList.

    attrString[0] = '\0';
    attrStringCnt = 0;
    for (attrCnt = 0; attrCnt < 4; attrCnt++)
    {
      if (strcmp (fields[ATTR_NAME1 + attrCnt * 2], "") != 0)
      {
        //  Insert a comma before every attribute, except the first.
        if (!first)
          strcat (attrString, ",");
        else
          first = RTI::RTI_FALSE;
        
        //  Insert the attribute into the list.
        strcat (attrString, fields[ATTR_NAME1 + attrCnt * 2]);
        attrStringCnt++;
      }
      else
        break;
    }
    
    // Convert the attribute names to attribute handles, as necessary.
    buildAttributeList (attrString, attrList, attrStringCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    
    // Now build the Attribute Handle/Value Pair Set.
    attrVals = RTI::AttributeSetFactory::create (attrStringCnt);
    
    for (i = 0; i < attrStringCnt; i++)
    {
      if(*(fields[ATTR_VAL1 + i * 2]) == '^')
      {
        formatCode = tolower(*(fields[ATTR_VAL1 + i * 2] + 1));
        
        if(formatCode == 'l' &&
           *(fields[ATTR_VAL1 + i * 2] + 2) == ' ' )
        {
          signedLongVal = atoi(fields[ATTR_VAL1 + i * 2] + 3);
          attrVals->add (attrList[i],
                         (char *)&signedLongVal,
                         sizeof(signedLongVal));
        }
        else if (formatCode == 's' &&
                 *(fields[ATTR_VAL1 + i * 2] + 2) == ' ' )
        {
          signedShortVal = (short)atoi(fields[ATTR_VAL1 + i * 2] + 3);
          attrVals->add (attrList[i],
                         (char *)&signedShortVal,
                         sizeof(signedShortVal));
        }
        else if (formatCode == 'f' &&
                 *(fields[ATTR_VAL1 + i * 2] + 2) == ' ' )
        {
          floatVal = atof(fields[ATTR_VAL1 + i * 2] + 3);
          attrVals->add (attrList[i],
                         (char *)&floatVal,
                         sizeof(floatVal));
        }
        else if (formatCode == 'd' &&
                 *(fields[ATTR_VAL1 + i * 2] + 2) == ' ' )
        {
          doubleVal = (double)atof(fields[ATTR_VAL1 + i * 2] + 3);
          attrVals->add (attrList[i],
                         (char *)&doubleVal,
                         sizeof(doubleVal));
        }
        else if (formatCode == 'h' &&
                 *(fields[ATTR_VAL1 + i * 2] + 2) == ' ' )
        {
          strcpy(hexString, fields[ATTR_VAL1 + i * 2] + 3);
          hexStringLength = strlen(hexString);
          
          if (hexStringLength % 2)
          {
            strcat(hexString, "0");
            hexStringLength = strlen(hexString);
          }
          
          for (int index = 0; index < hexStringLength; index = index + 2)
          {
            hexChar[0] = hexString[index];
            hexChar[1] = hexString[index + 1];
            hexChar[2] = '\0';
            
            hexVal[index/2] = (unsigned char)
              strtol(hexChar, (char **)NULL, 16);
          }
          attrVals->add (attrList[i],
                         (char *)hexVal,
                         hexStringLength/2);
        }
        else
        {
          attrVals->add (attrList[i],
                         fields[ATTR_VAL1 + i * 2],
                         strlen (fields[ATTR_VAL1 + i * 2]) + 1);
        }
      }
      else
      {
        attrVals->add (attrList[i],
                       fields[ATTR_VAL1 + i * 2],
                       strlen (fields[ATTR_VAL1 + i * 2]) + 1);
      }
    }
    
    // Check the time field to see if a time was specified
    
    if (strlen(fields[FED_TIME]) > 0) {
      // Time specified
      
      RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
      
      // Finally, call the Update Attribute Values RTI function.
      theHandle = myRTIamb.updateAttributeValues (
        objHandle,
        *attrVals,
        *theTime,
        (char *)fields[USER_TAG]);

      myStream << "Update Attribute Values Event Retraction Handle: \n" <<
        "  Serial Number:  "   << theHandle.theSerialNumber <<
        "  Federate Handle:  " << theHandle.sendingFederate << '\0';
      outputMsg (buf);
    }
    else {
      // No time specified
      
      // Finally, call the Update Attribute Values RTI function.
      myRTIamb.updateAttributeValues (
        objHandle,
        *attrVals,
        (char *)fields[USER_TAG]);
    }

    delete attrVals;
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }
}

//-----------------------------------------------------------------------------
//4.6
void do_sendInteraction (char **fields, int fieldCnt)
{
  char                              buf[200];
  ostrstream                        myStream (buf, 200);
  char                              parmString[256];
  int                               parmStringCnt;
  int                               i, parmCnt;
  RTI::InteractionClassHandle       interactionClassHandle = 0;
  RTI::ParameterHandle              parmList[4];
  RTI::ParameterHandleValuePairSet *parmVals;
  RTI::EventRetractionHandle        theHandle;
  RTI::Boolean                      first = RTI::RTI_TRUE;
  long                              signedLongVal;
  short                             signedShortVal;
  float                             floatVal;
  double                            doubleVal;
  unsigned char                     hexVal[500];
  char                              hexChar[5];
  char                              hexString[1000];
  unsigned long                     hexStringLength;
  char                              formatCode;
  
  enum { INTERACTION_CLASS_HANDLE, FED_TIME, USER_TAG,
         PARM_NAME1, PARM_VAL1, PARM_NAME2, PARM_VAL2, 
         PARM_NAME3, PARM_VAL3, PARM_NAME4, PARM_VAL4 };

  // In the code that follows, you will see expressions that look like:
  //    PARM_NAME1 + i * 2
  //    PARM_VAL1  + i * 2
  //
  //  The incoming fields are placed into the fields array in the order
  //  given by the enumeration above.  When we want to iterate through
  //  the attribute names, we use the expression:  PARM_NAME1 + i * 2.
  //  As i changes, we will retrieve PARM_NAME1, PARM_NAME2, etc.  This
  //  also applies to the attribute values with PARM_VAL1, PARM_VAL2, etc.

  try
  {
    // Convert the interaction Class Handle.
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us a interaction name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);
    
    // Convert the individual parameter names into a comma-delimited
    // string of parameter names.  This will be needed by buildAttributeList.
    parmString[0] = '\0';
    parmStringCnt = 0;
    for (parmCnt = 0; parmCnt < 4; parmCnt++)
    {
      if (strcmp (fields[PARM_NAME1 + parmCnt * 2], "") != 0)
      {
        //  Insert a comma before every parameter, except the first.
        if (!first)
          strcat (parmString, ",");
        else
          first = RTI::RTI_FALSE;
        
        //  Insert the parameter into the list.
        strcat (parmString, fields[PARM_NAME1 + parmCnt * 2]);
        parmStringCnt++;
      }
      else
        break;
    }
    
    // Convert the parameter names to parameter handles, as necessary.
    buildParameterList (parmString, parmList, parmStringCnt,
                        fields[INTERACTION_CLASS_HANDLE],
                        interactionClassHandle);
    
    // Now build the Parameter Handle/Value Pair Set.
    parmVals = RTI::ParameterSetFactory::create (parmStringCnt);
    
    for (i = 0; i < parmStringCnt; i++)
    {
      if(*(fields[PARM_VAL1 + i * 2]) == '^')
      {
        formatCode = tolower(*(fields[PARM_VAL1 + i * 2] + 1));
        
        if(formatCode == 'l' &&
           *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
          
        {
          signedLongVal = atoi(fields[PARM_VAL1 + i * 2] + 3);
          parmVals->add (parmList[i],
                         (char *)&signedLongVal,
                         sizeof(signedLongVal));
        }
        else if (formatCode == 's' &&
                 *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
        {
          signedShortVal = (short)atoi(fields[PARM_VAL1 + i * 2] + 3);
          parmVals->add (parmList[i],
                         (char *)&signedShortVal,
                         sizeof(signedShortVal));
        }
        else if (formatCode == 'f' &&
                 *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
        {
          floatVal = atof(fields[PARM_VAL1 + i * 2] + 3);
          parmVals->add (parmList[i],
                         (char *)&floatVal,
                         sizeof(floatVal));
        }
        else if (formatCode == 'd' &&
                 *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
        {
          doubleVal = (double)atof(fields[PARM_VAL1 + i * 2] + 3);
          parmVals->add (parmList[i],
                         (char *)&doubleVal,
                         sizeof(doubleVal));
        }
        else if (formatCode == 'h' &&
                 *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
        {
          strcpy(hexString, fields[PARM_VAL1 + i * 2] + 3);
          hexStringLength = strlen(hexString);
          
          if (hexStringLength % 2)
          {
            strcat(hexString, "0");
            hexStringLength = strlen(hexString);
          }
          
          for (int index = 0; index < hexStringLength; index = index + 2)
          {
            hexChar[0] = hexString[index];
            hexChar[1] = hexString[index + 1];
            hexChar[2] = '\0';
            
            hexVal[index/2] = (unsigned char)
              strtol(hexChar, (char **)NULL, 16);
          }
          parmVals->add (parmList[i],
                         (char *)hexVal,
                         hexStringLength/2);
        }
        else
        {
          parmVals->add (parmList[i],
                         fields[PARM_VAL1 + i * 2],
                         strlen (fields[PARM_VAL1 + i * 2]) + 1);
        }
      }
      else
      {
        parmVals->add (parmList[i],
                       fields[PARM_VAL1 + i * 2],
                       strlen (fields[PARM_VAL1 + i * 2]) + 1);
      }
    }
    
    if (strlen(fields[FED_TIME]) > 0) {
      // Time specified
      
      RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
      
      // Finally, call the Send Interaction RTI function.
      theHandle = myRTIamb.sendInteraction (
        interactionClassHandle,
        *parmVals,
        *theTime,
        (char *)fields[USER_TAG]);
      
      myStream << "Send Interaction Event Retraction Handle: \n" <<
        "  Serial Number:  "   << theHandle.theSerialNumber <<
        "  Federate Handle:  " << theHandle.sendingFederate << '\0';
      outputMsg (buf);
      
    }
    else { 
      // Finally, call the Send Interaction RTI function.
      myRTIamb.sendInteraction (
        interactionClassHandle,
        *parmVals,
        (char *)fields[USER_TAG]);
    }
    delete parmVals;
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }
}

//-----------------------------------------------------------------------------
// 4.8
void do_deleteObject (char **fields, int fieldCnt)
{
  char                        buf[200];
  ostrstream                  myStream (buf, 200);
  RTI::EventRetractionHandle  theHandle;
   
  enum { OBJECT_HANDLE, FED_TIME, USER_TAG };
  
  
  try
  {
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
  
    if (strlen(fields[FED_TIME]) > 0) {
      // Time specified

      RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
      
      theHandle = myRTIamb.deleteObjectInstance (
        objHandle,
        *theTime,
        (char *)fields[USER_TAG]);

      myStream << "Delete Object Event Retraction Handle: \n" <<
        "  Serial Number:  "   << theHandle.theSerialNumber <<
        "  Federate Handle:  " << theHandle.sendingFederate << '\0';
      outputMsg (buf);
      
    } else {
      myRTIamb.deleteObjectInstance (
        objHandle,
        (char *)fields[USER_TAG]);
    }
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }
}

//-----------------------------------------------------------------------------
// 4.9
void do_localDeleteObject (char **fields, int fieldCnt)
{
  char                        buf[100];
  ostrstream                  myStream (buf, 100);
   
  enum { OBJECT_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    myRTIamb.localDeleteObjectInstance (objHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }
  
}

//-----------------------------------------------------------------------------    
// 4.10
void do_changeAttributeTransportType (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;

  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST, TRANSPORT_TYPE };

  // Get object handle
  RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
  if (objHandle == INVALID_ID)
    //  Ok, the user gave us a object name.  Convert it to a handle.
    objHandle =
      myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
  
  try
  {
    // Build the Attribute set.
    attrCnt = countListElements (fields[ATTR_LIST]);
    attrs   = RTI::AttributeHandleSetFactory::create (attrCnt);

    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.changeAttributeTransportationType (
      objHandle,
      *attrs,
      (RTI::TransportType)
      atoi (fields[TRANSPORT_TYPE]));
 
    delete attrs;
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
}

//-----------------------------------------------------------------------------
// 4.11
void do_changeAttributeOrderType (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;

  enum { OBJECT_ID, OBJ_CLASS_HANDLE, ATTR_LIST, ORDER_TYPE };

  try
  {
    // Build the Attribute set.
    attrCnt = countListElements (fields[ATTR_LIST]);
    attrs   = RTI::AttributeHandleSetFactory::create (attrCnt);

    buildAttributeList (fields[ATTR_LIST],
   attrList, attrCnt,
   fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);

    myRTIamb.changeAttributeOrderType ((RTI::ObjectHandle)
                                       atol (fields[OBJECT_ID]),
                                       *attrs,
                                       (RTI::OrderType)
                                       atoi (fields[ORDER_TYPE]));
 
    delete attrs;
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
}

//-----------------------------------------------------------------------------
// 4.12
void do_changeInteractionTransportType (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;

  enum { INTERACTION_CLASS_HANDLE, TRANSPORT_TYPE };
  
  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us an interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);
    
    myRTIamb.changeInteractionTransportationType (interactionClassHandle,
                                             (RTI::TransportType)
                                             atoi (fields[TRANSPORT_TYPE]));
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
}

//-----------------------------------------------------------------------------
// 4.13
void do_changeInteractionOrderType (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;

  enum { INTERACTION_CLASS_HANDLE, ORDER_TYPE };

  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us an interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);
    
    myRTIamb.changeInteractionOrderType (interactionClassHandle,
                                         (RTI::OrderType)
                                         atoi (fields[ORDER_TYPE]));
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
}

//-----------------------------------------------------------------------------
// 4.18
void do_requestObjectAttributeValueUpdate (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[30];
  RTI::AttributeHandleSet *attrs;

  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST };
  try
  {
    
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    // Build the Attribute set.
    attrCnt = countListElements (fields[ATTR_LIST]);
    attrs   = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.requestObjectAttributeValueUpdate (objHandle, *attrs);
    delete attrs;
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
}

//-----------------------------------------------------------------------------
// 4.18
void do_requestClassAttributeValueUpdate (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[30];
  RTI::AttributeHandleSet *attrs;

  enum { OBJ_CLASS_HANDLE, ATTR_LIST };

  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    // Build the Attribute set.
    attrCnt = countListElements (fields[ATTR_LIST]);
    attrs   = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.requestClassAttributeValueUpdate (objClassHandle, *attrs);
    
    delete attrs;
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
}

//-----------------------------------------------------------------------------
// 4.20
void do_retract (char **fields, int fieldCnt)
{
  char                        buf[200];
  ostrstream                  myStream (buf, 200);
  RTI::EventRetractionHandle  theHandle;

  enum {SERIAL_NUMBER };

  try
  {
    theHandle.theSerialNumber = (RTI::UniqueID)atol (fields[SERIAL_NUMBER]);
    theHandle.sendingFederate = myHandle;
    
    myRTIamb.retract (theHandle);
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
}

////////////////////////////////////////////////////////////////////////////////
// Ownership Management Services
////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// 5.1
void do_unconditionalAttributeOwnershipDivestiture (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST };

  try
  {
    
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    // Build the Attribute set.
    attrCnt = countListElements (fields[ATTR_LIST]);
    attrs   = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.unconditionalAttributeOwnershipDivestiture (objHandle, *attrs);
    
    delete attrs;
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
}

//-----------------------------------------------------------------------------
// 5.2
void do_negotiatedAttributeOwnershipDivestiture (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt, federateCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST, USER_TAG};

  try
  {
    
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    // Build the Attribute set.
    attrCnt = countListElements (fields[ATTR_LIST]);
    attrs   = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.negotiatedAttributeOwnershipDivestiture (objHandle,
                                                      *attrs,
                                                      fields[USER_TAG]);
    
    delete attrs;
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
}

//-----------------------------------------------------------------------------
// 5.6
void do_attributeOwnershipAcquisition (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST, USER_TAG };

  try
  {
    
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    attrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.attributeOwnershipAcquisition (objHandle,
                                            *attrs,
                                            fields[USER_TAG]);
    
    delete attrs;
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
}


//-----------------------------------------------------------------------------
// 5.8
void do_attributeOwnershipReleaseResponse (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  char                     handleStr[6];
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;
  RTI::AttributeHandleSet *releasedAttrs;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST };

  try
  {
    
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    attrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    releasedAttrs = myRTIamb.attributeOwnershipReleaseResponse (objHandle,
                                                                *attrs);

    // Print out successfully release attrs
    attrCnt = releasedAttrs->size();
    
    if(attrCnt > 0)
      myStream << "Released Attributes:  ";
    else
      myStream << "No Attributes were Released." << endl;
    
    RTI::Boolean first = RTI::RTI_TRUE;
    for (int i = 0; i < attrCnt; i++)
    {
      sprintf (handleStr, "%i", releasedAttrs->getHandle (i));
      
      if (first)
        first = RTI::RTI_FALSE;
      else
        myStream << ",";
      
      myStream << handleStr;
    }
    
    outputMsg (buf);
    delete attrs;
    delete releasedAttrs;
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
}

//-----------------------------------------------------------------------------
// 5.9
void do_cancelNegotiatedAttributeOwnershipDivestiture (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST };

  try
  {
    
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    attrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.cancelNegotiatedAttributeOwnershipDivestiture (objHandle,
                                                            *attrs);
    
    delete attrs;
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
}

//-----------------------------------------------------------------------------
// 5.10
void do_cancelAttributeOwnershipAcquisition (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST };

  try
  {
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    attrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.cancelAttributeOwnershipAcquisition (objHandle, *attrs);
    
    delete attrs;
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
}

//-----------------------------------------------------------------------------
// 5.12
void do_attributeOwnershipAcquisitionIfAvailable (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *attrs;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_LIST };

  try
  {
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    attrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);
    
    myRTIamb.attributeOwnershipAcquisitionIfAvailable (objHandle,
                                                       *attrs);
    
    delete attrs;
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
}


//-----------------------------------------------------------------------------
// 5.7
void do_queryAttributeOwnership (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ObjectClassHandle  objClassHandle;
  RTI::AttributeHandle    attrHandle;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_HANDLE };

  try
  {
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    attrHandle = convertLongID (fields[ATTR_HANDLE]);
    if (attrHandle == INVALID_ID)
    {
      //  Ok, the user gave us an attribute name.  To convert this to a
      //  handle, we first need to get the object handle.
      
      objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
      if (objClassHandle == INVALID_ID)
        //  Ok, the user gave us a object class name.  Convert it to a handle.
        objClassHandle =
          myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
      
      attrHandle = myRTIamb.getAttributeHandle (fields[ATTR_HANDLE],
                                                objClassHandle);
    }
    
    myRTIamb.queryAttributeOwnership (objHandle,
                                      attrHandle);
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
}

//-----------------------------------------------------------------------------
// 5.9
void do_isAttributeOwnedByFederate (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ObjectClassHandle  objClassHandle;
  RTI::AttributeHandle    attrHandle;
  
  enum { OBJECT_HANDLE, OBJ_CLASS_HANDLE, ATTR_HANDLE };

  try
  {
    
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);
    
    attrHandle = convertLongID (fields[ATTR_HANDLE]);
    if (attrHandle == INVALID_ID)
    {
      //  Ok, the user gave us an attribute name.  To convert this to a
      //  handle, we first need to get the object handle.
      
      objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
      if (objClassHandle == INVALID_ID)
        //  Ok, the user gave us a object class name.  Convert it to a handle.
        objClassHandle =
          myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
      
      attrHandle = myRTIamb.getAttributeHandle (fields[ATTR_HANDLE],
                                                objClassHandle);
    }
    
    if (myRTIamb.isAttributeOwnedByFederate (objHandle,
                                             attrHandle))
      myStream << "Attribute " << fields[ATTR_HANDLE] <<
        " IS owned by Federate" << '\0';
    else
      myStream << "Attribute " << fields[ATTR_HANDLE] <<
        " is NOT owned by Federate" << '\0';
    
    outputMsg (buf);
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
}

////////////////////////////////////////////////////////////////////////////////
// Time Management Services
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
//6.1 
void do_enableTimeRegulation (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);
  enum { FED_TIME, LOOKAHEAD };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
    RTIfedTime *theLookahead = new RTIfedTime(atof (fields[LOOKAHEAD]));
    
    myRTIamb.enableTimeRegulation (*theTime, *theLookahead);
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
}

//------------------------------------------------------------------------------
// 6.3
void do_disableTimeRegulation (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.disableTimeRegulation ();
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
}

//------------------------------------------------------------------------------
//6.4 
void do_enableTimeConstrained (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);


  try
  {
    myRTIamb.enableTimeConstrained ();
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
}

//------------------------------------------------------------------------------
//6.6 
void do_disableTimeConstrained (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);
  
  
  try
  {
    myRTIamb.disableTimeConstrained ();
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
}

//------------------------------------------------------------------------------
// 6.8
void do_requestLBTS (char **fields, int fieldCnt)
{
  char                 buf[200];
  ostrstream           myStream (buf, 200);
  RTI::FedTime* theTime = RTI::FedTimeFactory::makeZero();
  
  try
  {
    myRTIamb.queryLBTS (*theTime);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "LBTS = " << theTime << '\0';
  //sprintf (buf,"LBTS = %f\n",((RTIfedTime*)theTime)->getTime());
  outputMsg (buf);
  delete theTime;
}

//------------------------------------------------------------------------------
// 6.9
void do_requestFederateTime (char **fields, int fieldCnt)
{
  char                 buf[200];
  ostrstream           myStream (buf, 200);
  RTI::FedTime* theTime = RTI::FedTimeFactory::makeZero();
  
  try
  {
    myRTIamb.queryFederateTime (*theTime);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Federate Time = " << theTime << '\0';
  //sprintf (buf,"Federate Time = %f\n",((RTIfedTime*)theTime)->getTime());
  outputMsg (buf);
  delete theTime;
}

//------------------------------------------------------------------------------
// 6.10
void do_requestMinNextEventTime (char **fields, int fieldCnt)
{
  char                 buf[200];
  ostrstream           myStream (buf, 200);
  RTI::FedTime* theTime = RTI::FedTimeFactory::makeZero();
  
  try
  {
    myRTIamb.queryMinNextEventTime (*theTime);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Minimum Next Event Time = " << theTime << '\0';
  //sprintf (buf,"Minimum Next Event Time = %f\n",((RTIfedTime*)theTime)->getTime());
  outputMsg (buf);
  delete theTime;
}

//------------------------------------------------------------------------------
// 6.11
void do_changeLookahead (char **fields, int fieldCnt)
{
  char                      buf[200];
  ostrstream                myStream (buf, 200);
  
  enum { LOOKAHEAD };

  try
  {
    RTIfedTime *theLookahead = new RTIfedTime(atof (fields[LOOKAHEAD]));
    myRTIamb.modifyLookahead (*theLookahead);
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
}

//------------------------------------------------------------------------------
// 6.12
void do_requestLookahead (char **fields, int fieldCnt)
{
  char                      buf[200];
  ostrstream                myStream (buf, 200);
  RTI::FedTime* theTime = RTI::FedTimeFactory::makeZero();
  
  try
  {
    myRTIamb.queryLookahead (*theTime);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Lookahead = " << theTime << '\0';
  //sprintf (buf,"Lookahead = %f\n",((RTIfedTime*)theTime)->getTime());
  outputMsg (buf);
  delete theTime;
}

//------------------------------------------------------------------------------
// 6.13
void do_timeAdvanceRequest (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);
  
  enum { FED_TIME };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
    
    myRTIamb.timeAdvanceRequest (*theTime);
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
}

//------------------------------------------------------------------------------
// 6.14
void do_timeAdvanceRequestAvailable (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);
  
  enum { FED_TIME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
    
    myRTIamb.timeAdvanceRequestAvailable (*theTime);
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
}

//------------------------------------------------------------------------------
// 6.15
void do_nextEventRequest (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { FED_TIME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
    
    myRTIamb.nextEventRequest (*theTime);
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
}

//------------------------------------------------------------------------------
// 6.16
void do_nextEventRequestAvailable (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { FED_TIME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
    
    myRTIamb.nextEventRequestAvailable (*theTime);
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
}

//------------------------------------------------------------------------------
// 6.17
void do_flushQueueRequest (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);

  enum { FED_TIME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
    
    myRTIamb.flushQueueRequest (*theTime);
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
}

//---------------------------------------------------------------------------
void do_enableAsynchronousDelivery (char**fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.enableAsynchronousDelivery ();
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
}

//---------------------------------------------------------------------------
void do_disableAsynchronousDelivery (char**fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.disableAsynchronousDelivery ();
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
}

/////////////////////////////////////////////////////////////////////////////
// Data Distribution Management
/////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------
// 7.1
void do_createRegion (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  RTI::SpaceHandle  spaceHandle;
  RTI::ULong        numExt;
  RTI::Region      *newRegion = NULL;
  
  enum { SPACE_HANDLE, NUM_EXTENTS };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
   
  try
  {
    if (nextRegionID >= MAX_NUM_REGIONS)
    {
      myStream << "Too Many regions." << '\0';
      outputMsg (buf);
      return;
    }
    
    spaceHandle = convertLongID (fields[SPACE_HANDLE]);
    if (spaceHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      spaceHandle =
        myRTIamb.getRoutingSpaceHandle (fields[SPACE_HANDLE]);

    numExt = convertLongID (fields[NUM_EXTENTS]);
    
    newRegion = myRTIamb.createRegion (spaceHandle, numExt);

    if (newRegion)
    {
      regionDB[nextRegionID] = newRegion;
      nextRegionID++;
      myStream << "Region created with ID: " << nextRegionID - 1 << '\0';
      outputMsg (buf);
    }
    else
    {
      myStream << "Create Region Failed." << endl;
      outputMsg(buf);
    }
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
}

//--------------------------------------------------------------------------
void do_setRangeLowerBound (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::ExtentIndex  extentIndex;
  RTI::DimensionHandle   dimHandle;
  RTI::ULong        theLowerBound;
  RTI::Region      *theRegion = NULL;

  enum { REGION_ID, EXTENT_INDEX, DIM_HANDLE, LOWER_BOUND };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get the region index
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    theRegion = regionDB[regionIndex];

    // Get the extent index
    extentIndex = convertLongID (fields[EXTENT_INDEX]);
    if (extentIndex == INVALID_ID)
    {
      myStream << "Error: Extent Index is Invalid" << '\0';
      outputMsg(buf);
      return;
    }

    // Get the dimension handle
    dimHandle = convertLongID (fields[DIM_HANDLE]);
    if (dimHandle == INVALID_ID)
      dimHandle =myRTIamb.getDimensionHandle (fields[DIM_HANDLE],
                                                theRegion->getSpaceHandle());
    
    
    // Get the lower bound
    theLowerBound = convertLongID(fields[LOWER_BOUND]);

    RTI::ULong lower = (RTI::ULong)
      (((double)theLowerBound - USER_MIN_EXTENT) *
       (MAX_EXTENT - MIN_EXTENT)
       / (USER_MAX_EXTENT - USER_MIN_EXTENT) + MIN_EXTENT);
    
    // Set the lower range bound
    theRegion->setRangeLowerBound(extentIndex, dimHandle, lower);
    
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
  
  return;
}

//--------------------------------------------------------------------------
void do_setRangeUpperBound (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::ExtentIndex  extentIndex;
  RTI::DimensionHandle   dimHandle;
  RTI::ULong        theUpperBound;
  RTI::Region      *theRegion = NULL;
  
  enum { REGION_ID, EXTENT_INDEX, DIM_HANDLE, UPPER_BOUND };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get the region index
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    theRegion = regionDB[regionIndex];

    // Get the extent index
    extentIndex = convertLongID (fields[EXTENT_INDEX]);
    if (extentIndex == INVALID_ID)
    {
      myStream << "Error: Extent Index is Invalid" << '\0';
      outputMsg(buf);
      return;
    }

    // Get the dimension handle
    dimHandle = convertLongID (fields[DIM_HANDLE]);
    if (dimHandle == INVALID_ID)
      dimHandle =myRTIamb.getDimensionHandle (fields[DIM_HANDLE],
                                                theRegion->getSpaceHandle());
    
    
    // Get the Upper bound
    theUpperBound = convertLongID(fields[UPPER_BOUND]);

    RTI::ULong upper = (RTI::ULong)
      (((double)theUpperBound - USER_MIN_EXTENT) *
       (MAX_EXTENT - MIN_EXTENT)
       / (USER_MAX_EXTENT - USER_MIN_EXTENT) + MIN_EXTENT);
    
    // Set the Upper range bound
    theRegion->setRangeUpperBound(extentIndex, dimHandle, upper);
    
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
  
  return;
}

//--------------------------------------------------------------------------
void do_getRangeLowerBound (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::ExtentIndex  extentIndex;
  RTI::DimensionHandle   dimHandle;
  RTI::ULong        theLowerBound;
  RTI::Region      *theRegion = NULL;
  
  enum { REGION_ID, EXTENT_INDEX, DIM_HANDLE };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get the region index
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    theRegion = regionDB[regionIndex];

    // Get the extent index
    extentIndex = convertLongID (fields[EXTENT_INDEX]);
    if (extentIndex == INVALID_ID)
    {
      myStream << "Error: Extent Index is Invalid" << '\0';
      outputMsg(buf);
      return;
    }

    // Get the dimension handle
    dimHandle = convertLongID (fields[DIM_HANDLE]);
    if (dimHandle == INVALID_ID)
      dimHandle =myRTIamb.getDimensionHandle (fields[DIM_HANDLE],
                                                theRegion->getSpaceHandle());
    
    
    // Get the lower range bound
    theLowerBound = theRegion->getRangeLowerBound(extentIndex, dimHandle);

    theLowerBound = (RTI::ULong)
      (((double)theLowerBound - MIN_EXTENT) *
       (USER_MAX_EXTENT - USER_MIN_EXTENT)
       / (MAX_EXTENT - MIN_EXTENT) + USER_MIN_EXTENT);

    myStream << "The Lower Bound: " << theLowerBound << '\0';
    outputMsg (buf);
    
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
  
  return;
}

//--------------------------------------------------------------------------
void do_getRangeUpperBound (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::ExtentIndex  extentIndex;
  RTI::DimensionHandle   dimHandle;
  RTI::ULong        theUpperBound;
  RTI::Region      *theRegion = NULL;
  
  enum { REGION_ID, EXTENT_INDEX, DIM_HANDLE };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get the region index
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    theRegion = regionDB[regionIndex];

    // Get the extent index
    extentIndex = convertLongID (fields[EXTENT_INDEX]);
    if (extentIndex == INVALID_ID)
    {
      myStream << "Error: Extent Index is Invalid" << '\0';
      outputMsg(buf);
      return;
    }

    // Get the dimension handle
    dimHandle = convertLongID (fields[DIM_HANDLE]);
    if (dimHandle == INVALID_ID)
      dimHandle =myRTIamb.getDimensionHandle (fields[DIM_HANDLE],
                                                theRegion->getSpaceHandle());
    
    // Get the Upper range bound
    theUpperBound = theRegion->getRangeUpperBound(extentIndex, dimHandle);

    theUpperBound = (RTI::ULong)
      (((double)theUpperBound - MIN_EXTENT) *
       (USER_MAX_EXTENT - USER_MIN_EXTENT)
       / (MAX_EXTENT - MIN_EXTENT) + USER_MIN_EXTENT);

    myStream << "The Upper Bound: " << theUpperBound << '\0';
    outputMsg (buf);
    
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
  
  return;
}

//--------------------------------------------------------------------------
void do_getSpaceHandleForRegion (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::SpaceHandle  theHandle;
  RTI::Region      *theRegion = NULL;
  
  enum { REGION_ID};
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get the region index
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    theRegion = regionDB[regionIndex];

    theHandle = theRegion->getSpaceHandle();

    myStream << "The Space Handle: " << theHandle << '\0';
    outputMsg (buf);
    
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
  
  return;

}

//--------------------------------------------------------------------------
void do_getNumberOfExtents (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::ULong        theNumberOfExtents;
  RTI::Region      *theRegion = NULL;
  
  enum { REGION_ID};
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get the region index
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    theRegion = regionDB[regionIndex];

    theNumberOfExtents = theRegion->getNumberOfExtents();

    myStream << "The Number of Extents: " << theNumberOfExtents << '\0';
    outputMsg (buf);
    
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
  
  return;
}

//--------------------------------------------------------------------------
void do_getRangeLowerBoundNotificationLimit (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::ExtentIndex  extentIndex;
  RTI::DimensionHandle   dimHandle;
  RTI::ULong        theLowerBound;
  RTI::Region      *theRegion = NULL;
  
  enum { REGION_ID, EXTENT_INDEX, DIM_HANDLE };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get the region index
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    theRegion = regionDB[regionIndex];

    // Get the extent index
    extentIndex = convertLongID (fields[EXTENT_INDEX]);
    if (extentIndex == INVALID_ID)
    {
      myStream << "Error: Extent Index is Invalid" << '\0';
      outputMsg(buf);
      return;
    }

    // Get the dimension handle
    dimHandle = convertLongID (fields[DIM_HANDLE]);
    if (dimHandle == INVALID_ID)
      dimHandle =myRTIamb.getDimensionHandle (fields[DIM_HANDLE],
                                                theRegion->getSpaceHandle());
    
    
    // Get the lower range bound
    theLowerBound = theRegion->getRangeLowerBoundNotificationLimit(
      extentIndex, dimHandle);
    
    myStream << "The Lower Bound Notification Limit: " << theLowerBound << '\0';
    outputMsg (buf);
    
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
  
  return;
}

//--------------------------------------------------------------------------
void do_getRangeUpperBoundNotificationLimit (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::ExtentIndex  extentIndex;
  RTI::DimensionHandle   dimHandle;
  RTI::ULong        theUpperBound;
  RTI::Region      *theRegion = NULL;
  
  enum { REGION_ID, EXTENT_INDEX, DIM_HANDLE };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get the region index
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    theRegion = regionDB[regionIndex];

    // Get the extent index
    extentIndex = convertLongID (fields[EXTENT_INDEX]);
    if (extentIndex == INVALID_ID)
    {
      myStream << "Error: Extent Index is Invalid" << '\0';
      outputMsg(buf);
      return;
    }

    // Get the dimension handle
    dimHandle = convertLongID (fields[DIM_HANDLE]);
    if (dimHandle == INVALID_ID)
      dimHandle =myRTIamb.getDimensionHandle (fields[DIM_HANDLE],
                                                theRegion->getSpaceHandle());
    
    // Get the Upper range bound
    theUpperBound = theRegion->getRangeUpperBoundNotificationLimit(
      extentIndex, dimHandle);
    
    myStream << "The Upper Bound Notification Limit: " << theUpperBound << '\0';
    outputMsg (buf);
    
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
  
  return;
}

//--------------------------------------------------------------------------
// 7.2
void do_notifyRegionModification (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  int               regionIndex;
  RTI::Region      *newRegion = NULL;
  
  enum { REGION_ID };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
   
  try
  {
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    myRTIamb.notifyAboutRegionModification ( *regionDB[regionIndex]);    
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
}

//--------------------------------------------------------------------------
// 7.3
void do_deleteRegion (char **fields, int fieldCnt)
{
  char       buf[200];
  ostrstream myStream (buf, 200);
  int        regionIndex;
  
  enum { REGION_ID };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    
    myRTIamb.deleteRegion (regionDB[regionIndex]);
    regionDB[regionIndex] = NULL;
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

}

//--------------------------------------------------------------------------
// 7.4
void do_registerObjectWithRegion1 (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  RTI::ObjectClassHandle   objClassHandle;
  RTI::AttributeHandle     attrList[5];
  RTI::Region             *regList[5];
  RTI::ObjectHandle        theObjectHandle;
  
  enum { OBJ_CLASS_HANDLE, OBJ_NAME, ATTR_LIST, REG_LIST, NUM_HANDLES };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);

    // Build attribute handle array
    buildAttributeList (fields[ATTR_LIST],
                        attrList, atoi(fields[NUM_HANDLES]),
                        fields[OBJ_CLASS_HANDLE], objClassHandle);

    // Build region handle array
    RTI::Boolean success = buildRegionList (fields[REG_LIST],
                                            regList,
                                            atoi(fields[NUM_HANDLES]));
    
    if(!success)
    {
      myStream << "Error: Region List is bad." << '\0';
      outputMsg(buf);
      return;
    }
    
    theObjectHandle = myRTIamb.registerObjectInstanceWithRegion (
      objClassHandle,
      fields[OBJ_NAME],
      attrList,
      regList,
      (RTI::ULong)atoi(fields[NUM_HANDLES]));

    // Print object Handle
    myStream << "Register Object With Handle: " << theObjectHandle  << '\0';
    outputMsg (buf);
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
}

//--------------------------------------------------------------------------
// 7.4 extended 
void do_registerObjectWithRegion2 (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  RTI::ObjectClassHandle   objClassHandle;
  RTI::AttributeHandle     attrList[5];
  RTI::Region             *regList[5];
  RTI::ObjectHandle        theObjectHandle;
  
  enum { OBJ_CLASS_HANDLE, ATTR_LIST, REG_LIST, NUM_HANDLES };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    // Build attribute handle array
    buildAttributeList (fields[ATTR_LIST],
                        attrList, atoi(fields[NUM_HANDLES]),
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    // Build region handle array
    RTI::Boolean success = buildRegionList (fields[REG_LIST],
                                            regList,
                                            atoi(fields[NUM_HANDLES]));
    
    if(!success)
    {
      myStream << "Error: Region List is bad." << '\0';
      outputMsg(buf);
      return;
    }
    
    theObjectHandle = myRTIamb.registerObjectInstanceWithRegion (
      objClassHandle,
      attrList,
      regList,
      (RTI::ULong)atoi(fields[NUM_HANDLES]));
    
    myStream << "Registered Object with Handle: " << theObjectHandle  << '\0';
    outputMsg (buf);
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
}

//--------------------------------------------------------------------------
// 7.5
void do_associateRegionForUpdates (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i;
  int                      attrCnt;
  RTI::ObjectHandle        objHandle;
  RTI::ObjectClassHandle   objClassHandle;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *theAttrs;
  
  enum { REGION_ID, OBJECT_HANDLE, ATTR_LIST };

  try
  {
    objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);

    int regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    
    objClassHandle = myRTIamb.getObjectClass(objHandle);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    theAttrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      theAttrs->add (attrList[i]);
    
    myRTIamb.associateRegionForUpdates (*regionDB[regionIndex],
                                        objHandle, *theAttrs);
    
    delete theAttrs;
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
}

//--------------------------------------------------------------------------
// 7.6
void do_unassociateRegionForUpdates (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  RTI::ObjectHandle        objHandle;
  
  enum { REGION_ID, OBJECT_HANDLE};

  try
  {
    objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);

    int regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    
    myRTIamb.unassociateRegionForUpdates (*regionDB[regionIndex],
                                          objHandle);
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
}

//--------------------------------------------------------------------------
// 7.7
void do_subscribeObjectClassAttributesWithRegion (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  RTI::ObjectClassHandle   objClassHandle;
  int                      attrCnt,i;
  RTI::AttributeHandle     attrList[20];
  RTI::AttributeHandleSet *subAttrs;
  
  enum { OBJ_CLASS_HANDLE, ATTR_LIST, REGION_ID };
  
  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    attrCnt  = countListElements (fields[ATTR_LIST]);
    subAttrs = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      subAttrs->add (attrList[i]);

    int regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    
    myRTIamb.subscribeObjectClassAttributesWithRegion (objClassHandle,
                                                       *regionDB[regionIndex],
                                                       *subAttrs);
    delete subAttrs;
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
}

//--------------------------------------------------------------------------
// 7.8
void do_unsubscribeObjectClassWithRegion (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ObjectClassHandle  objClassHandle;
  
  enum { OBJ_CLASS_HANDLE, REGION_ID };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);

    int regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    
    myRTIamb.unsubscribeObjectClassWithRegion (objClassHandle,
                                               *regionDB[regionIndex]);
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
}

//--------------------------------------------------------------------------
// 7.9
void do_subscribeInteractionClassWithRegion (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;
  
  enum { INTERACTION_CLASS_HANDLE, REGION_ID };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us an interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);

    int regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    
    myRTIamb.subscribeInteractionClassWithRegion (interactionClassHandle,
                                                  *regionDB[regionIndex]);
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
}

//--------------------------------------------------------------------------
// 7.10
void do_unsubscribeInteractionClassWithRegion (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;
  
  enum { INTERACTION_CLASS_HANDLE, REGION_ID };
  
  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us an interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);

    int regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    
    myRTIamb.unsubscribeInteractionClassWithRegion (interactionClassHandle,
                                                    *regionDB[regionIndex]);  
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
}

//--------------------------------------------------------------------------
// 7.11
void do_sendInteractionWithRegion (char **fields, int fieldCnt)
{
  char                              buf[200];
  ostrstream                        myStream (buf, 200);
  char                              parmString[256];
  int                               parmStringCnt;
  int                               i, parmCnt;
  RTI::InteractionClassHandle       interactionClassHandle = 0;
  RTI::ParameterHandle              parmList[4];
  RTI::ParameterHandleValuePairSet *parmVals;
  RTI::EventRetractionHandle        theHandle;
  RTI::Boolean                      first = RTI::RTI_TRUE;
  long                              signedLongVal;
  short                             signedShortVal;
  float                             floatVal;
  double                            doubleVal;
  unsigned char                     hexVal[500];
  char                              hexChar[5];
  char                              hexString[1000];
  unsigned long                     hexStringLength;
  char                              formatCode;
  
  enum { INTERACTION_CLASS_HANDLE, REGION_ID, FED_TIME, USER_TAG,
         PARM_NAME1, PARM_VAL1, PARM_NAME2, PARM_VAL2, 
         PARM_NAME3, PARM_VAL3, PARM_NAME4, PARM_VAL4 };

  // In the code that follows, you will see expressions that look like:
  //    PARM_NAME1 + i * 2
  //    PARM_VAL1  + i * 2
  //
  //  The incoming fields are placed into the fields array in the order
  //  given by the enumeration above.  When we want to iterate through
  //  the attribute names, we use the expression:  PARM_NAME1 + i * 2.
  //  As i changes, we will retrieve PARM_NAME1, PARM_NAME2, etc.  This
  //  also applies to the attribute values with PARM_VAL1, PARM_VAL2, etc.

  try
  {
    // Convert the interaction Class Handle.
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us a interaction name.
      //  Convert it to a handle.
      interactionClassHandle =
 myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);

    // Convert the individual parameter names into a comma-delimited
    // string of parameter names.  This will be needed by buildAttributeList.
    parmString[0] = '\0';
    parmStringCnt = 0;
    for (parmCnt = 0; parmCnt < 4; parmCnt++)
    {
      if (strcmp (fields[PARM_NAME1 + parmCnt * 2], "") != 0)
      {
 //  Insert a comma before every parameter, except the first.
 if (!first)
   strcat (parmString, ",");
 else
   first = RTI::RTI_FALSE;

        //  Insert the parameter into the list.
 strcat (parmString, fields[PARM_NAME1 + parmCnt * 2]);
 parmStringCnt++;
      }
      else
 break;
    }

    // Convert the parameter names to parameter handles, as necessary.
    buildParameterList (parmString, parmList, parmStringCnt,
   fields[INTERACTION_CLASS_HANDLE],
   interactionClassHandle);

    // Now build the Parameter Handle/Value Pair Set.
    parmVals = RTI::ParameterSetFactory::create (parmStringCnt);
    
    for (i = 0; i < parmStringCnt; i++)
    {
      if(*(fields[PARM_VAL1 + i * 2]) == '^')
      {
        formatCode = tolower(*(fields[PARM_VAL1 + i * 2] + 1));
        
        if(formatCode == 'l' &&
           *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
          
        {
          signedLongVal = atoi(fields[PARM_VAL1 + i * 2] + 3);
          parmVals->add (parmList[i],
                         (char *)&signedLongVal,
                         sizeof(signedLongVal));
        }
        else if (formatCode == 's' &&
                 *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
        {
          signedShortVal = (short)atoi(fields[PARM_VAL1 + i * 2] + 3);
          parmVals->add (parmList[i],
                         (char *)&signedShortVal,
                         sizeof(signedShortVal));
        }
        else if (formatCode == 'f' &&
                 *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
        {
          floatVal = atof(fields[PARM_VAL1 + i * 2] + 3);
          parmVals->add (parmList[i],
                         (char *)&floatVal,
                         sizeof(floatVal));
        }
        else if (formatCode == 'd' &&
                 *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
        {
          doubleVal = (double)atof(fields[PARM_VAL1 + i * 2] + 3);
          parmVals->add (parmList[i],
                         (char *)&doubleVal,
                         sizeof(doubleVal));
        }
        else if (formatCode == 'h' &&
                 *(fields[PARM_VAL1 + i * 2] + 2) == ' ' )
        {
          strcpy(hexString, fields[PARM_VAL1 + i * 2] + 3);
          hexStringLength = strlen(hexString);
          
          if (hexStringLength % 2)
          {
            strcat(hexString, "0");
            hexStringLength = strlen(hexString);
          }
          
          for (int index = 0; index < hexStringLength; index = index + 2)
          {
            hexChar[0] = hexString[index];
            hexChar[1] = hexString[index + 1];
            hexChar[2] = '\0';
            
            hexVal[index/2] = (unsigned char)
              strtol(hexChar, (char **)NULL, 16);
          }
          parmVals->add (parmList[i],
                         (char *)hexVal,
                         hexStringLength/2);
          
        }
        else
        {
          parmVals->add (parmList[i],
                         fields[PARM_VAL1 + i * 2],
                         strlen (fields[PARM_VAL1 + i * 2]) + 1);
        }
      }
      else
      {
        parmVals->add (parmList[i],
                       fields[PARM_VAL1 + i * 2],
                       strlen (fields[PARM_VAL1 + i * 2]) + 1);
      }
    }

    int regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }

    if (strlen(fields[FED_TIME]) > 0) {
      // Time specified

      RTIfedTime *theTime = new RTIfedTime(atof (fields[FED_TIME]));
      
      // Finally, call the Send Interaction RTI function.
      theHandle = myRTIamb.sendInteractionWithRegion (
        interactionClassHandle,
        *parmVals,
        *theTime,
        (char *)fields[USER_TAG],
        *regionDB[regionIndex]);
      
      myStream << "Send Interaction With Region Event Retraction Handle: \n" <<
        "  Serial Number:  "   << theHandle.theSerialNumber <<
        "  Federate Handle:  " << theHandle.sendingFederate << '\0';
      outputMsg (buf);
      
    }
    else { 
      // Finally, call the Send Interaction RTI function.
      myRTIamb.sendInteractionWithRegion (
        interactionClassHandle,
        *parmVals,
        (char *)fields[USER_TAG],
        *regionDB[regionIndex]);
    }

    delete parmVals;
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }
}

//--------------------------------------------------------------------------
// 7.12
void do_requestClassAttributeValueUpdateWithRegion (char **fields, int fieldCnt)
{
  char                     buf[200];
  ostrstream               myStream (buf, 200);
  int                      i, attrCnt;
  RTI::ObjectClassHandle   objClassHandle = 0;
  RTI::AttributeHandle     attrList[30];
  RTI::AttributeHandleSet *attrs;
  
  enum { OBJ_CLASS_HANDLE, ATTR_LIST, REGION_ID };
  
  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us an object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);
    
    // Build the Attribute set.
    attrCnt = countListElements (fields[ATTR_LIST]);
    attrs   = RTI::AttributeHandleSetFactory::create (attrCnt);
    
    buildAttributeList (fields[ATTR_LIST],
                        attrList, attrCnt,
                        fields[OBJ_CLASS_HANDLE], objClassHandle);
    
    for (i = 0; i < attrCnt; i++)
      attrs->add (attrList[i]);

    int regionIndex = convertLongID (fields[REGION_ID]);
    if (regionIndex >= nextRegionID || regionDB[regionIndex] == NULL)
    {
      myStream << "Error: Region Not Known" << '\0';
      outputMsg(buf);
      return;
    }
    
    myRTIamb.requestClassAttributeValueUpdateWithRegion (
      objClassHandle,*attrs,*regionDB[regionIndex]);
    
    delete attrs;
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
}

//////////////////////////////////////////////////////////////////////////////
// Support Services
//////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
// 8.1
void do_getObjectClassHandle (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  RTI::ObjectClassHandle theHandle;

  enum { OBJ_CLASS_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theHandle = myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_NAME]);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Object Class Handle for Object Class Name '" <<
              fields[OBJ_CLASS_NAME] << "' = " << theHandle << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.2
void do_getObjectClassName (char **fields, int fieldCnt)
{
  char             buf[200];
  ostrstream       myStream (buf, 200);
  char            *theName;

  enum { OBJ_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theName = myRTIamb.getObjectClassName ((RTI::ObjectClassHandle)
                                           atoi (fields[OBJ_CLASS_HANDLE]));
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Object Class Name for Object Class Handle '" <<
              fields[OBJ_CLASS_HANDLE] << "' = " << theName << '\0';
  outputMsg (buf);

  delete [] theName;
}

//---------------------------------------------------------------------------
// 8.3
void do_getAttributeHandle (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::AttributeHandle    theHandle;
  RTI::ObjectClassHandle  objClassHandle;

  enum { ATTR_NAME, OBJ_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us a object class name.  Convert it to a handle.
      objClassHandle =
 myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);

    theHandle = myRTIamb.getAttributeHandle (fields[ATTR_NAME],
          objClassHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Attribute Handle for Attribute '" <<
              fields[ATTR_NAME] << "', Object Class Handle '" <<
              objClassHandle    << "' = " << theHandle  << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.4
void do_getAttributeName (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ObjectClassHandle  objClassHandle;
  char                   *theName;

  enum { ATTR_HANDLE, OBJ_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    objClassHandle = convertLongID (fields[OBJ_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us a object class name.  Convert it to a handle.
      objClassHandle =
 myRTIamb.getObjectClassHandle (fields[OBJ_CLASS_HANDLE]);

    theName = myRTIamb.getAttributeName (
      (RTI::AttributeHandle)atoi (fields[ATTR_HANDLE]),
      objClassHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Attribute Name for Attribute Handle '" <<
              fields[ATTR_HANDLE] << "', Object Class Handle '" <<
       objClassHandle << "' = " << theName << '\0';
  outputMsg (buf);

  delete [] theName;
}

//---------------------------------------------------------------------------
// 8.5
void do_getInteractionClassHandle (char **fields, int fieldCnt)
{
  char                   buf[200];
  ostrstream             myStream (buf, 200);
  RTI::InteractionClassHandle theHandle;

  enum { INTERACTION_CLASS_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theHandle =
      myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_NAME]);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Interaction Class Handle for Interaction Class Name '" <<
              fields[INTERACTION_CLASS_NAME] << "' = " << theHandle << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.6
void do_getInteractionClassName (char **fields, int fieldCnt)
{
  char                  buf[200];
  ostrstream            myStream (buf, 200);
  char                 *theName;

  enum { INTERACTION_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theName =
      myRTIamb.getInteractionClassName (
    (RTI::InteractionClassHandle)
    atoi (fields[INTERACTION_CLASS_HANDLE]));
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Interaction Class Name for Interaction Class Handle '" <<
              fields[INTERACTION_CLASS_HANDLE] << "' = " << theName << '\0';
  outputMsg (buf);

  delete [] theName;
}

//---------------------------------------------------------------------------
// 8.7
void do_getParameterHandle (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::ParameterHandle         theHandle;
  RTI::InteractionClassHandle  interactionClassHandle;

  enum { PARM_NAME, INTERACTION_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us a interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
 myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);

    theHandle = myRTIamb.getParameterHandle (fields[PARM_NAME],
          interactionClassHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Parameter Handle for Parameter '" <<
              fields[PARM_NAME] << "', Interaction Handle '" <<
       interactionClassHandle << "' = " << theHandle << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.8
void do_getParameterName (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::InteractionClassHandle  interactionClassHandle;
  char                   *theName;

  enum { PARM_HANDLE, INTERACTION_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    interactionClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (interactionClassHandle == INVALID_ID)
      //  Ok, the user gave us a interaction class name.
      //  Convert it to a handle.
      interactionClassHandle =
 myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);

    theName = myRTIamb.getParameterName (
      (RTI::ParameterHandle)atoi (fields[PARM_HANDLE]),
      interactionClassHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Parameter Name for Parameter Handle '" <<
              fields[PARM_HANDLE] << "', Interaction Class Handle '" <<
       interactionClassHandle << "' = " << theName << '\0';
  outputMsg (buf);

  delete [] theName;
}

//---------------------------------------------------------------------------
// 8.9
void do_getObjectHandle (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  RTI::ObjectHandle      theHandle;

  enum { OBJ_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theHandle = myRTIamb.getObjectInstanceHandle (fields[OBJ_NAME]);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Object Handle for Object Name '" <<
              fields[OBJ_NAME] << "' = " << theHandle << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.10
void do_getObjectName (char **fields, int fieldCnt)
{
  char             buf[200];
  ostrstream       myStream (buf, 200);
  char            *theName;

  enum { OBJECT_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theName = myRTIamb.getObjectInstanceName (
      (RTI::ObjectHandle)atoi (fields[OBJECT_HANDLE]));
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Object Name for Object  Handle '" <<
              fields[OBJECT_HANDLE] << "' = " << theName << '\0';
  outputMsg (buf);

  delete [] theName;
}

//---------------------------------------------------------------------------
// 8.11
void do_getSpaceHandle (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);
  RTI::SpaceHandle  theHandle;

  enum { SPACE_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theHandle = myRTIamb.getRoutingSpaceHandle (fields[SPACE_NAME]);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Space Handle for Space Name '" <<
              fields[SPACE_NAME] << "' = " << theHandle << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.13
void do_getDimensionHandle (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::DimensionHandle    theHandle;
  RTI::SpaceHandle        spaceHandle;

  enum { DIM_NAME, SPACE_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    spaceHandle = convertLongID (fields[SPACE_HANDLE]);
    if (spaceHandle == INVALID_ID)
      //  Ok, the user gave us a space name.  Convert it to a handle.
      spaceHandle =
        myRTIamb.getRoutingSpaceHandle (fields[SPACE_HANDLE]);
    
    theHandle = myRTIamb.getDimensionHandle (fields[DIM_NAME],spaceHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }
  
  myStream << "Dimension Handle for Dimension '" <<
              fields[DIM_NAME] << "', Space Handle '" <<
              spaceHandle    << "' = " << theHandle  << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.12
void do_getSpaceName (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);
  char        *theName;

  enum { SPACE_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theName = myRTIamb.getRoutingSpaceName ((RTI::SpaceHandle)
         atoi (fields[SPACE_HANDLE]));
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Space Name for Space Handle '" <<
              fields[SPACE_HANDLE] << "' = " << theName << '\0';
  outputMsg (buf);

  delete [] theName;
}

//---------------------------------------------------------------------------
// 8.14
void do_getDimensionName (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::SpaceHandle        spaceHandle;
  char                   *theName;

  enum { DIM_HANDLE, SPACE_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    spaceHandle = convertLongID (fields[SPACE_HANDLE]);
    if (spaceHandle == INVALID_ID)
      //  Ok, the user gave us a space name.  Convert it to a handle.
      spaceHandle =
        myRTIamb.getRoutingSpaceHandle (fields[SPACE_HANDLE]);
    
    theName = myRTIamb.getDimensionName ((RTI::DimensionHandle)
                                         atoi (fields[DIM_HANDLE]),
                                         spaceHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }
  
  myStream << "Dimension Name for Dimension Handle '" <<
    fields[DIM_HANDLE] << "', Space Handle '" <<
    spaceHandle << "' = " << theName << '\0';
  outputMsg (buf);
  
  delete [] theName;
}

//---------------------------------------------------------------------------
//8.?
void do_getTransportationHandle (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  RTI::TransportationHandle theHandle;

  enum { TRANSPORT_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theHandle = myRTIamb.getTransportationHandle (fields[TRANSPORT_NAME]);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Transportation Handle for Transportation Name '" <<
              fields[TRANSPORT_NAME] << "' = " << theHandle << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.?
void do_getTransportationName (char **fields, int fieldCnt)
{
  char             buf[200];
  ostrstream       myStream (buf, 200);
  char            *theName;

  enum { TRANSPORT_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theName = myRTIamb.getTransportationName ((RTI::TransportationHandle)
                                              atoi (fields[TRANSPORT_HANDLE]));
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Transportation Name for Transportation Handle '" <<
              fields[TRANSPORT_HANDLE] << "' = " << theName << '\0';
  outputMsg (buf);

  delete [] theName;
}

//---------------------------------------------------------------------------
//8.?
void do_getOrderingHandle (char **fields, int fieldCnt)
{
  char              buf[200];
  ostrstream        myStream (buf, 200);
  RTI::OrderingHandle theHandle;

  enum { ORDERING_NAME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theHandle = myRTIamb.getOrderingHandle (fields[ORDERING_NAME]);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Ordering Handle for Ordering Name '" <<
              fields[ORDERING_NAME] << "' = " << theHandle << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.?
void do_getOrderingName (char **fields, int fieldCnt)
{
  char             buf[200];
  ostrstream       myStream (buf, 200);
  char            *theName;

  enum { ORDERING_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    theName = myRTIamb.getOrderingName ((RTI::OrderingHandle)
                                        atoi (fields[ORDERING_HANDLE]));
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Ordering Name for Ordering Handle '" <<
              fields[ORDERING_HANDLE] << "' = " << theName << '\0';
  outputMsg (buf);

  delete [] theName;
}

//---------------------------------------------------------------------------
// 8.15
void do_getAttributeSpaceHandle (char **fields, int fieldCnt)
{
  char                    buf[200];
  ostrstream              myStream (buf, 200);
  RTI::SpaceHandle        theHandle;
  RTI::AttributeHandle    attrHandle;
  RTI::ObjectClassHandle  objClassHandle;

  enum { ATTR_HANDLE, OBJECT_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    objClassHandle = convertLongID (fields[OBJECT_CLASS_HANDLE]);
    if (objClassHandle == INVALID_ID)
      //  Ok, the user gave us a object class name.  Convert it to a handle.
      objClassHandle =
        myRTIamb.getObjectClassHandle (fields[OBJECT_CLASS_HANDLE]);

    attrHandle = convertLongID (fields[ATTR_HANDLE]);
    if (attrHandle == INVALID_ID)
      //  Ok, the user gave us a attribute name.  Convert it to a handle.
      attrHandle =
        myRTIamb.getAttributeHandle (fields[ATTR_HANDLE], objClassHandle);
    
    theHandle = myRTIamb.getAttributeRoutingSpaceHandle (attrHandle, objClassHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Space Handle for attribute '" <<
    attrHandle << "', Object Class Handle '" <<
    objClassHandle    << "' = " << theHandle  << '\0';
  outputMsg (buf);
}

//---------------------------------------------------------------------------
// 8.16
void do_getObjectClass (char **fields, int fieldCnt)
{
  char             buf[200];
  ostrstream       myStream (buf, 200);
  RTI::ObjectClassHandle   theClass;

  enum { OBJECT_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;
  
  try
  {
    // Get object handle
    RTI::ObjectHandle objHandle = convertLongID (fields[OBJECT_HANDLE]);
    if (objHandle == INVALID_ID)
      //  Ok, the user gave us a object name.  Convert it to a handle.
      objHandle =
        myRTIamb.getObjectInstanceHandle (fields[OBJECT_HANDLE]);

    theClass = myRTIamb.getObjectClass (objHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Object Class Handle for Object '" <<
              fields[OBJECT_HANDLE] << "' = " << theClass << '\0';
  outputMsg (buf);

}

//---------------------------------------------------------------------------
// 8.17
void do_getInteractionSpaceHandle (char **fields, int fieldCnt)
{
  char                         buf[200];
  ostrstream                   myStream (buf, 200);
  RTI::SpaceHandle             theHandle;
  RTI::InteractionClassHandle  intClassHandle;

  enum { INTERACTION_CLASS_HANDLE };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    intClassHandle = convertLongID (fields[INTERACTION_CLASS_HANDLE]);
    if (intClassHandle == INVALID_ID)
      //  Ok, the user gave us a interaction class name.  Convert it to a handle.
      intClassHandle =
        myRTIamb.getInteractionClassHandle (fields[INTERACTION_CLASS_HANDLE]);
    
    theHandle = myRTIamb.getInteractionRoutingSpaceHandle (intClassHandle);
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  myStream << "Space Handle for interaction '" <<
    intClassHandle    << "' = " << theHandle  << '\0';
  outputMsg (buf);
}

//------------------------------------------------------------------------------
// 8.18
void do_enableClassRelevanceAdvisory (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.enableClassRelevanceAdvisorySwitch ();
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
}

//------------------------------------------------------------------------------
// 8.19
void do_disableClassRelevanceAdvisory (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.disableClassRelevanceAdvisorySwitch ();
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
}

//------------------------------------------------------------------------------
// 8.20
void do_enableAttributeRelevanceAdvisory (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.enableAttributeRelevanceAdvisorySwitch ();
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
}

//------------------------------------------------------------------------------
// 8.21
void do_disableAttributeRelevanceAdvisory (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.disableAttributeRelevanceAdvisorySwitch ();
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
}

//------------------------------------------------------------------------------
// 8.22
void do_enableAttributeScopeAdvisory (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.enableAttributeScopeAdvisorySwitch ();
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
}

//------------------------------------------------------------------------------
// 8.23
void do_disableAttributeScopeAdvisory (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.disableAttributeScopeAdvisorySwitch ();
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
}

//------------------------------------------------------------------------------
// 8.24
void do_enableInteractionRelevanceAdvisory (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.enableInteractionRelevanceAdvisorySwitch ();
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
}

//------------------------------------------------------------------------------
// 8.25
void do_disableInteractionRelevanceAdvisory (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);

  try
  {
    myRTIamb.disableInteractionRelevanceAdvisorySwitch ();
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
}

//---------------------------------------------------------------------------
// 8.19
void do_tick1 (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);
  RTI::Boolean more;

  try
  {
    more = myRTIamb.tick ();
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  if (more)
    outputMsg ("More incoming messages are waiting.");
}

void do_tick2 (char **fields, int fieldCnt)
{
  char         buf[200];
  ostrstream   myStream (buf, 200);
  RTI::Boolean more;

  enum { MIN_TIME, MAX_TIME };

  if (!checkRequiredParameters (fields, fieldCnt))
    return;

  try
  {
    more = myRTIamb.tick ((RTI::TickTime)(atof (fields[MIN_TIME])),
     (RTI::TickTime)(atof (fields[MAX_TIME])));
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Error:  " << &ex << '\0';
    outputMsg (buf);
    return;
  }
  catch (...)
  {
    outputMsg ("Caught unknown exception");
    return;
  }

  if (more)
    outputMsg ("More incoming messages are waiting.");
}


// Because this function is called from the genericPrompDialog just
// like the other functions of this file, it must use the same arguments.
// However, this function behaves quite different from the others.  The
// 'fields' argument will always contain the time interval, while the
// 'fieldCnt' argument is really used to provide the autoTick State.
void do_autoTick (char **fields, int fieldCnt)
{
  char                  buf[250];
  ostrstream            myStream (buf, 250);
  static RTI::TickTime  tickMinTime;
  static RTI::TickTime  tickMaxTime;
  unsigned short        timeIntervalField;

  static RTI::Boolean autoTickEnabled;
  enum { MIN_TIME, MAX_TIME };

  AutoTickStates autoTickState = (AutoTickStates)fieldCnt;

  switch (autoTickState)
  {
  case TICK_WITH_ARGS:
    tickMinTime = atof (fields[MIN_TIME]);
    tickMaxTime = atof (fields[MAX_TIME]);

    timeIntervalField = 2;
    autoTickEnabled = RTI::RTI_TRUE;
    break;
  case TICK_NO_ARGS:
    tickMinTime = 0.0;
    tickMaxTime = 0.0;

    timeIntervalField = 0; 
    autoTickEnabled = RTI::RTI_TRUE;
    break;
  case CONTINUE:
    if (!autoTickEnabled)
      return;
    timeIntervalField = 0;
    break;
  case STOP:
    autoTickEnabled = RTI::RTI_FALSE;
    return;
    break;
  default:
    outputMsg ("AutoTick:  Unexpected AutoTick state");
    return;
    break;
  }
  
  try
  {
    if ((tickMinTime != 0.0) && (tickMaxTime != 0.0))
      myRTIamb.tick (tickMinTime, tickMaxTime);
    else 
      myRTIamb.tick ();
  }
  catch (RTI::Exception& ex)
  {
    myStream << "Autotick: " << &ex << '\0';
    outputMsg (buf);

    //autoTickEnabled = RTI::RTI_FALSE;
    //return;
  }
  catch (...)
  { 
    outputMsg ("AutoTick:  Could not register autoTick."
        "  Caught unknown exception");

    autoTickEnabled = RTI::RTI_FALSE;
    return;
  }

  if (autoTickState != CONTINUE)
    autoTickEngaged ();
  scheduleNextTick (fields[timeIntervalField]);
}

/***********************************************************************

    AUXILLARY FUNCTIONS

***********************************************************************/

unsigned long convertLongID (const char* const s)
{
  char          *b;
  unsigned long  a;

  a = strtoul (s, &b, 10);

  if ((a == 0) && (b - s == 0))
    return INVALID_ID;
  else
    return a;
}

unsigned short convertShortID (const char* const s)
{
  char          *b;
  unsigned long  a;

  a = strtoul (s, &b, 10);

  if ((a == 0) && (b - s == 0))
    return INVALID_ID;
  else
    return (a > INVALID_ID) ? INVALID_ID : a;
}


static RTI::Boolean checkRequiredParameters (char      **fields,
          const int   fieldCnt)
{
  RTI::Boolean paramsOk = RTI::RTI_TRUE;

  for (int i = 0; i < fieldCnt; i++)
  {
    if (strcmp (fields[i], "") == 0)
    {
      outputMsg ("Error:  Not all required fields provided.");
      paramsOk = RTI::RTI_FALSE;
      break;
    }
  }

  return paramsOk;
}


// Count the number of comma-delimited elements in a string
int countListElements (const char* const listStr)
{
  int i;
  int count = 0;
  int strLen;

  if (listStr[0] == '\0')
    return 0;

  strLen = strlen (listStr);

  // Count the number of commas
  for (i = 0; i < strLen; i++)
    if (listStr[i] == ',')
      count++;

  return count + 1;
}


// Build an array of Attribute Handles from a comma-delimited list
// of attributes (in either Handle or Name form).
static void buildAttributeList (const char* const       attrString,
                                RTI::AttributeHandle    attrList[],
                                const int               attrListLength,
                                char*                   objClassHandleStr,
                                RTI::ObjectClassHandle& objClassHandle)
{
  char         *localAttrStr;
  char         *attrNameStr;
  int           index;
  RTI::Boolean  haveObjClassHandle = RTI::RTI_FALSE;
  RTI::Boolean  first              = RTI::RTI_TRUE;
  
  
  localAttrStr = new char[strlen (attrString) + 1];
  strcpy (localAttrStr, attrString);
  
  
  // Tokenize the attribute list string.  Place each attribute handle
  // in the attrList array.

  for (index = 0; index < attrListLength; index++)
  {
    // Get the token.
    if (first)
    {
      attrNameStr = strtok (localAttrStr, ",");
      first = RTI::RTI_FALSE;
    }
    else
      attrNameStr = strtok (NULL, ",");
    
    // If there are no more tokens, exit the loop.
    if (attrNameStr == NULL)
      break;
    
    attrList[index] = convertLongID (attrNameStr);
    if (attrList[index] == INVALID_ID)
    {
      //  Ok, the user gave us an attribute name.  Convert it to a handle.
      
      //  If we haven't checked the Object Class Handle yet, do so now.
      if (objClassHandle != 0)
        haveObjClassHandle = RTI::RTI_TRUE;
      
      if (!haveObjClassHandle)
      {
        objClassHandle = convertLongID (objClassHandleStr);
        if (objClassHandle == INVALID_ID)
          //  Ok, the user gave us a object class name.
          //  Convert it to a handle.
          objClassHandle = myRTIamb.getObjectClassHandle (objClassHandleStr);
        
        haveObjClassHandle = RTI::RTI_TRUE;
      }
      
      attrList[index] = myRTIamb.getAttributeHandle (attrNameStr,
                                                     objClassHandle);
    }
  }   // end for
  
  delete [] localAttrStr;
}


// Build an array of Attribute Handles from a comma-delimited list
// of attributes (in either Handle or Name form).
// This one is overriden
static void buildAttributeList (const char* const       attrString,
                                RTI::AttributeHandle    attrList[],
                                const int               attrListLength,
                                RTI::ObjectClassHandle& objClassHandle)
{
  char         *localAttrStr;
  char         *attrNameStr;
  int           index;
  RTI::Boolean  haveObjClassHandle = RTI::RTI_FALSE;
  RTI::Boolean  first              = RTI::RTI_TRUE;
  
  
  localAttrStr = new char[strlen (attrString) + 1];
  strcpy (localAttrStr, attrString);
  
  
  // Tokenize the attribute list string.  Place each attribute handle
  // in the attrList array.

  for (index = 0; index < attrListLength; index++)
  {
    // Get the token.
    if (first)
    {
      attrNameStr = strtok (localAttrStr, ",");
      first = RTI::RTI_FALSE;
    }
    else
      attrNameStr = strtok (NULL, ",");
    
    // If there are no more tokens, exit the loop.
    if (attrNameStr == NULL)
      break;
    
    attrList[index] = convertLongID (attrNameStr);
    if (attrList[index] == INVALID_ID)
    {
      //  Ok, the user gave us an attribute name.  Convert it to a handle.      
      attrList[index] = myRTIamb.getAttributeHandle (attrNameStr,
                                                     objClassHandle);
    }
  }   // end for
  
  delete [] localAttrStr;
}


// Build an array of Parameter Handles from a comma-delimited list
// of parameters (in either Handle or Name form).
static void buildParameterList (
                        const char* const       parmString,
   RTI::ParameterHandle    parmList[],
   const int               parmListLength,
   char*                   interactionClassHandleStr,
   RTI::InteractionClassHandle& interactionClassHandle)
{
  char         *localParmStr;
  char         *parmNameStr;
  int           index;
  RTI::Boolean  haveInteractionClassHandle = RTI::RTI_FALSE;
  RTI::Boolean  first                      = RTI::RTI_TRUE;


  localParmStr = new char[strlen (parmString) + 1];
  strcpy (localParmStr, parmString);


  // Tokenize the parameter list string.  Place each parameter handle
  // in the parmList array.

  for (index = 0; index < parmListLength; index++)
  {
    // Get the token.
    if (first)
    {
      parmNameStr = strtok (localParmStr, ",");
      first = RTI::RTI_FALSE;
    }
    else
      parmNameStr = strtok (NULL, ",");

    // If there are no more tokens, exit the loop.
    if (parmNameStr == NULL)
      break;


    // Convert the parameter to a long.  This returns 0 on error, but since
    // 0 could be a valid value, check the number of characters processed.

    parmList[index] = convertLongID (parmNameStr);
    if (parmList[index] == INVALID_ID)
    {
      //  Ok, the user gave us an parameter name.  Convert it to a handle.

      //  If we haven't checked the Interaction Handle yet, do so now.
      if (interactionClassHandle != 0)
 haveInteractionClassHandle = RTI::RTI_TRUE;

      if (!haveInteractionClassHandle)
      {
 interactionClassHandle = convertLongID (interactionClassHandleStr);
 if (interactionClassHandle == INVALID_ID)
   //  Ok, the user gave us a interaction name.
   //  Convert it to a handle.
   interactionClassHandle =
     myRTIamb.getInteractionClassHandle (interactionClassHandleStr);

 haveInteractionClassHandle = RTI::RTI_TRUE;
      }

      parmList[index] = myRTIamb.getParameterHandle (parmNameStr,
           interactionClassHandle);
    }
  }   // end for

  delete [] localParmStr;
}


// Build an array of Federate Handles from a comma-delimited list
// of federates (in either Handle or Name form).
static void buildFederateList (const char* const   federateString,
          RTI::FederateHandle federateList[],
          const int           federateListLength)
{
  char                   *localFederateStr;
  char                   *federateNameStr;
  int                     index;
  RTI::Boolean            first = RTI::RTI_TRUE;


  localFederateStr = new char[strlen (federateString) + 1];
  strcpy (localFederateStr, federateString);


  // Tokenize the federate list string.  Place each federate handle
  // in the federateList array.

  for (index = 0; index < federateListLength; index++)
  {
    // Get the token.
    if (first)
    {
      federateNameStr = strtok (localFederateStr, ",");
      first = RTI::RTI_FALSE;
    }
    else
      federateNameStr = strtok (NULL, ",");

    // If there are no more tokens, exit the loop.
    if (federateNameStr == NULL)
      break;


    // Convert the federate to a long.  This returns 0 on error, but since
    // 0 could be a valid value, check the number of characters processed.

    federateList[index] = convertLongID (federateNameStr);
    if (federateList[index] == INVALID_ID)
      ;
      //  Ok, the user gave us an federate name.  Convert it to a handle.
      //federateList[index] = myRTIamb.getFederateHandle (federateNameStr);

  }   // end for

  delete [] localFederateStr;
}

// Build an array of Region Handles from a comma-delimited list
// of region handles.
static RTI::Boolean buildRegionList (const char* const   regionString,
                                     RTI::Region       *regionList[],
                                     const int           regionListLength)
{
  char                   *localRegionStr;
  char                   *regionNameStr;
  int                     index;
  RTI::Boolean            first = RTI::RTI_TRUE;


  localRegionStr = new char[strlen (regionString) + 1];
  strcpy (localRegionStr, regionString);


  // Tokenize the region list string.  Place each region handle
  // in the regionList array.

  for (index = 0; index < regionListLength; index++)
  {
    // Get the token.
    if (first)
    {
      regionNameStr = strtok (localRegionStr, ",");
      first = RTI::RTI_FALSE;
    }
    else
      regionNameStr = strtok (NULL, ",");

    // If there are no more tokens, exit the loop.
    if (regionNameStr == NULL)
      break;


    // Convert the region to a long.  This returns 0 on error, but since
    // 0 could be a valid value, check the number of characters processed.

    int regionIndex = convertLongID (regionNameStr);
    regionList[index] = regionDB[regionIndex];
    if (regionList[index] == NULL)
    {
      return RTI::RTI_FALSE;
    }
    
  }   // end for
  
  delete [] localRegionStr;
  return RTI::RTI_TRUE;
}

static int
determineUpdateAttrTestCharacteristics (
                              const char* const  attributeCnt,
         const char* const  charactersPerValue,
         const char* const  objClassName,
         const RTI::ObjectClassHandle  objClassHandle,
         int               &attrCnt,
         int               &attrValueStringLength)
{
  char           buf[200];
  ostrstream     myStream (buf, 200);
  int            i = 0;


  try
  {
    // Determine if the requested number of attributes exist in the
    // specified class.
    attrCnt = convertLongID (attributeCnt);
    if ((attrCnt == INVALID_ID) || (attrCnt < 1))
      return -1;

    attrValueStringLength = convertLongID (charactersPerValue);
    if ((attrValueStringLength == INVALID_ID) || (attrValueStringLength < 1))
      return -1;

    for (i = 0; i < attrCnt; i++)
      myRTIamb.getAttributeName ((RTI::AttributeHandle)i + 1, objClassHandle);
  }
  catch (RTI::Exception& ex)
  {
    if (i > 0)
    {
      sprintf (buf,
        "Update Attributes Test:  requested number of "
        "attributes (%d) exceeds the", attrCnt);
      outputMsg (buf);
      sprintf (buf,
        "  number of attributes available for class %s (%d).",
        objClassName, objClassHandle);
      outputMsg (buf);
      sprintf (buf, "  Proceeding using %d attributes...", i);
      outputMsg (buf);

      attrCnt = i;
    }
    else
    {
      sprintf (buf,
        "Update Attributes Test:  class %s (%d) has no attributes.",
        objClassName, objClassHandle);
      outputMsg (buf);
      outputMsg ("Test aborting");
      return -1;
    }
  }
  catch (...)
  {
    outputMsg ("Update Attributes Test:  caught unknown exception");
    return -1;
  }

  attrValueStringLength = convertLongID (charactersPerValue);
#if 0
  if (attrValueStringLength > MAX_BYTES_PER_VALUE)
  {
    sprintf (buf,
      "Update Attributes Test:  requested length of "
      "attribute values (%d) exceeds the", attrValueStringLength);
    outputMsg (buf);
    sprintf (buf,
      "  maximum for these values (%d)", MAX_BYTES_PER_VALUE);
    outputMsg (buf);
    sprintf (buf,
      "  Proceeding using a length of %d...", MAX_BYTES_PER_VALUE);
    outputMsg (buf);

    attrValueStringLength = MAX_BYTES_PER_VALUE;
  }
#endif
  return 0;
}


static int
determineRoundTripTestCharacteristics (
                const char* const  parameterCnt,
  const char* const  charactersPerValue,
  const char* const  interactionClassName,
  const RTI::InteractionClassHandle  interactionClassHandle,
  int               &parmCnt,
  int               &parmValueStringLength)
{
  char           buf[200];
  ostrstream     myStream (buf, 200);
  int            i = 0;


  try
  {
    // Determine if the requested number of parameters exist in the
    // specified class.
    parmCnt = convertLongID (parameterCnt);
    if ((parmCnt == INVALID_ID) || (parmCnt < 0))
      return -1;
    else if (parmCnt > 0)
    {
      parmValueStringLength = convertLongID (charactersPerValue);
      if ((parmValueStringLength == INVALID_ID) || (parmValueStringLength < 1))
 return -1;

      for (i = 0; i < parmCnt; i++)
 myRTIamb.getParameterName ((RTI::ParameterHandle)i,
       interactionClassHandle);
    }
  }
  catch (RTI::Exception& ex)
  {
    sprintf (buf,
      "Round Trip Test:  requested number of "
      "parameters (%d) exceeds the", parmCnt);
    outputMsg (buf);
    sprintf (buf,
      "  number of parameters available for class %s (%d).",
      interactionClassName, interactionClassHandle);
    outputMsg (buf);
    sprintf (buf, "  Proceeding using %d parameters...", i);
    outputMsg (buf);

    parmCnt = i;
  }
  catch (...)
  {
    outputMsg ("Round Trip Test:  caught unknown exception");
    return -1;
  }

  if (parmCnt > 0)
  {
    parmValueStringLength = convertLongID (charactersPerValue);
#if 0
    if (parmValueStringLength > MAX_BYTES_PER_VALUE)
    {
      sprintf (buf,
        "Round Trip Test:  requested length of "
        "parameter values (%d) exceeds the", parmValueStringLength);
      outputMsg (buf);
      sprintf (buf,
        "  maximum for these values (%d)", MAX_BYTES_PER_VALUE);
      outputMsg (buf);
      sprintf (buf,
        "  Proceeding using a length of %d...", MAX_BYTES_PER_VALUE);
      outputMsg (buf);
      
      parmValueStringLength = MAX_BYTES_PER_VALUE;
    }
#endif
  }
  else
    parmValueStringLength = 0;

  return 0;
}
