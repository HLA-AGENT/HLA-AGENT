;;; Sample configuration file for a distributed HLA simulation

;;; The user's configuration file should define the following variables:
vars 
    ;;; A string naming this execution
    HLA_execName = 'TileWorld run 1',

    ;;; The name of the file which describes the ?federation?
    HLA_fedFile = 'fedFile',

    ;;; A string naming this federate
    HLA_fedName = 'TileWorld environment',

    ;;; An association list specifying the configuration of this federate.
    ;;; Information about classes and attributes in the FOM is held in an
    ;;; alist with entries of the form: [<className> <classProperties>]
    ;;; where <classProperties> is an alist of the form: [[sim_class name]
    ;;; [attributes <attributeList>]].  <attributeList> is another alist
    ;;; with entries of the form: [<attributeName> <attributeProperties>].
    ;;; <attributeProperties> is yet another alist of the form [[type
    ;;; <typespec>] [slot <slotname>] [published <bool>] [subscribed
    ;;; <bool>]].  If an attribute is either published or subscribed, it
    ;;; must have a slot and a type.
    HLA_FOM_alist = 
    [['tileagent' 
      [['position_x' [[subscribed true]
		      [slot position_x]]]
       ['position_y' [[subscribed true]
		     [slot position_y]]]
       ]] 
     ['tile' 
      [['position_x' [[published true]
		      [slot position_x]]]
       ['position_y' [[published true]
		      [slot position_y]]]
       ['colour' [[published true]]]
       ]] 
     ['obstacle' 
      [['position_x' [[published true]
		      [slot position_x]]]
       ['position_y' [[published true]
		      [slot position_y]]]]]
     ]

    ;;; The name of a file which, when loaded, will load the user's
    ;;; simulation.  This following example assumes that the user's
    ;;; simulation is in a file 'tw.p' (or that loading the file tw.p will
    ;;; cause the user's simulation to be loaded) and that loading this file
    ;;; will create the objects required by the simulation and call
    ;;; sim_scheduler.  Alternatively, the user can call
    ;;; HLA_start_simulation with a second argument, which should be a
    ;;; procedure of no arguments which performs any necessary
    ;;; initialisation of the user's simulation together with the desired
    ;;; call to sim_scheduler.
    HLA_simFile = 'tw.p',
    
    ;;; A procedure which performs any neccessary initialistation of the
    ;;; user's simulation and/or calls sim_scheduler 
    HLA_simInit_p = identfn
    ;


;;; The user may also wish to redefine the procedure HLA_local_object_p
;;; which is used to determine if a given object in the user's simulation is
;;; to be simulated on this federate.  To distribute an existing simulation
;;; it may be sufficient to give each federate an appropriate defintion of
;;; HLA_local_object_p.  As far as the user's simulation startup code is
;;; concerned, each federate appears to create all the objects in the
;;; simulation, but some objects will be proxies (i.e., will be actually run
;;; on other federates).  The decision can be based on, for example, the
;;; name of the agent (if this is meaningful), the number of objects already
;;; created, the position in which the object is created and so on.  For
;;; example, for consistency with the name of the federate defined above,
;;; this should probably be redefined so that only objects forming the
;;; tileworld environment are "local".  Note: by default, this is redefined
;;; to return true after the intial setup phase (i.e., all objects created
;;; on the federate after the simulation starts should be simulated on this
;;; federate).
vars procedure HLA_local_object_p = HLA_objects_always_local;