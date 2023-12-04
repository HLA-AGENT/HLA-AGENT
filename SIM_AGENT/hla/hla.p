/* ---- Copyright University of Nottingham 2002. All rights reserved. ------
 > File:            /home/bsl/pop/hla/hla.p
 > Purpose:         Distributing sim_agent using the High Level Architecture
 > Author:          Brian Logan, Sep 23 2002
 > Documentation:   
 > Related Files:   hla/*
 > RCS Info:        $Id: hla.p,v 1.7 2002/09/25 13:32:28 bsl Exp $
 */

uses objectclass;
uses newkit;
uses sim_agent;

uses fom;
uses rti;
uses proxy;
uses hla_startup;

;;; This file contains the defintions of the following procedures:
vars procedure HLA_object_new_wrapper,
     procedure HLA_object_update_wrapper,
     procedure HLA_object_destroy_wrapper,

;;; We also redefine the following procedures and methods:
     procedure sim_setup_scheduler,
     procedure sim_post_cycle_actions,
     procedure sim_run_agent;

;;; Information about the federation.
define :singleton HLA_federation;
    HLA_executionName;
    HLA_federationFile;
    ;;; The current logical time for the federation.
    HLA_federation_time == 0;
    enddefine;

;;; Information about this federate.  We assume that a federate can be a
;;; member of at most one federation.
define :singleton HLA_federate;
    ;;; A string naming this federate
    slot HLA_federateName;
    ;;; The id assoicated with the federate by the RTI for this execution.
    slot HLA_federateId;
    ;;; An external reference to the federate Ambassador
    slot HLA_federateAmbassadorRef;
    ;;; An association list containing the information used to configure this
    ;;; federate.
    slot HLA_federate_configuration == false;
    ;;; The list of HLA_class instances defined for this federate.
    slot HLA_federate_classes == [];
    ;;; A list of this federate's proxies: this is a subset of
    ;;; HLA_federate_objects (see below).
    slot HLA_federate_proxies == [];
    ;;; A list of proxy objects (typically created by the intialisation of
    ;;; the user's simulation) for which we don't have instanceIds.
    slot HLA_federate_freelist == [];
    ;;; Mapping from instanceIds to instances
    slot HLA_federate_instances == newassoc([]);
    ;;; A list of updates performed at this cycle
    slot HLA_federate_updates == [];
    ;;; A list of updates which are deferred pending a transfer of ownership
    ;;; of the attribute
    slot HLA_federate_deferredUpdates == [];
    enddefine;

;;; Returns a list of objects and proxies created on this federate.  
define :method HLA_federate_objects(federate:HLA_federate) -> objects;
    lvars hla_class;
    [% for hla_class in HLA_federate_classes(federate) do
	   dl(HLA_class_objects(hla_class));
	   endfor; %] -> objects;
    enddefine;

;;; Returns a list of all the attributes defined for this federate from
;;; the Federation Object Model.  Not clear if we need this.
define :method HLA_federate_attributes(federate:HLA_federate) -> attributes;
    lvars hla_class;
    [% for hla_class in HLA_federate_classes(federate) do
	   dl(HLA_class_attributes(hla_class));
	   endfor; %] -> attributes;
    enddefine;


;;; Define a new abstract base class for distributed simulations.  We can't
;;; use this to add new slots to the sim_object class, however we can use it
;;; as a place to put an on update wrapper ...  Other ways to do this would
;;; be to redefine sim_object to inherit from an HLA_object mixin or to
;;; redefine all the classes in the simulation to inherit from an HLA_object
;;; mixin.  However, the latter means maintining two versions of a
;;; simulation: distributed and non-distributed.
define :class HLA_object adopting sim_object;
    ;;; Add a new wrapper to detetmine if this is a proxy, i.e. it is not
    ;;; simulated on this federate.  We use new rather than cons so that the
    ;;; decision as to whether this should be a proxy object can be based on
    ;;; the intial values of the slots.
    on new(o) do HLA_object_new_wrapper(o);

    ;;; Add an update wrapper to propagate the update to objects simulated
    ;;; on other federates.
    on update do HLA_object_update_wrapper;
    ;;;    on update do update_wrapper;

    ;;; Add a wrapper to remove this object from list of objects maintained
    ;;; by its federate and class.
    on destroy(o) do HLA_object_destroy_wrapper(o);
    enddefine;


;;; Define shared and if_needed slots for HLA_objects.  These can't be real
;;; slots as otherwise HLA_object couldn't adopt sim_object.

;;; HLA_attributeHandleSet is a vector of attributeHandles the id the RTI
;;; associates with the classHandle of this object.
define :shared_slot HLA_attributeHandleSet(o:HLA_object);
    HLA_attributeHandleSets(datakey(o));
    enddefine;

;;; HLA_instanceId is the unique id handed out by the RTI for this object.
define :if_needed HLA_instanceId(o:HLA_object);
    undef;
    enddefine;

;;; HLA_proxy_object is a boolean which indicates whether this object is a
;;; local proxy for an object or agent being simulated on another federate.
define :if_needed HLA_proxy_object(o:HLA_object);
    false;
    enddefine;

;;; HLA_object_owned_attributes is a list of the attributes of this object
;;; currently owned by this federate.
define :if_needed HLA_object_owned_attributes(o:HLA_object);
    [];
    enddefine;

;;; HLA_object_classTable implements a mapping from the names of classes
;;; (actually their datawords) to their corresponding HLA_class instances.
;;; This is rather clumsy but (a) it means the classes comprising the user's
;;; simulation don't have to be compiled prior to the HLA configuration
;;; stage, and (b) it avoids keeping this information in the federate.
define :shared_slot HLA_object_classTable(o:HLA_object);
    newassoc([])
    enddefine;

;;; Return the HLA_class instance corresponding to this HLA_object instance.
define :method HLA_object_class(o:HLA_object) -> hla_class;
    HLA_object_classTable(o)(dataword(o)) -> hla_class;
    enddefine;

;;; Set the HLA_class instance corresponding to this HLA_object instance.
define :method updaterof HLA_object_class(hla_class, o:HLA_object);
    hla_class -> HLA_object_classTable(o)(dataword(o));
    enddefine;

;;; HLA_classHandle is the id the RTI associates with the HLA class of this
;;; object.
define :method HLA_classHandle(o:HLA_object);
    HLA_classHandle(HLA_object_class(o));
    enddefine;



;;; on new wrapper which determines whether an object created on this
;;; federate should be simulated locally.  HLA_local_object_p is a predicate
;;; that returns false if an object is to be simulated on another federate.
;;; Proxy objects effectively form a free list of proxies which get
;;; reallocated to instanceIds of the appropriate type during the object
;;; discovery phase of sim_setup_scheduler.  Note that we can't just discard
;;; the proxies and recreate them later on object discovery in case the
;;; simulation we are distributing requires a return value, e.g., in an
;;; assignment.
define HLA_object_new_wrapper(o);
    if HLA_local_object_p(o) then
	;;; Register the instance, save the instanceId and update the
	;;; federate mapping from instanceIds to instances.
	registerObjectInstance(HLA_classHandle(o)) -> HLA_instanceId(o);
	o -> HLA_federate_instances(HLA_federate)(HLA_instanceId(o));

	lvars attribute;
    	for attribute in HLA_class_attributes(HLA_object_class(o)) do
	    if HLA_attribute_published(attribute) then
		;;; This is a local object.  Flush the initial values of the
		;;; published attributes to the RTI so that the proxies of
		;;; this object can be intialised.
		lvars value = valof(HLA_attribute_slot(attribute)(o);
	    	updateAttributeValues(HLA_instanceId(o), 
				      AttributeHandleValuePairSet,
                            	      theTime);

		;;; When an object is created, by default it owns the published
		;;; attributes of the corresponding HLA class.
	       	cons(attribute, HLA_object_owned_attributes(o)) 
	          -> HLA_object_owned_attributes(o);
	       	endif;
	    endfor; 


    else
	;;; This is a proxy.  Zap the slot values: this proxy is just shell
	;;; which will be reinitialised on object discovery.  All the slot
	;;; values required by the simulation on this federate should be set
	;;; by get_reflectAttributeValues(); if not, we may be more likely
	;;; to pick up problems if the slots have null values, though the
	;;; undef values may be overwritten if the user's simulation does
	;;; futher intialisation of its instances.  Note also that
	;;; class_slots does not return if_needed slots.
	lvars slot_p;
	for slot_p in class_slots(datakey(o)) do
	    undef -> slot_p(o);
	    endfor;

	true -> HLA_proxy_object(o);
	cons(o, HLA_federate_proxies(HLA_federate)) 
	  -> HLA_federate_proxies(HLA_federate);
	cons(o, HLA_federate_freelist(HLA_federate)) 
	  -> HLA_federate_freelist(HLA_federate);
	endif;	

    ;;; For this work, we need the HLA class of this object.
    cons(o, HLA_class_objects(HLA_object_class(o))) 
      -> HLA_class_objects(HLA_object_class(o));
    enddefine;

;;; Wrapper which is called before an update.  We assume that all attributes
;;; are exclusive, i.e. they can only be updated once per cycle.  If we
;;; publish the attribute and we currently own it, then we do the update and
;;; remember that we did so, so that we can refuse any transfer of ownership
;;; requests from other federates.  If we don't currently own the attribute,
;;; we cache the update and ask for ownership.  If we don't publish the
;;; attribute, we shouldn't be trying to update it, so print a warning.  If
;;; an attribute is neither published nor subscribed, we assume that anyone
;;; can perform an update at any time.
define HLA_object_update_wrapper(value, o, wrapped_p);
    ;;; Not clear if we can avoid explictly taking the args off the stack.
    ;;; Can we assume that each slot updater will take precisely two arguments?

    lvars slot_update = apply(% wrapped_p %),
	  attribute = false;
    for attribute in HLA_class_attributes(HLA_object_class(o)) do
	quitif(valof(HLA_attribute_slot(attribute)) = wrapper_invoker);
	;;; or we could use
	;;; quitif(HLA_attribute_slot(attribute) = pdprops(wrapper_invoker));
	endfor;
    
    if attribute then 
    	if HLA_attribute_published(attribute) then
	    if member(attribute HLA_object_owned_attributes(o)) then
	    	;;; Do the update.
	    	slot_update(value, o);
	    	;;; Notify the RTI
	    	updateAttributeValues(HLA_instanceId(o), 
				      AttributeHandleValuePairSet,
                            	      theTime);
	    	;;; Remember that we updated this slot, in case another
	    	;;; federate requests ownership
		cons([% HLA_instanceId(o), HLA_attributeHandle(attribute), value %],
		     HLA_federate_updates(HLA_federate)) 
		  -> HLA_federate_updates(HLA_federate);
	    else
	    	;;; Request transfer of ownership from the federate which
	    	;;; currently owns this attribute for this instance.
	    	attributeOwnershipAcquisitionIfAvailable(HLA_instanceId(o),
                                               	     	 AttributeHandleSet);
	    	;;; Cache the update so that we can do the update at the end
	    	;;; of this cycle if we can get ownership.  If we store them
	    	;;; in this order we can use: apply(explode(update))
		cons([% value, o, slot_p %],
		     HLA_federate_deferredUpdates(HLA_federate)) 
		  -> HLA_federate_deferredUpdates(HLA_federate);
	    	endif;
	    
    	elseif HLA_attribute_subscribed(attribute) then
	    ;;; Attempt to udpate an attribute which we don't publish.
	    ;;; Print a warning and discard the update.
	    printf();
    	else
	    ;;; An attribute which is part of the FOM, but which we neither
	    ;;; publish nor subscribe.  
	    ;;; *** Perhaps this should be as above: i.e., we can't update
	    ;;;     anything we don't explicitly publish ***

	    ;;;Do the update.
	    slot_update(value, o); 
	    endif;
    else
	;;; We assume that we can update attributes which don't are not
	;;; defined in the FOM.  Do the update.
	slot_update(value, o);
	endif;
    enddefine;

;;; Not clear if destroy wrappers are run when an object is unlinked
;;; (becomes garbage) or when the gc actually happens.  If it's the latter,
;;; this could leave dangling references around for a while.
define HLA_object_destroy_wrapper(o);
    ;;; Remove the object from the lists of objects maintained by its class.
    ncdelete(o, HLA_class_objects(HLA_object_class(o))) 
      -> HLA_class_objects(HLA_object_class(o));
    enddefine;


;;; sim_agent user-definable methods and procedures

;;; Save the original sim_agent defintion.
lvars procedure _sim_setup_scheduler = sim_setup_scheduler;

;;; Object discovery, proxy initialisation and time advance request are
;;; handled by the sim_agent user-definable procedure sim_setup_scheduler on
;;; the first and subsequent passes.  It would be neater to use
;;; sim_edit_objects_list to handle object discovery, but this is not called
;;; until after the first cycle of the scheduler.  This is too late for
;;; proxy instantiation.
define sim_setup_scheduler(sim_objects, sim_cycle_number);

    ;;; Redefine HLA_local_object_p to return true after the intial setup
    ;;; phase (i.e., all objects created on the federate after the
    ;;; simulation starts should be simulated on this federate).
    HLA_objects_always_local ->  HLA_local_object_p;

    _sim_setup_scheduler(sim_objects, sim_cycle_number);
    enddefine;

;;; get_reflectAttributeValues and updateAttributeValues are handled by the
;;; sim_agent user-definable procedure sim_post_cycle_actions.
;;; This is called after sim_do_actions has run, so any slot updates
;;; triggered by DO actions will already have been processed and their
;;; update requests sent to the RTI.
define sim_post_cycle_actions(sim_objects, sim_cycle_number);
    lvars callback;

    ;;; Post-cycle processing proceeds in four phases (we assume that all
    ;;; callbacks are processed in FIFO order):

    ;;; 1. Make a time advance request and wait (tick the RTI) until the
    ;;; request is granted (i.e., until all federates have reached the end
    ;;; of their simulation cycle.
    HLA_advance_time();

    ;;; Process object discovery callbacks from other federates.  We have to
    ;;; do this first as attribute updates may refer to a newly created
    ;;; object.
    for callback in objectDiscoveryCallbacks do
	;;; Assume that we get the instanceId and the classHandle of the new
	;;; object
	
	;;; Look for a proxy skeleton to use.  Any proxy with the same
	;;; classHandle will do ...
	lvars proxy = lisp_lemember(classHandle, 
				    HLA_federate_freelist(HLA_federate),
			 	    HLA_classHandle <> nonop =);
	
	if proxy /== [] then
	    hd(proxy) -> proxy;
            ;;; Assign the instanceId to this proxy
            instanceId -> HLA_instanceId(proxy);
	    ;;; Update the federate mapping from instanceIds to instances.
	    proxy -> HLA_federate_instances(HLA_federate)(instanceId);
 	    ;;; Remove it from the proxy freelist.  Note that the proxy is
            ;;; already in the proxy list and has been tagged with its
            ;;; HLA_class when the skeleton was created.
	    ncdelete(proxy, HLA_federate_freelist(HLA_federate)) 
	      -> HLA_federate_freelist(HLA_federate);
        else
	    ;;; Make a new proxy.  First we have to get the class key
	    ;;; corresponding to the classHandle.
	    lvars hla_class;
	    for hla_class in HLA_federate_classes(HLA_federate) do
		quitif(HLA_classHandle(hla_class) = classHandle);
		endfor;

	    HLA_make_proxy_object(valof(HLA_class_key(hla_class)), 
				  [HLA_instanceId ^instanceId]) -> proxy;
	    ;;; Update the federate mapping from instanceIds to instances.
	    proxy -> HLA_federate_instances(HLA_federate)(instanceId);
	    ;;; Add the proxy to the list of objects of this class
    	    cons(proxy, HLA_class_objects(hla_class)) 
      	      -> HLA_class_objects(hla_class);
	    ;;; Add it to the list of proxies for this federate.
	    cons(o, HLA_federate_proxies(HLA_federate)) 
	      -> HLA_federate_proxies(HLA_federate);

	    endif;
	endfor;

    ;;; Process attribute update callbacks from other federates.  We assume
    ;;; that the updates are legal, e.g., that no other federate will try to
    ;;; update a slot that we own.
    for callback in attributeUpdateCallbacks do
	;;; Assume that we get the instanceId, the attributeHandle and the value
	;;; Do we need to check whether we subscribe to this attribute /
	;;; instance?  Perhaps we should also check to see if we own the
	;;; attribute -- if we do then another federate shouldn't be trying
	;;; to update it
	lvars object = HLA_federate_instances(HLA_federate)(instanceId),
	      attribute = hd(lisp_lmember(attributeHandle, 
				       	  HLA_class_attributes(HLA_object_class(object)),
				       	  HLA_attributeHandle <> nonop =)),
	      slot_p = valof(HLA_attribute_slot(attribute));
		  
	;;; Do the local update
	value -> slot_p(object);
	endfor;

    ;;; Process change of attribute ownwership requests from other
    ;;; federates.  Grant the request if we haven't updated the attribute
    ;;; ourselves this cycle.
    for callback in owershipRequestCallbacks do
	;;; Assume that we get the instanceId and the attributeHandle
	;;; Do we need to check if we are publishing the attribute?
	lvars object = HLA_federate_instances(HLA_federate)(instanceId),
	      attribute = hd(lisp_lmember(attributeHandle, 
				       	  HLA_owned_attributes(object),
				       	  HLA_attributeHandle <> nonop =)),
	      update;

        ;;; Check to see if we have updated this attribute at this cycle
	;;; If so, jump to processing the next callback.
	for update in HLA_federate_updates(HLA_federate) do
	    quitif(HLA_instanceId(object) = update(1) and
	       	   HLA_attributeHandle(attribute) = update(2))(2);
	    endfor;

	;;; Transfer ownership and update the owned attributes list.
	unconditionalAttributeOwnershipDivesture(instanceId, AttributeHandleSet);
	ncdelete(attribute, HLA_owned_attributes(object)) 
	  -> HLA_owned_attributes(object);
	endfor;

    [] -> HLA_federate_updates(HLA_federate);

    ;;; 2. Make a time advance request and wait (tick the RTI) until the
    ;;; request is granted (i.e., until all federates have processed their
    ;;; change of ownership requests.)
    HLA_advance_time();

    ;;; Process attributes for which we have just acquired ownership (if
    ;;; any).  Update the local slot values and propagate the changes to the
    ;;; RTI.
    for callback in ownershipTransferCallbacks do
	;;; Assume that we get the instanceId and the attributeHandle
	lvars object = HLA_federate_instances(HLA_federate)(instanceId),
	      attribute = hd(lisp_lmember(attributeHandle, 
				       	  HLA_class_attributes(HLA_object_class(object)),
				       	  HLA_attributeHandle <> nonop =)),
	      slot_p = valof(HLA_attribute_slot(attribute)),
	      deferredUpdate;

	;;; Find the update that we wanted to perform.
	for deferredUpdate in HLA_federate_deferredUpdates(HLA_federate) do
	    quitif(HLA_instanceId(object) = update(1) and
	       	   HLA_attributeHandle(attribute) = update(2));
	    endfor;

	;;; Do the local update and propgate it to the RTI.  Note that we
	;;; don't have to remember that we did the update, as no one can ask
	;;; for an ownership transfer at this point.
	deferredUpdate(3) -> slot_p(object);
	updateAttributeValues(HLA_instanceId(o), 
			      AttributeHandleValuePairSet,
                              theTime);

	;;; It may be useful to know which ownership transfers were
	;;; unscuccesful so delete this update from the list of deferred
	;;; updates.
	ncdelete(update, HLA_federate_deferredUpdates(HLA_federate)) 
	  -> HLA_federate_deferredUpdates(HLA_federate);
	endfor;

    ;;; Collect all the reinitialisation code together at the end of the cycle?
    [] -> HLA_federate_deferredUpdates(HLA_federate);

    ;;; 3. Make a time advance request and wait (tick the RTI) until the
    ;;; request is granted (i.e., until all federates have generated
    ;;; attribute updates for any attributes they acquired at the last
    ;;; step.)
    HLA_advance_time();

    ;;; Process the remaining attribute update requests.
    for callback in attributeUpdateaCallbacks do
	;;; Assume that we get the instanceId, the attributeHandle and the value
	;;; Do we need to check whether we subscribe to this attribute /
	;;; instance?  Perhaps we should also check to see if we own the
	;;; attribute -- if we do then another federate shouldn't be trying
	;;; to update it
	lvars object = HLA_federate_instances(HLA_federate)(instanceId),
	      attribute = hd(lisp_lmember(attributeHandle, 
				       	  HLA_class_attributes(HLA_object_class(object)),
				       	  HLA_attributeHandle <> nonop =)),
	      slot_p = valof(HLA_attribute_slot(attribute));
		  
	;;; Do the local update
	value -> slot_p(object);
	endfor;

    ;;; 4. Make a time advance request and wait (tick the RTI) until the
    ;;; request is granted (i.e., until all federates have reached the end
    ;;; of post cycle processing.)
    HLA_advance_time();

    enddefine;

;;; Add wrappers for sim_run_agent so that we skip proxy objects.  If the
;;; user has redefined sim_run_agent for subclasses used in the simulation,
;;; then we have a problem.
define :wrapper sim_run_agent(object:sim_object, objects, default_method);
    unless HLA_proxy_object(object) then
	default_method(sim_run_agent)(object, objects);
	endunless;
    enddefine;

define :wrapper sim_run_agent(agent:sim_agent, objects, default_method);
    unless HLA_proxy_object(agent) then
	default_method(sim_run_agent)(agent, objects);
	endunless;
    enddefine;

;;; Barrier synchronisation: busy waits until all federates are ready to
;;; move to the next timestep.
define HLA_advance_time();
    HLA_federation_time(HLA_federation) + 1 
      -> HLA_federation_time(HLA_federation);
    ;;; Request that the federation time advance to the next logical
    ;;; timestep and
    timeAdvanceRequest(HLA_federation_time(HLA_federation));

    ;;; wait until all federates have made a similar request.
    enddefine;