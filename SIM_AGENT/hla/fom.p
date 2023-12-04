;;; Code to handle the FOM

;;; This file contains the defintions of the following procedures and methods:
vars procedure HLA_class_published,
     procedure HLA_class_subscribed,
     procedure HLA_publish_attribute,
     procedure HLA_subscribe_attribute,
     procedure HLA_unpublish_attribute,
     procedure HLA_unsubscribe_attribute,
     procedure HLA_configure_federate,
     procedure HLA_make_class,
     procedure HLA_make_attribute,
     ;

uses lisp_utils;

;;; For published attributes (those which we are updating locally), we need
;;; to map from the attribute name, (which we assume is the same as the slot
;;; name) to the attributeHandle.  For subscribed attributes (those which
;;; can also or only be updated by other federates), we need to map from the
;;; attributeHandle to the slot updater.

;;; Class to hold information about classes defined in the FOM.  Note that
;;; we don't hold a reference to the federate as this is represented by a
;;; singleton class.
define :class HLA_class;
    ;;; A string naming the class
    slot HLA_class_name;
    ;;; The classHandle allocated to the class by the RTI
    slot HLA_classHandle;
    ;;; A list of HLA_attribute instances, one for each attribute of this
    ;;; class in the FOM
    slot HLA_class_attributes == [];
    ;;; The name of the corresponsing objectclass class (actually its
    ;;; dataword) or false if there is no corresponding class.
    slot HLA_class_key == false;
    ;;; A list of the HLA_objects of this class
    slot HLA_class_objects == [];
    enddefine;

;;; Compute the published and subscribed attributeHandleSets

;;; Returns the attributeHandleSet of published attributes for this class.
define :method HLA_class_published(c:HLA_class) -> attributeHandleSet;
    lvars attribute;

    [% for attribute in HLA_class_attributes(c) do
	   if HLA_attribute_published(attribute) then
	       HLA_attributeHandle(attribute);
	       endif;
	   endfor; %] -> attributeHandleSet;
    enddefine;
    
;;; Returns an attributeHandleSet of subscribed attributes for this class.
define :method HLA_class_subscribed(c:HLA_class) -> attributeHandleSet;
    lvars attribute;

    [% for attribute in HLA_class_attributes(c) do
	   if HLA_attribute_subscribed(attribute) then
	       HLA_attributeHandle(attribute);
	       endif;
	   endfor; %] -> attributeHandleSet;
    enddefine;




;;; A Class to hold information about attributes defined in the FOM.
;;; Note: we assume that all attributes are "exclusive", i.e., they can be
;;; updated at most once per cycle.
define :class HLA_attribute;
    ;;; A string naming the attribute
    slot HLA_attribute_name;
    ;;; The HLA_class instance of which this is an attribute
    slot HLA_attribute_class;
    ;;; The attributeHandle allocated to the attribute by the RTI.  Note
    ;;; that the namespace is not flat, and this is only guaranteed to be
    ;;; unique within the context of the class of which this is an
    ;;; attribute.
    slot HLA_attributeHandle;
    ;;; The datatype used by the RTI to represent values of this attribute
    ;;; *** We may not need this ***
    slot HLA_attribute_type;
    ;;; Whether we publish this attribute of this class
    slot HLA_attribute_published == false;
    ;;; Whether we subscribe to this attribute of this class
    slot HLA_attribute_subscribed == false;
    ;;; The name of the objectclass generic function corresponding to this
    ;;; attribute or false if there is none.
    slot HLA_attribute_slot;
    enddefine;



;;; The following procedures may be useful if we want to change set of
;;; attributes published or subscribed during the simulation.  However, at
;;; the moment, it is not clear how the RTI handles this or what the
;;; arguments to the Pop procedures should be, e.g., the name of the
;;; attribute, the attribute handle or an attribute instance.

;;; Publish an attribute from this federate
define HLA_publish_attribute(name);
    lvars attribute = hd(lisp_lmember(name, HLA_FOM_attributes,
			     	      HLA_attribute_name <> nonop =));
    ;;; Tell the RTI that we want to publish this attribute
    ;;; publishObjectClassAttributes(attributeHandle, attributeHandleSet);

    true -> HLA_attribute_published(attribute);
    enddefine;

;;; Subscribe to an attribute published by another federate.
define HLA_subscribe_attribute(name);
    lvars attribute = hd(lisp_lmember(name, HLA_FOM_attributes,
			     	      HLA_attribute_name <> nonop =));
    ;;; Tell the RTI that we want to subscribe to this attribute

    true -> HLA_attribute_subscribed(attribute);
    enddefine;

;;; Stop publishing an attribute we were publishing from this federate.
define HLA_unpublish_attribute(name);
    lvars attribute = hd(lisp_lmember(name, HLA_FOM_attributes,
			     	      HLA_attribute_name <> nonop =));
    ;;; Tell the RTI that we no longer want to publish this attribute

    false -> HLA_attribute_published(attribute);
    enddefine;

;;; Stop subscribing to an attribute published by another federate.
define HLA_unsubscribe_attribute(name);
    lvars attribute = hd(lisp_lmember(name, HLA_FOM_attributes,
			     	      HLA_attribute_name <> nonop =));
    ;;; Tell the RTI that we want to subscribe to this attribute

    false -> HLA_attribute_subscribed(attribute);
    enddefine;


;;; Information about classes and attributes is held in an alist with
;;; entries of the form: [<className> <classProperties>] where
;;; <classProperties> is an alist of the form: [[sim_class name] [attributes
;;; <attributeList>]].  <attributeList> is another alist with entries of the
;;; form: [<attributeName> <attributeProperties>].  <attributeProperties> is
;;; yet another alist of the form [[type <typespec>] [slot <slotname>]
;;; [published <bool>] [subscribed <bool>]].  If an attribute is either
;;; published or subscribed, it must have a slot and a type.

;;; Make an HLA_class instance from a class_spec.
define HLA_make_class(class_spec);
    lvars (class_name, class_props) = dl(class_spec),
	  hla_class = instance HLA_class;
	                  HLA_class_name = class_name;
			  HLA_classHandle = getObjectClassHandle(class_name);
			  endinstance,
	  class_prop,
	  attribute;

    if (lassoc("sim_class") ->> class_prop) then
	;;; We indirect through the class name (which should be the dataword
	;;; of the class) in case the class key doesn't exist (i.e., the
	;;; classes comprising the user's simulation typically won't have
	;;; been compiled at this point).
	hd(tl(class_prop)) -> HLA_class_key(hla_class);
	;;; Put the class name directly into the classTable for HLA_object, as
	;;; the HLA_object_class updater expects a class instance, not the
	;;; name of its key.
	hla_class -> HLA_object_classTable(HLA_class_key(hla_class));
	endif;

    if (lassoc("attributes") ->> class_prop) then
	maplist(hd(tl(class_prop)), HLA_make_attribute(% hla_class %)) 
	  -> HLA_class_attributes(hla_class);

	;;; Notify the RTI of the published and subscribed attributeHandleSets
	publishObjectClass(HLA_classHandle(hla_class),
			   HLA_class_published(hla_class));
	subscribeObjectClassAttributes(HLA_classHandle(hla_class),
			   	       HLA_class_subscribed(hla_class));
        endif;

    ;;; Add the class to the list of classes for this federate.
    cons(hla_class, HLA_federate_classes(HLA_federate)) 
      -> HLA_federate_classes(HLA_federate);
    enddefine;

;;; Make an HLA_attribute from an attribute_spec.
define HLA_make_attribute(attribute_spec, hla_class);
    lvars (attribute_name, attribute_props) = dl(attribute_spec),
	  hla_attribute = instance HLA_attribute;
	                      HLA_attribute_name = attribute_name;
			      HLA_attribute_class = hla_class;
			      HLA_attribute_handle 
			        = getAttributeHandle(attribute_name, 
						     HLA_classHandle(hla_class));
			      endinstance,
	  attribute_prop;

    if (lassoc("sim_slot") ->> attribute_prop) then
	;;; As with classes, we indirect through the name of the generic
	;;; function, so that the classes and methods comprising the user's
	;;; simulation don't have to be compiled at this point.
	hd(tl(attribute_prop)) -> HLA_attribute_slot(hla_attribute);
	endif;

    ;;; We may not need the type if we use serialisation: then we should be
    ;;; able to send most of the pop types without even needing their
    ;;; datakey ...
    if (lassoc("type") ->> attribute_prop) then
	valof(hd(tl(attribute_prop))) -> HLA_attribute_type(hla_attribute);
	endif;

    if (lassoc("published") ->> attribute_prop) then
	valof(hd(tl(attribute_prop))) -> HLA_attribute_published(hla_attribute);
	endif;

    if (lassoc("subscribed") ->> attribute_prop) then
	valof(hd(tl(attribute_prop))) -> HLA_attribute_subscribed(hla_attribute);
	endif;

    ;;; Sanity check
    if (HLA_attribute_published(hla_attribute) or 
	HLA_attribute_subscribed(hla_attribute)) and 
	not(HLA_attribute_type(hla_attribute) and 
	    HLA_attribute_slot(hla_attribute)) then
	mishap('malformed attribute specification', attribute_spec);
	endif;
    enddefine;
