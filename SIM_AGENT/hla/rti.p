;;; An update wrapper which propagates a slot (attribute) update to objects
;;; simulated on other federates.  Note that slot update wrappers are not
;;; called during object creation -- this is handy here, but it's not clear
;;; if this is the correct behaviour in general.
define RTI_update_attribute(o, upd_p); 
    ;;; first we need to get the name of slot from the update procedure upd_p

    ;;; then we need to check whether this slot corresponds to an attribute
    ;;; which we publish in the FOM

    ;;; if so, we extract the new value of the attribute from the slot
    ;;; (since the slot has already been updated when the wrapper is called)
    ;;; and propagate it to the RTI.  

    ;;; Get the accessor corresponding to this updater.
    
    ;;; Debugging only
    printf('Updating the slot: %p %p\n', [^o ^upd_p]);
    enddefine;


;;; Debugging only.
define registerObjectInstance(classHandle) -> instanceId;
    201 -> instanceId;
    enddefine;


;;; Note: can we avoid triggering the wrapper by using set_slots?  If not,
;;; we could use method wrappers on the slot accessors and wrapper_deref to
;;; strip the wrappers from a method when we need to avoid RTI propagation.
