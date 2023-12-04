#ifndef _DATATYPES_H
#define _DATATYPES_H

#include <Xm/Xm.h>

typedef void (*ServiceFunct)(char **fields, int fieldCnt);


/*  The order of this enumerated type is very important.  The promptRecs array
    (an array of PromptStruct records defined in config.h) in indexed by these
    names.  The elements of that array must appear in the same order as this
    enumeration type.
*/

typedef enum {
  CREATE_FEDEX,
  DESTROY_FEDEX,
  JOIN_FEDEX,
  RESIGN_FEDEX,
  REGISTER_SYNC_POINT,
  SYNC_POINT_ACHIEVED,
  REQUEST_FEDERATION_SAVE,
  FEDERATE_SAVE_BEGUN,
  FEDERATE_SAVE_ACHIEVED,
  FEDERATE_SAVE_NOT_ACHIEVED,
  REQUEST_RESTORE,
  RESTORE_ACHIEVED,
  RESTORE_NOT_ACHIEVED,
  PUBLISH_OBJ_CLASS,
  UNPUBLISH_OBJ_CLASS,
  PUBLISH_INTERACTION_CLASS,
  UNPUBLISH_INTERACTION_CLASS,
  SUBSCRIBE_OBJ_CLASS_ATTR,
  UNSUBSCRIBE_OBJ_CLASS_ATTR,
  SUBSCRIBE_INTERACTION_CLASS,
  UNSUBSCRIBE_INTERACTION_CLASS,
  REGISTER_OBJ,
  UPDATE_ATTR_VALUES,
  SEND_INTERACTION,
  DELETE_OBJ,
  LOCAL_DELETE_OBJ,
  CHANGE_ATTR_TRANSPORT_TYPE,
  CHANGE_ATTR_ORDER_TYPE,
  CHANGE_INTERACTION_TRANSPORT_TYPE,
  CHANGE_INTERACTION_ORDER_TYPE,
  REQUEST_OBJ_ATTR_VALUE_UPDATE,
  REQUEST_CLASS_ATTR_VALUE_UPDATE,
  RETRACT,
  UNCONDITIONAL_ATTR_OWNERSHIP_DIVESTITURE,
  NEGOTIATED_ATTR_OWNERSHIP_DIVESTITURE,
  ATTR_OWNERSHIP_ACQUISITION,
  ATTR_OWNERSHIP_RELEASE_RESPONSE,
  CANCEL_NEGOTIATED_ATTR_OWNERSHIP_DIVESTITURE,
  CANCEL_ATTR_OWNERSHIP_ACQUISITION,
  ATTR_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
  QUERY_ATTR_OWNERSHIP,
  ATTR_IS_OWNED_BY_FEDERATE,
  ENABLE_TIME_REGULATION,
  DISABLE_TIME_REGULATION,
  ENABLE_TIME_CONSTRAINED,
  DISABLE_TIME_CONSTRAINED,
  REQUEST_LBTS,
  REQUEST_FEDERATE_TIME,
  REQUEST_MIN_NEXT_EVENT_TIME,
  CHANGE_LOOKAHEAD,
  REQUEST_LOOKAHEAD,
  TIME_ADVANCE_REQUEST,
  TIME_ADVANCE_REQUEST_AVAILABLE,
  NEXT_EVENT_REQUEST,
  NEXT_EVENT_REQUEST_AVAILABLE,
  FLUSH_QUEUE_REQUEST,
  ENABLE_ASYNCHRONOUS_DELIVERY,
  DISABLE_ASYNCHRONOUS_DELIVERY,
  CREATE_REGION,
  SET_RANGE_LOWER_BOUND,
  SET_RANGE_UPPER_BOUND,
  GET_RANGE_LOWER_BOUND,
  GET_RANGE_UPPER_BOUND,
  GET_SPACE_HANDLE_FOR_REGION,
  GET_NUMBER_OF_EXTENTS,
  GET_RANGE_LOWER_BOUND_NOTIFICATION_LIMIT,
  GET_RANGE_UPPER_BOUND_NOTIFICATION_LIMIT,
  NOTIFY_REGION_MODIFICATION,
  DELETE_REGION,
  REGISTER_OBJ_WITH_REGION,
  ASSOCIATE_REGION_FOR_UPDATES,
  UNASSOCIATE_REGION_FOR_UPDATES,
  SUBSCRIBE_OBJ_CLASS_ATTR_WITH_REGION,
  UNSUBSCRIBE_OBJ_CLASS_WITH_REGION,
  SUBSCRIBE_INTERACTION_CLASS_WITH_REGION,
  UNSUBSCRIBE_INTERACTION_CLASS_WITH_REGION,
  SEND_INTERACTION_WITH_REGION,
  REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE_WITH_REGION,
  GET_OBJ_CLASS_HANDLE,
  GET_OBJ_CLASS_NAME,
  GET_ATTR_HANDLE,
  GET_ATTR_NAME,
  GET_INTERACTION_CLASS_HANDLE,
  GET_INTERACTION_CLASS_NAME,
  GET_PARM_HANDLE,
  GET_PARM_NAME,
  GET_OBJ_HANDLE,
  GET_OBJ_NAME,
  GET_SPACE_HANDLE,
  GET_SPACE_NAME,
  GET_DIMENSION_HANDLE,
  GET_DIMENSION_NAME,
  GET_TRANSPORTATION_HANDLE,
  GET_TRANSPORTATION_NAME,
  GET_ORDERING_HANDLE,
  GET_ORDERING_NAME,
  GET_ATTRIBUTE_SPACE_HANDLE,
  GET_OBJ_CLASS,
  GET_INTERACTION_SPACE_HANDLE,
  ENABLE_CLASS_RELEVANCE_ADVISORY,
  DISABLE_CLASS_RELEVANCE_ADVISORY,
  ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY,
  DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY,
  ENABLE_ATTRIBUTE_SCOPE_ADVISORY,
  DISABLE_ATTRIBUTE_SCOPE_ADVISORY,
  ENABLE_INTERACTION_RELEVANCE_ADVISORY,
  DISABLE_INTERACTION_RELEVANCE_ADVISORY,
  TICK,
  AUTO_TICK } ServiceID;

typedef enum { NONE,
               IF_SPEC,
               API,
               BOTH } AvailableHelp;

typedef enum { TICK_NO_ARGS = 1,
               TICK_WITH_ARGS=3,
               CONTINUE,
               STOP } AutoTickStates;


/***********************************************************************************
  The following are comments on the PromptStruct structure that follows:
  
  prompts       -- An array of prompt strings appropriate to a particular command.
  promptCnt     -- The number of prompt strings a command uses.
  helpTag       -- The HTML tag used to locate specific information on a command
                   within the IF Specification and/or API Specification help files.
                   If no help is available, this field should contain an empty
                   string ("").
  availableHelp -- Specifies the type of help available.  This field takes its value
                   from the AvailableHelp enumerated type defined above.  If no
                   help is available for the command, this field should
                   specify NONE.
  service       -- A pointer to a function in the rtiAmbInterface.cc file.
                   A function prototype should be defined for the function in the
                   rtiAmbInterface.ff file.
  extendedForm  -- A pointer to another PromptStruct record.  This is used only if
                   an extended form of a command is available.  Otherwise, this
                   field should contain a NULL.
  ***********************************************************************************/


struct PromptStruct_t
{
  char                  **prompts;
  short                   promptCnt;
  char                   *helpTag;
  AvailableHelp           availableHelp;
  ServiceFunct            service;
  struct PromptStruct_t  *extendedForm;
};
typedef struct PromptStruct_t PromptStruct;


#endif








