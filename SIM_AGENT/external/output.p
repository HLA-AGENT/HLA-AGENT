;;; DECLARING VARIABLE x



                                        ** <undef x>
** <callback <external_ptr> 5 6>
** 0
** 0
** 0

;;; MISHAP - CALLBACK NEEDED
;;; INVOLVING:  0
;;; FILE     :  /home/mhl/external/temp.p   LINE NUMBER:  64
;;; DOING    :  xptr_ptr runproc
;;; DECLARING VARIABLE y
** <external_ptr>

;;; MISHAP - NUMBER(S) NEEDED
;;; INVOLVING:  <external_ptr> 1
;;; FILE     :  /home/mhl/external/temp.p   LINE NUMBER:  65
;;; DOING    :  + runproc
** <external_ptr>
;;; DECLARING VARIABLE acb

;;; DECLARING VARIABLE x

;;; MISHAP - enp: EXECUTING NON-PROCEDURE
;;; INVOLVING:  <undef x>
;;; FILE     :  /home/mhl/external/external.p   LINE NUMBER:  19
;;; DOING    :  runproc
** struct _the_callback
    x - 3
    y - 4
** 3
** 4
** 3

;;; MISHAP - Unknown spec definition
;;; INVOLVING:  ulong
;;; FILE     :  /home/mhl/external/external.p   LINE NUMBER:  42
;;; DOING    :  read_spec read_vars read_struct read_spec read_vars read_typed
;;;     ef newc_dec apply external runproc
** 3
** 4
;;; Error: can't find value for symbol foo

;;; MISHAP - ERRORS ACCESSING EXTERNAL SYMBOLS (see above)
;;; FILE     :  /home/mhl/external/external.p   LINE NUMBER:  51
;;; DOING    :  external_do_load exload_do_batch_load do_load external
;;;     runproc
** 135387775
** 135387577
** 135387775
** 135387577
** 135387775
** 135387577
** 135387775
  ** 0
** 0
** 1
** 1
** 1
** 1
** 1
;;; DECLARING VARIABLE x
;;; DECLARING VARIABLE array1

;;; MISHAP - INTEGER -2147483648 TO 2147483647 NEEDED
;;; INVOLVING:  4294967295
;;; FILE     :  /home/mhl/external/external.p   LINE NUMBER:  140
;;; DOING    :  fast_subscrpointer_to_int runproc
** 0

;;; MISHAP - INTEGER -2147483648 TO 2147483647 NEEDED
;;; INVOLVING:  4294967295
;;; FILE     :  /home/mhl/external/external.p   LINE NUMBER:  140
;;; DOING    :  fast_subscrpointer_to_int runproc

;;; MISHAP - INTEGER -2147483648 TO 2147483647 NEEDED
;;; INVOLVING:  4294967295
;;; FILE     :  /home/mhl/external/external.p   LINE NUMBER:  140
;;; DOING    :  fast_subscrpointer_to_int runproc


;;; DECLARING VARIABLE array2
** 2
** 2
** 1785358954

;;; MISHAP - ste: STACK EMPTY (missing argument? missing result?)
;;; FILE     :  /home/mhl/external/external.p   LINE NUMBER:  121
;;; DOING    :  start runproc
;;; DECLARING VARIABLE acb
;;; DECLARING VARIABLE node
** 0

;;; MISHAP - enp: EXECUTING NON-PROCEDURE
;;; INVOLVING:  <external_ptr>
;;; FILE     :  /home/mhl/external/external.p   LINE NUMBER:  125
;;; DOING    :  runproc
;;; Warning: can't open shared object ./rti.so (Success)
;;; Error: can't find value for symbol getAttributeHandle
;;; Error: can't find value for symbol getAttributeName
;;; Error: can't find value for symbol getObjectClassName
;;; Error: can't find value for symbol getObjectClassHandle
;;; Error: can't find value for symbol enableTimeRegulation
;;; Error: can't find value for symbol attributeOwnershipAcquisitionIfAvailabl
;;;     e
;;; Error: can't find value for symbol unconditionalAttributeOwnershipDivestur
;;;     e
;;; Error: can't find value for symbol requestObjectAttributeValueUpdate
;;; Error: can't find value for symbol requestClassAttributeValueUpdate
;;; Error: can't find value for symbol deleteObjectInstance
;;; Error: can't find value for symbol updateAttributeValues
;;; Error: can't find value for symbol registerObjectInstance
;;; Error: can't find value for symbol subscribeObjectClassAttributes
;;; Error: can't find value for symbol publishObjectClass
;;; Error: can't find value for symbol resignFederationExecution
;;; Error: can't find value for symbol joinFederationExecution
;;; Error: can't find value for symbol destroyFederationExecution
;;; Error: can't find value for symbol createFederationExecution

;;; MISHAP - ERRORS ACCESSING EXTERNAL SYMBOLS (see above)
;;; FILE     :  /home/mhl/SIM_AGENT/external/rti.p   LINE NUMBER:  432
;;; DOING    :  external_do_load exload_do_batch_load external runproc
;;;
;;; Warning: can't open shared object /home/mhl/SIM_AGENT/external/rti.so
;;;     (Success)
;;; Error: can't find value for symbol getAttributeHandle
;;; Error: can't find value for symbol getAttributeName
;;; Error: can't find value for symbol getObjectClassName
;;; Error: can't find value for symbol getObjectClassHandle
;;; Error: can't find value for symbol enableTimeRegulation
;;; Error: can't find value for symbol attributeOwnershipAcquisitionIfAvailabl
;;;     e
;;; Error: can't find value for symbol unconditionalAttributeOwnershipDivestur
;;;     e
;;; Error: can't find value for symbol requestObjectAttributeValueUpdate
;;; Error: can't find value for symbol requestClassAttributeValueUpdate
;;; Error: can't find value for symbol deleteObjectInstance
;;; Error: can't find value for symbol updateAttributeValues
;;; Error: can't find value for symbol registerObjectInstance
;;; Error: can't find value for symbol subscribeObjectClassAttributes
;;; Error: can't find value for symbol publishObjectClass
;;; Error: can't find value for symbol resignFederationExecution
;;; Error: can't find value for symbol joinFederationExecution
;;; Error: can't find value for symbol destroyFederationExecution
;;; Error: can't find value for symbol createFederationExecution
;;; Error: can't find value for symbol foo

;;; MISHAP - ERRORS ACCESSING EXTERNAL SYMBOLS (see above)
;;; FILE     :  /home/mhl/SIM_AGENT/external/rti.p   LINE NUMBER:  435
;;; DOING    :  external_do_load exload_do_batch_load external runproc
;;;
foo=>
** <procedure foo>
foo(3)=>

;;; MISHAP - ste: STACK EMPTY (missing argument? missing result?)
;;; FILE     :  /home/mhl/SIM_AGENT/external/output.p   LINE NUMBER:  157
;;; DOING    :  new_pop_ext_call runproc

;;; MISHAP - interrupted
;;; DOING    :  runproc
;;; editing: output.p, ON LINE 157
;;; Warning: can't open shared object /home/mhl/SIM_AGENT/external/rti.so
;;;     (Success)
;;; Error: can't find value for symbol tick
;;; Error: can't find value for symbol getAttributeHandle
;;; Error: can't find value for symbol getAttributeName
;;; Error: can't find value for symbol getObjectClassName
;;; Error: can't find value for symbol getObjectClassHandle
;;; Error: can't find value for symbol enableTimeRegulation
;;; Error: can't find value for symbol attributeOwnershipAcquisitionIfAvailabl
;;;     e
;;; Error: can't find value for symbol unconditionalAttributeOwnershipDivestur
;;;     e
;;; Error: can't find value for symbol requestObjectAttributeValueUpdate
;;; Error: can't find value for symbol requestClassAttributeValueUpdate
;;; Error: can't find value for symbol deleteObjectInstance
;;; Error: can't find value for symbol updateAttributeValues
;;; Error: can't find value for symbol registerObjectInstance
;;; Error: can't find value for symbol subscribeObjectClassAttributes
;;; Error: can't find value for symbol publishObjectClass
;;; Error: can't find value for symbol resignFederationExecution
;;; Error: can't find value for symbol joinFederationExecution
;;; Error: can't find value for symbol destroyFederationExecution
;;; Error: can't find value for symbol createFederationExecution
;;; Error: can't find value for symbol foo
;;; Error: can't find value for symbol get_timeAdvanceGrant
;;; Error: can't find value for symbol get_timeConstrainedEnabled
;;; Error: can't find value for symbol get_timeRegulationEnabled
;;; Error: can't find value for symbol get_attributeOwnershipUnavailable
;;; Error: can't find value for symbol get_attributeOwnershipAcquisitionNotifi
;;;     cation
;;; Error: can't find value for symbol get_attributeOwnershipDivestitureNotifi
;;;     cation
;;; Error: can't find value for symbol get_provideAttributeValueUpdate
;;; Error: can't find value for symbol get_removeObjectInstance
;;; Error: can't find value for symbol get_reflectAttributeValues
;;; Error: can't find value for symbol get_discoverObjectInstance

;;; MISHAP - ERRORS ACCESSING EXTERNAL SYMBOLS (see above)
;;; FILE     :  /home/mhl/SIM_AGENT/external/rti.p   LINE NUMBER:  450
;;; DOING    :  external_do_load exload_do_batch_load external runproc
;;;
