#include <stdio.h>
#include <dlfcn.h>      /* defines dlopen(), etc.       */
#include "globals.hh"   /* used for AttributeHandle types etc. */

/* first define all functions to be returned by dlsym*/
void (*dl_createFederationExecution)(char *, char *);
void (*dl_destroyFederationExecution)(char *);
unsigned long(*dl_joinFederationExecution)(char *,char *,unsigned *);
void (*dl_resignFederationExecution)();
void (*dl_publishObjectClass)(unsigned long,AttributeHandleSet_t);
void (*dl_subscribeObjectClassAttributes)(unsigned long ,AttributeHandleSet_t);
unsigned long (*dl_registerObjectInstance)(unsigned long);
void (*dl_updateAttributeValues)(unsigned long, AttributeHandleValuePairSet_t,unsigned long);
void (*dl_deleteObjectInstance)(unsigned long, unsigned long);
void (*dl_requestClassAttributeValueUpdate)(unsigned long,AttributeHandleSet_t);
void (*dl_requestObjectAttributeValueUpdate)(unsigned long,AttributeHandleSet_t);
void (*dl_unconditionalAttributeOwnershipDivesture)(unsigned long,
						    AttributeHandleSet_t);
void (*dl_attributeOwnershipAcquisitionIfAvailable)(unsigned long,
						    AttributeHandleSet_t);
void (*dl_enableTimeRegulation)(unsigned long, unsigned long);
void (*dl_timeAdvanceRequest)(unsigned long);
void (*dl_enableTimeConstrained)();
unsigned long (*dl_getObjectClassHandle)(char *);
char *(*dl_getObjectClassName)(unsigned long);
char *(*dl_getAttributeName)(unsigned long,unsigned long);
unsigned long (*dl_getAttributeHandle)(char *, unsigned long);
void (*dl_tick)(unsigned long, unsigned long);

NODE_t *(*dl_get_discoverObjectInstance)();
NODE_t *(*dl_get_reflectAttributeValues)();
NODE_t *(*dl_get_removeObjectInstance)();
NODE_t *(*dl_get_provideAttributeValueUpdate)();
NODE_t *(*dl_get_attributeOwnershipDivestitureNotification)();
NODE_t* (*dl_get_attributeOwnershipAcquisitionNotification)();
NODE_t *(*dl_get_attributeOwnershipUnavailable)();
NODE_t *(*dl_get_timeRegulationEnabled)();
NODE_t *(*dl_get_timeConstrainedEnabled)();
NODE_t *(*dl_get_timeAdvanceGrant)();

void dlinit(const char* librarypath)
{
  void* lib_handle;       /* handle of the opened library */
  const char* error_msg;  /* Error message returned */

  /*  = "/home/mhl/rti/RTI-1.3NGv4/Linux-rh62-i386-egcs-2.91.66-opt-mt/apps/tileWorld/data/librti.so";*/

  /*Open the library*/
  fprintf(stderr, "Trying dlopen()\n");
  lib_handle = dlopen(librarypath, RTLD_NOW);

  /*Check to ensure library opened correctly*/
  if (!lib_handle) {
    fprintf(stderr, "Error during dlopen(): %s\n", dlerror());
    exit(1);
  } else {
    fprintf(stderr, "Completed dlopen()\n");
  }


  /* now locate the functions in the library*/
  dl_createFederationExecution = dlsym(lib_handle,"createFederationExecution"); 
  dl_destroyFederationExecution= dlsym(lib_handle,"destroyFederationExecution"); 
  dl_joinFederationExecution= dlsym(lib_handle,"joinFederationExecution");
  dl_resignFederationExecution= dlsym(lib_handle,"resignFederationExecution");
  dl_publishObjectClass= dlsym(lib_handle,"publishObjectClass");
  dl_subscribeObjectClassAttributes= dlsym(lib_handle,"subscribeObjectClassAttributes");
  dl_registerObjectInstance= dlsym(lib_handle,"registerObjectInstance");
  dl_updateAttributeValues= dlsym(lib_handle,"updateAttributeValues");
  dl_deleteObjectInstance= dlsym(lib_handle,"deleteObjectInstance");
  dl_requestClassAttributeValueUpdate= dlsym(lib_handle,"requestClassAttributeValueUpdate");
  dl_requestObjectAttributeValueUpdate= dlsym(lib_handle,"requestObjectAttributeValueUpdate");
  dl_unconditionalAttributeOwnershipDivesture= dlsym(lib_handle,
						     "unconditionalAttributeOwnershipDivesture");
  dl_attributeOwnershipAcquisitionIfAvailable= dlsym(lib_handle,
						     "attributeOwnershipAcquisitionIfAvailable");
  dl_enableTimeRegulation= dlsym(lib_handle,"enableTimeRegulation");
  dl_timeAdvanceRequest= dlsym(lib_handle,"timeAdvanceRequest");
  dl_enableTimeConstrained= dlsym(lib_handle,"enableTimeConstrained");
  dl_getObjectClassHandle= dlsym(lib_handle,"getObjectClassHandle");
  dl_getObjectClassName= dlsym(lib_handle,"getObjectClassName");
  dl_getAttributeName= dlsym(lib_handle,"getAttributeName");
  dl_getAttributeHandle= dlsym(lib_handle,"getAttributeHandle");
  dl_tick= dlsym(lib_handle,"tick");
  dl_get_discoverObjectInstance=dlsym(lib_handle,"get_discoverObjectInstance");
  dl_get_reflectAttributeValues=dlsym(lib_handle,"get_reflectAttributeValues");
  dl_get_removeObjectInstance=dlsym(lib_handle,"get_removeObjectInstance");
  dl_get_provideAttributeValueUpdate=dlsym(lib_handle,"get_provideAttributeValueUpdate");
  dl_get_attributeOwnershipDivestitureNotification=dlsym(lib_handle,
							 "get_attributeOwnershipDivestitureNotification");
  dl_get_attributeOwnershipAcquisitionNotification=dlsym(lib_handle,
						       "get_attributeOwnershipAcquisitionNotification");
  dl_get_attributeOwnershipUnavailable=dlsym(lib_handle,"get_attributeOwnershipUnavailable");
  dl_get_timeRegulationEnabled=dlsym(lib_handle,"get_timeRegulationEnabled");
  dl_get_timeConstrainedEnabled=dlsym(lib_handle,"get_timeConstrainedEnabled");
  dl_get_timeAdvanceGrant=dlsym(lib_handle,"get_timeAdvanceGrant");
  /* check that no error occured */
  error_msg = dlerror();
  if (error_msg) {
    fprintf(stderr, "Error locating symbol - %s\n", error_msg);
    exit(1);
  }
}

/* all functions defined here so poplog can see symbols when dlinit.so. is loaded */

void createFederationExecution(char *executionName, char *Fedfile){
  (*dl_createFederationExecution)(executionName, Fedfile);
}
void destroyFederationExecution(char *executionName){
  (*dl_destroyFederationExecution)(executionName);
}

unsigned long joinFederationExecution(char *federateName,
				      char *executionName,
				      unsigned *fedAmbRef){
  return (*dl_joinFederationExecution)(federateName,executionName,fedAmbRef);
}

void resignFederationExecution(){
  (*dl_resignFederationExecution)();
}

void publishObjectClass(unsigned long classHandle ,AttributeHandleSet_t handleSet){
  (*dl_publishObjectClass)(classHandle,handleSet);
}

void subscribeObjectClassAttributes(unsigned long classHandle, AttributeHandleSet_t handleSet){
  (*dl_subscribeObjectClassAttributes)(classHandle ,handleSet);
}

unsigned long registerObjectInstance(unsigned long classHandle){
  (*dl_registerObjectInstance)(classHandle);
}

void updateAttributeValues(unsigned long instanceId, 
			   AttributeHandleValuePairSet_t handleValuePairSet,
			   unsigned long theTime){
  (*dl_updateAttributeValues)(instanceId, handleValuePairSet,theTime);
}

void deleteObjectInstance(unsigned long instanceId, unsigned long theTime){
  (*dl_deleteObjectInstance)(instanceId,theTime);
}

void requestClassAttributeValueUpdate(unsigned long classHandle,AttributeHandleSet_t handleSet){
  (*dl_requestClassAttributeValueUpdate)(classHandle,handleSet);
}

void requestObjectAttributeValueUpdate(unsigned long instanceId,
				       AttributeHandleSet_t handleSet){
  (*dl_requestObjectAttributeValueUpdate)(instanceId,handleSet);
}

void unconditionalAttributeOwnershipDivesture(unsigned long instanceId,
					      AttributeHandleSet_t handleSet){
  (*dl_unconditionalAttributeOwnershipDivesture)(instanceId,handleSet);
}

void attributeOwnershipAcquisitionIfAvailable(unsigned long instanceId,
					      AttributeHandleSet_t handleSet){
  (*dl_attributeOwnershipAcquisitionIfAvailable)(instanceId, handleSet);
}

void enableTimeRegulation(unsigned long theFederateTime, unsigned long theLookahead){
  (*dl_enableTimeRegulation)(theFederateTime, theLookahead);
}

void timeAdvanceRequest(unsigned long theTime){
  (*dl_timeAdvanceRequest)(theTime);
}

void enableTimeConstrained(){
  (*dl_enableTimeConstrained)();
}
 

unsigned long getObjectClassHandle(char *theName){
  return (*dl_getObjectClassHandle)(theName);
}

char *getObjectClassName(unsigned long instanceId){
  return (*dl_getObjectClassName)(instanceId);
}

char *getAttributeName(unsigned long attributeId ,unsigned long classHandle){
  (*dl_getAttributeName)(attributeId ,classHandle);
}

unsigned long getAttributeHandle(char *theName, unsigned long classHandle){
  return (*dl_getAttributeHandle)(theName, classHandle);
}

void tick(unsigned long minimum, unsigned long maximum){
  (*dl_tick)(minimum,maximum);
}

NODE_t *get_discoverObjectInstance(){
  return (*dl_get_discoverObjectInstance)();
}
  
NODE_t *get_reflectAttributeValues(){
  return (*dl_get_reflectAttributeValues)();
}

NODE_t *get_removeObjectInstance(){
  return (*dl_get_removeObjectInstance)();
}

NODE_t *get_provideAttributeValueUpdate(){
  return (*dl_get_provideAttributeValueUpdate)();
}

NODE_t *get_attributeOwnershipDivestitureNotification(){
  return (*dl_get_attributeOwnershipDivestitureNotification)();
}

NODE_t*get_attributeOwnershipAcquisitionNotification(){
  return (*dl_get_attributeOwnershipAcquisitionNotification)();
}

NODE_t *get_attributeOwnershipUnavailable(){
  return (*dl_get_attributeOwnershipUnavailable)();
}

NODE_t *get_timeRegulationEnabled(){
  return (*dl_get_timeRegulationEnabled)();
}

NODE_t *get_timeConstrainedEnabled(){
  return (*dl_get_timeConstrainedEnabled)();
}

NODE_t *get_timeAdvanceGrant(){
  return (*dl_get_timeAdvanceGrant)();
}

