;;; First attempt at an extension of sim_agent to allow distributed
;;; simulation using the HLA.

uses objectclass;
uses newkit;
uses sim_agent;

uses fom;
uses rti;

;;; This file contains the defintions of the following procedures:
vars procedure HLA_objects_always_local,
     procedure HLA_local_object_p,
     procedure HLA_allocate_proxy_object,
     procedure HLA_make_proxy_object;

;;; A user-definable predicate which determines if an object is to be
;;; simulated on this federate.  To distribute an existing simulation it may
;;; be sufficient to give each federate an appropriate defintion of
;;; HLA_local_object_p.  As far as the (non-distributed) simulation startup
;;; code is concerned, each federate appears to create all the objects in
;;; the simulation, but some objects will be proxies.  The decision can be
;;; based on, for example, the name of the agent (if this is meaningful),
;;; the number of objects already created, the position in which the object
;;; is created and so on.  Simulations will probably want to reset this to
;;; return true after the intial setup phase (i.e., all objects created on
;;; the federate after the simulation starts should be simulated on this
;;; federate).
vars procedure HLA_objects_always_local = procedure(o) -> bool;
    	                                      true -> bool;
				 	      endprocedure;

vars procedure HLA_local_object_p = HLA_objects_always_local;

;;; Allocate a proxy object.  This can either come from the
;;; HLA_proxy_freelist, or or be constructed from scratch.
define HLA_allocate_proxy_object(type /* &optional inital_values */) -> proxy;
    ;;; Somehow we need to associate the type_key with the type or generate
    ;;; the type_key from the type.
    lvars type, initial_values = [];
    ARGS type, &OPTIONAL initial_values:islist;

    enddefine;

;;; Make a proxy object corresponding to an object being simulated on another
;;; federate.  <type> is the type of the object (typically sim_object,
;;; sim_agent or one of their subclasses.  Attribute values for proxy
;;; objects are normally determined by RTI update after object discovery,
;;; but if we know them at the time of creation then they can be set using
;;; the <intial_values> argument.
define HLA_make_proxy_object(type /* &optional inital_values */) -> proxy;
    ;;; Somehow we need to associate the type_key with the type or generate
    ;;; the type_key from the type.
    lvars type, initial_values = [];
    ARGS type, &OPTIONAL initial_values:islist;

    create_instance(valof(consword(type >< '_key')), 
		    [HLA_proxy_object ^true] <> initial_values) -> proxy;
    enddefine;

