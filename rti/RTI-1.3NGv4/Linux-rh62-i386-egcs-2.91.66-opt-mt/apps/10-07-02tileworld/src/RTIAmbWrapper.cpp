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
// RTI include files
//-----------------------------------------------------------------
#include <RTI.hh>
#include "RTIAmbWrapper.hh"
#include "globals.hh"
//-----------------------------------------------------------------
// FILE: RTIAmbWrapper.cpp
//
// This File is used in the SIM_AGENT-HLA interface to wrap all
// the method calls made to the rtiAmb in c style functions
//
// The numbers above each of the functions corresponds to the number
// in the RTI specification v1.3. See this document for more details 
// on each of the methods called. For the exact synopsis of the C++
// methods see *  Appendix A - RTI::RTIambassador available from 
// the sdc.dmso.mil site
// 
//
// Created 03/04/02
// Author - Michael Lees (mhl@cs.nott.ac.uk)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
//All the functions in this file are called from poplog. Below is a
//complete list of all the possible argument types poplog can pass.
//
//  1.char* 
//  2.RTI::FederateAmbassadorPtr
//  3.RTI::ObjectClassHandle (unsigned long)
//  4.AttributeHandleSet& 
//  5.RTI::ObjectHandle (unsiged long)
//  6.AttributeHandleValuePairSet
//  7.RTI::FedTime&
//
//  1. char* can be passed by default eg., foo('A string');
//  2. External pointer to a C object
//  3. Unsigned longs can be pass as default
//  4. This is passed as an array of unsigned longs(C++ reconstructs class)
//  5. As 3.
//  6. As 4. except an array of user defined types
//  7. use encode() and decode() methods to transfer as byte array
//----------------------------------------------------------------
//

//extern RTI::RTIambassador       rtiAmb;         // libRTI provided


/* used to determine how many attributes are being passed
   Normally the handleset would be a C++ class but due to the 
   fact that poplog cannot handle C++ classes the list of 
   handles is passed as NULL terminated array.
*/

int sizeOfHandleSet(unsigned long *attributeArray)
{
  int count=0;
  int x=0;

//count = (sizeof attributeArray)/sizeof(unsigned long);

    while(*(&attributeArray+x)!=(unsigned long)NULL)
    {
      x+=sizeof(unsigned long);
      count +=1;
    }
    cout<<"Pointer value: "<<*(&attributeArray+x)<<endl;
  return count;
}

int sizeOfHandleValuePairSet(AttributeHandleValuePair *ahvpset)
{
 int count=0;
 int x=0;//sizeof(AttributeHandleValuePair);

  while(*(&ahvpset+x)!=0)
    {
      x+=sizeof(AttributeHandleValuePair);
      count +=1;
    }
  return count;
}



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

void createFederationExecution(const char* executionName, const char* FED)
{
   try
    {
      //------------------------------------------------------
      // A successful createFederationExecution will cause
      // the fedex process to be executed on this machine.
      //------------------------------------------------------
      rtiAmb.createFederationExecution( executionName, FED );
    }
  
  catch ( RTI::FederationExecutionAlreadyExists& e )
    {
      cerr << "FED: Note: Federation execution already exists." <<
      &e << endl;
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
      //      abort();
    }

}

void destroyFederationExecution(const char* executionName)
{
  try
    {
      cout << "FED: DESTROY FEDERATION EXECUTION CALLED" << endl;
      rtiAmb.destroyFederationExecution( executionName ); 
      cout << "FED: SUCCESSFUL DESTROY FEDERATION EXECUTION CALLED" << endl;
    }
  catch ( RTI::FederatesCurrentlyJoined&  e  )
    {
      cerr << "FED: FederatesCurrentlyJoined" << endl;
    }
  catch ( RTI::FederationExecutionDoesNotExist&  e )
    {
      cerr << "FED: FederationExecutionDoesNotExist" << endl;
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }
  
}

unsigned long  joinFederationExecution( const char *yourName,
				const char *executionName,
				RTI::FederateAmbassadorPtr fedAmbRef)
{

  RTI::Boolean Joined    = RTI::RTI_FALSE;	     
  RTI::FederateHandle federateId;
  int          numTries  = 0;

  while( !Joined && (numTries++ < 20) )
    {
      // When joining a Federation a FederateHandle is returned
      // The type RTI::FederateHandle is unsigned long it is therefore
      // cast to this standard type before being returned to poplog      
      try
	{
	  cout << "FED: JOINING FEDERATION EXECUTION: " 
	       << executionName << endl;
	  federateId = rtiAmb.joinFederationExecution( yourName,
						       executionName, 
						       fedAmbRef);
	  Joined = RTI::RTI_TRUE;
	  return (unsigned long)federateId;
	}
      catch (RTI::FederateAlreadyExecutionMember& e)
	{
	  cerr << "FED: ERROR: " << yourName
	       << " already exists in the Federation Execution "
	   << executionName << "." << endl;
	  cerr << &e << endl;
	  return -1;
	}
      catch (RTI::FederationExecutionDoesNotExist&)
	{
	  cerr << "FED: ERROR: " << executionName 
	   << " Federation Execution "
	       << "does not exists."<< endl;
	  rtiAmb.tick(2.0, 2.0);
	}
      catch ( RTI::Exception& e )
	{
	  cerr << "FED: ERROR:" << &e << endl;
	  return -1;
	}
      
    }
}
  
void resignFederationExecution()
{
  //NOTE
  //
  // When resigning from a federation one of the following enum is
  // passed as an argument.  
  // RTI::RELEASE_ATRIBUTES, 
  // RTI::DELETE_OBJECTS,
  // RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES
  // RTI::NO_ACTION
  //
  // Initially this is left out from the poplog call, although
  // eventually it should be possible to specify which flag to use
  // when resigning from the poplog side So here it is set as
  // RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES 
  //That is when a federate resigns it deletes all it's objects from
  //the execution and release ownership of any attributes

try
      {
         cout << "FED: RESIGN FEDERATION EXECUTION CALLED" << endl;
         
         rtiAmb.resignFederationExecution(
                   RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES ); 
         cout << "FED: SUCCESSFUL RESIGN FEDERATION EXECUTION CALLED" << endl;
      }
      catch ( RTI::Exception& e )
      {
         cerr << "FED: ERROR:" << &e << endl;
	 //         return -1;
      }
}


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
			AttributeHandleSet_t *attributeArray)
{
  // Here the attributehandleset needs to be passed in a form
  // which can be converted to the appropriate form to pass to the method
  // It needs to be a list of unsigned longs of known length x.
  int count;
  

  RTI::AttributeHandleSet *theAttributes; 
  //count=sizeOfHandleSet(attributeArray);
  count=attributeArray->size;
  theAttributes = RTI::AttributeHandleSetFactory::create(count); //create(x) x=number of attributes
  //cycle through the list of attributes passed adding them to HandleSet

  cout<<"Publishing There are "<<count<<" attributes."<<endl;
  for(int i=0;i<count;i++){
    theAttributes->add(attributeArray->attributes[i]);
    cout<<"Adding attribute "<<i<<": "<<attributeArray->attributes[i]<<endl;
  }
  try
    {
      rtiAmb.publishObjectClass( classHandle,*theAttributes);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    } 

  //Always tidy up after yourself
  theAttributes->empty();
  delete theAttributes; 

}


void subscribeObjectClassAttributes(RTI::ObjectClassHandle classHandle, 
				    AttributeHandleSet_t *attributeArray)
{
  //This is as above  
   int count;
  
  RTI::AttributeHandleSet *theAttributes; 
  //count=sizeOfHandleSet(attributeArray);
  count=attributeArray->size;;
  theAttributes = RTI::AttributeHandleSetFactory::create(count);
  //cycle through the list of attributes passed adding them to HandleSet

  cout<<"Subscribing There are "<<count<<" attributes."<<endl;
  for(int i=0;i<count;i++){
    theAttributes->add(attributeArray->attributes[i]);
    cout<<"Adding attribute "<<i<<": "<<attributeArray->attributes[i]<<endl;
  }
    
  try
    {
      rtiAmb.subscribeObjectClassAttributes( classHandle,*theAttributes );
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }
  theAttributes->empty();
  delete theAttributes;
}

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

unsigned long registerObjectInstance(RTI::ObjectClassHandle classHandle)

{
  // Class Handle and ObjectHandle are unsigned longs
  RTI::ObjectHandle instanceId;

  instanceId = rtiAmb.registerObjectInstance( classHandle );
  return (unsigned long int) instanceId;
}


void updateAttributeValues(RTI::ObjectHandle instanceId,
			   AttributeHandleValuePairSet_t *ahvpset,
			   RTI::FedTime& theTime)
{			   
	
  //As with the AttributeHandleSet for publish and subscribe some form 
  //of a list must be passed from poplog to the c function. It's a list
  //of pairs of (RTI::AttributeHandle,AttributeValue)
  //Each item in this list must then be added to the pNvpSet below

  int count=ahvpset->size; //Number of attributes to update

  RTI::AttributeHandleValuePairSet* pNvpSet; 
  
  pNvpSet = RTI::AttributeSetFactory::create(count); //Create class instance

  for(int i=0;i<count;i++){
  //Add all the attribute value pairs to the pNvpSet
  pNvpSet->add( ahvpset->ahvp[i].Id,
		ahvpset->ahvp[i].value,
		ahvpset->ahvp[i].length );
  }
  // This could return a event retraction handle but these are not
  // used at present The fourth argument is a tag (string) which isn't
  // used

  try
    {
      (void) rtiAmb.updateAttributeValues( instanceId,
				       *pNvpSet,
				       theTime, 
				       NULL );
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }

}


void deleteObjectInstance(RTI::ObjectHandle instanceId,
			  RTI::FedTime& theTime)
{

  // theTime is a pointer to a federate time object
  try
    {
      cout << "FED: DESTROYING OBJECT INSTANCE: "<<instanceId
	   << " AT TIME: "<<theTime<<endl;
      rtiAmb.deleteObjectInstance(instanceId, theTime, NULL);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }
}
  
void requestClassAttributeValueUpdate(RTI::ObjectClassHandle classHandle,
				      AttributeHandleSet_t *attributeArray)
{  
  // Here the attributehandleset needs to be passed in a form
  // which can be converted to the appropriate form to pass to the method
  // It needs to be a list of unsigned longs of known length x.
 int count;
  

  RTI::AttributeHandleSet *theAttributes; 
  count=attributeArray->size;
  theAttributes = RTI::AttributeHandleSetFactory::create(count); //create(x) x=number of attributes
  
  //cycle through the list of attributes passed adding them to HandleSet
  for(int i=0;i<count;i++)
    theAttributes->add(attributeArray->attributes[i]); 
  try
    {
      rtiAmb.publishObjectClass( classHandle,*theAttributes);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }

  //Always tidy up after yourself
  theAttributes->empty();
  delete theAttributes;   
}

void requestObjectAttributeValueUpdate(RTI::ObjectHandle instanceId,
				       AttributeHandleSet_t *attributeArray)
{
  int count;

  RTI::AttributeHandleSet *theAttributes; 
  count=attributeArray->size;
  theAttributes = RTI::AttributeHandleSetFactory::create(count); //create(x) x=number of attributes
  
  //cycle through the list of attributes passed adding them to HandleSet
  for(int i=0;i<count;i++)
    theAttributes->add(attributeArray->attributes[i]);

  try
    {
      rtiAmb.requestObjectAttributeValueUpdate( instanceId,*theAttributes);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }
  
  //Always tidy up after yourself
  theAttributes->empty();
  delete theAttributes;   			
}



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
					      AttributeHandleSet_t  *attributeArray)
{
  
  // Here the attributehandleset needs to be passed in a form
  // which can be converted to the appropriate form to pass to the method
  // It needs to be a list of unsigned longs of known length x.
  
  
  int count = attributeArray->size;

  RTI::AttributeHandleSet *theAttributes; 
  //  count=sizeOfHandleSet(attributeArray);
  
  theAttributes = RTI::AttributeHandleSetFactory::create(count); //create(x) x=number of attributes
  
  //cycle through the list of attributes passed adding them to HandleSet
  for(int i=0;i<count;i++)
    theAttributes->add(attributeArray->attributes[i]) ;

  try
    {
      rtiAmb.unconditionalAttributeOwnershipDivestiture( instanceId,
						       *theAttributes);      
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }

  //Always tidy up after yourself
  theAttributes->empty();
  delete theAttributes;   
}

void attributeOwnershipAcquisitionIfAvailable(RTI::ObjectHandle instanceId,
					      AttributeHandleSet_t *attributeArray)
{
  // Here the attributehandleset needs to be passed in a form
  // which can be converted to the appropriate form to pass to the method
  // It needs to be a list of unsigned longs of known length x.
  
  int count = attributeArray->size;  

  RTI::AttributeHandleSet *theAttributes; 
  // count=sizeOfHandleSet(attributeArray);
  theAttributes = RTI::AttributeHandleSetFactory::create(count); //create(x) x=number of attributes
  
  //cycle through the list of attributes passed adding them to HandleSet
  for(int i=0;i<count;i++)
    theAttributes->add(attributeArray->attributes[i]);  
  try
    {
      rtiAmb.attributeOwnershipAcquisitionIfAvailable( instanceId,
						       *theAttributes);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }
  
  //Always tidy up after yourself
  theAttributes->empty();
  delete theAttributes;   
}

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
			  RTI::FedTime& theLookahead)
{
  try
    {
      rtiAmb.enableTimeRegulation(theFederateTime, theLookahead);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }   
}

void timeAdvanceRequest(RTI::FedTime& theTime)
{
  try
    {
      rtiAmb.timeAdvanceRequest(theTime);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }   

}


void enableTimeConstrained()
{
  try
    {
      rtiAmb.enableTimeConstrained();
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }   
}

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

RTI::ObjectClassHandle getObjectClassHandle(const char *theName)
{
  // Here theName is a string corresponding to the name of the 
  // class in the FOM (*.fed file)
  
try
    {
      return rtiAmb.getObjectClassHandle(theName);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }   
}

char* getObjectClassName(RTI::ObjectClassHandle instanceId)
{
  // Here theName is a string corresponding to the name of the 
  // class in the FOM (*.fed file)
  
try
    {
      return rtiAmb.getObjectClassName(instanceId);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }   
}

RTI::AttributeHandle getAttributeHandle(const char *theName,
					RTI::ObjectClassHandle fromClass)
{
  // This returns the handle of the specified attribute(theName)
  // in the specified class (fromClass)
  // Here theName is a string corresponding to the name of the 
  // class in the FOM (*.fed file)
  
try
    {
      return rtiAmb.getAttributeHandle(theName,fromClass);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }   
}

char* getAttributeName(RTI::ObjectClassHandle AttributeId,
		       RTI::ObjectClassHandle fromClass)
{
  // This returns the handle of the specified attribute(AttributeId)
  // in the specified class (fromClass)
  // Here theName is a string corresponding to the name of the 
  // class in the FOM (*.fed file)
  
try
    {
      return rtiAmb.getAttributeName(AttributeId,fromClass);
    }
  catch ( RTI::Exception& e )
    {
      cerr << "FED: ERROR:" << &e << endl;
    }   
}


