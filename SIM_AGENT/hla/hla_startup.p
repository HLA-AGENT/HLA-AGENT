/* --- Copyright University of Nottingham 2002. All rights reserved. ------
 > File:            /home/bsl/pop/hla/hla_startup.p
 > Purpose:         Distributing sim_agent using the High Level Architecture
 > Author:          Brian Logan, Sep 23 2002
 > Documentation:   Public interface to the hla library.
 > Related Files:   hla/*
 > RCS Info:        $Id: pop-mode.el,v 1.22 2001/03/06 19:17:52 bsl Exp $
 */

;;; The steps in loading and running a distributed simulation are as follows:
;;; 1. load the standard sim_agent library
;;; 2. load the hla library
;;; 3. load the user's configuration file and initialise this federate --
;;;    the configuation file specifies which of the classes defined in the
;;;    FOM are relevant to this federate and which of the attributes of
;;;    these classes are published and subscribed by the federate.
;;; 4. load the code for the user's simulation -- this defines the classes
;;;    which constitute the user's simulation; it may also intialise the
;;;    user's simulation and start it running.
;;; 5. intialise and start the simulation (if loading the user's simulation
;;;    didn't initialise the simulation or call sim_scheduler).

;;; The tricky bit is that step (3) requires knowledge of the classes and
;;; slots in the user's simulation.  We get round this by indirecting
;;; through the names of the classes and their slots, since we don't have
;;; the relevant datakeys util step (4) is complete.

uses objectclass;
uses newkit;
uses sim_agent;

;;; This file contains the defintions of the following procedures:
vars procedure HLA_sim,
     procedure HLA_initialise_federate,
     procedure HLA_start_federate,
     procedure HLA_load_configuration,
     procedure HLA_configure_federate
    ;


;;; Global variables containing information about this federate.  These
;;; should be set in the configuration file.
global vars 
    ;;; A string naming this execution
    HLA_execName,
    ;;; The name of the file which describes the ?federation?
    HLA_fedFile,
    ;;; A string naming this federate
    HLA_fedName,
    ;;; An association list specifying the configuration of this federate
    HLA_FOM_alist
    ;;; The name of a file which, when loaded, will load the user's simulation
    HLA_simFile,
    ;;; A procedure which performs any neccessary initialistation of the
    ;;; user's simulation and/or calls sim_scheduler
    HLA_simInit_p = identfn
    ;


;;; Run the simulation specified in the configuration file <configFile>.
define HLA_sim(configFile);
    HLA_initialise_federate(configFile);
    HLA_start_federate(HLA_simFile, HLA_simInit_p);
    enddefine;

;;; Intialise the federate.  This creates the federation execution, joins
;;; the federation and publishes and subscribes the attributes defined in
;;; the user's configuration file.
define HLA_initialise_federate(configFile);
    ;;; Load the user's configuration file
    HLA_load_configuration(configFile);

    ;;; Initialise the federate ambassador for this federate.
    getFedAmbRef() -> HLA_federateAmbassadorRef(HLA_federate);

    ;;; Create the federation and join it.
    createFederationExecution(HLA_executionName(HLA_federation), 
			      HLA_federationFile(HLA_federation));
    joinFederationExecution(HLA_federateName(HLA_federate), 
			    HLA_executionName(HLA_federation), 
			    HLA_federateAmbassadorRef(HLA_federate));

    ;;; Should this be here or later?
    HLA_configure_federate(HLA_federate_configuration(HLA_federate));

    ;;; Can we set up Time Mangement here?
    enableTimeConstrained();
    enableTimeRegulation(fedTime, lookahead);

    ;;; What about attribute/instance ownership?
    enddefine;

;;; Start the simulation running on this federate.  <file> is the name of a
;;; file to be loaded containing the user's simulation code.  If <file> does
;;; not contain a call to sim_scheduler, then the <start_p> argument must be
;;; provided.  <start_p> should be a procedure of no arguments which
;;; performs any necessary initialisation of the user's simulation together
;;; with the desired call to sim_scheduler.  It defaults to identfn.
;;; Creation of the objects to be simulated on this (and possibly other)
;;; federate(s) triggers registration of object instances (and possibly
;;; creation of proxy objects).  Object discovery, proxy initialisation and
;;; time advance request are handled by the sim_agent user-definable
;;; procedure sim_setup_scheduler on the first and subsequent passes.
;;; get_reflectAttributeValues and updateAttributeValues are handled by the
;;; sim_agent user-definable procedure sim_post_cycle_actions.
define HLA_start_federate(file /* &optional start_p */ );
    lvars file, start_p = identfn;
    ARGS file, &OPTIONAL start_p:isprocedure;

    load(file);
    start_p();

    ;;; Shut down the execution.
    disableTimeConstrained();  ;;; Not defined in rti.p
    disableTimeRegulation();   ;;; Not defined in rti.p

    resignFederationExecution();
    destroyFederationExecution(HLA_execution_name);
    enddefine;

;;; Load the configuration file and initialise the singleton classes holding
;;; iformation about the federate and this federate.
define HLA_load_configuration(filename);
    if trycompile(filename) then
 	HLA_execName -> HLA_executionName(HLA_federation);
    	HLA_fedFile -> HLA_federateFile(HLA_federation);

    	HLA_fedName,  -> HLA_federateName(HLA_federate);
    	HLA_FOM_alist -> HLA_federate_configuration(HLA_federate);
	
    else
	mishap('federate configuration file not found', [^filename]);
	endif;
    enddefine;

;;; Process the configuration information for this federate which specifies
;;; which classes and attributes from the FOM we are interested in.  Assumes
;;; that the RTI is running to generate class and attribute handles.  As a
;;; side effect, this handles publishing and subscrbing attributes to the
;;; RTI and initialises the property used to map from simulation class names
;;; to the corresponding HLA class instances.  keys.
define HLA_configure_federate(FOM_alist);
    lvars class_spec;

    for class_spec in FOM_alist do
	HLA_make_class(class_entry);
	endfor;
    enddefine;

;;; hla_startup ends here
vars hla_startup = true;
