/* ---- Copyright University of Nottingham 2002. All rights reserved. ------
 > File:            /home/bsl/pop/sim_instrument/sim_instrument.p
 > Purpose:         Slot access and update instrumentation for SIM_AGENT.
 > Author:          Brian Logan, Nov 12 2002
 > Documentation:
 > Related Files:
 > RCS Info:        $Id: sim_instrument.p,v 1.1 2002/11/12 18:38:59 bsl Exp bsl $
 */


uses objectclass;
uses newkit;
uses sim_agent;

;;; This file contains the defintions of the following procedures and methods:
vars procedure sim_record_slot_access,
     procedure sim_record_slot_update
    ;

;;; Global variable used to record slot accesses and updates.
vars sim_instrumentation_data = [];
vars sim_all_actors =[];
;;; used to stop recursion
vars sim_glbl_instrument_on = true;
vars sim_wrappers_on = true;


;;; Class to record an instrumentation event: actor did action to slot of
;;; object at cycle.
define :class sim_instrument_datum;
    slot sim_datum_actor;
    slot sim_datum_action;
    slot sim_datum_slot;
    slot sim_datum_object;
    slot sim_datum_cycle;
    slot sim_datum_class;
    enddefine;



define :method print_instance(d:sim_instrument_datum);
    printf('<actor: %p action: %p slot: %p object: %p cycle: %p>',
	   [% d.sim_datum_actor, d.sim_datum_action, d.sim_datum_slot,
	      d.sim_datum_object, d.sim_datum_cycle %]);
    enddefine;


;;; Abstract base class for instrumented simulations.  We can't use this to
;;; add new slots to the sim_object class, however we can use it as a place
;;; to put wrappers.
define :class sim_instrument adopting sim_object;

    ;;; Add an access wrapper to record when a slot is accessed.
    on access(o, acc_p) do sim_record_slot_access(o, acc_p);

    ;;; Add an update wrapper to record when a slot is updated.
    on update(o, upd_p) do sim_record_slot_update(o, upd_p);

    enddefine;

;;; Define shared and if_needed slots for instrumented objects.  These can't
;;; be real slots as otherwise sim_instrument couldn't adopt sim_object.
;;; Objectclass uses tmpval (?) properties, so it should be safe to use
;;; objects as keys.

;;; A property which records whether a given class is instrumented.
define :shared_slot sim_instrumented_classes(o:sim_instrument);
    newmapping([], 20, false, true);
    enddefine;

;;; A property which records whether a given object is instrumented.
define :if_needed sim_instrumented_objects(o:sim_instrument);
    newassoc([]);
    enddefine;

;;; A property which records whether a given object is instrumented.
;;; Perhaps we should distinguish between accesses and updates?
define :shared_slot sim_instrumented_slots(o:sim_instrument);
    newassoc([]);
    enddefine;

;;; Instrument all calls to <slots> in class <o>.  If <slots> is true, then
;;; instruments all slots of <o>.
define :method sim_instrument_class(o:sim_instrument, slots);
    lvars key = datakey(o),
	  slot_p;

    if slots == true then
	class_slots(key) -> slots;
	endif;

    for slot_p in slots do
    	switchon slot_p
	    case .isprocedure then
	    	true -> sim_instrumented_classes(o)(conspair(key, pdprops(slot_p)));
	    case .isword then
	    	true -> sim_instrumented_classes(o)(conspair(key, slot_p));
	    	endswitchon;
	endfor;
    enddefine;

;;; Instrument all calls to <slots> in object <o>.  If <slots> is true, then
;;; instruments all slots of <o>.
define :method sim_instrument_object(o:sim_instrument, slots);
    lvars key = datakey(o),
	  slot_p;

    if slots == true then
	class_slots(key) -> slots;
	endif;

    for slot_p in slots do
    	switchon slot_p
	    case .isprocedure then
	    	true -> sim_instrumented_objects(o)(pdprops(slot_p));
	    case .isword then
	    	true -> sim_instrumented_objects(o)(slot_p);
	    	endswitchon;
	endfor;
    enddefine;

;;; Instrument all calls to <slot_p> in any object.
define sim_instrument_slot(slot_p);
    lvars o = class_example(sim_instrument_key);

    switchon slot_p
	case .isprocedure then
	    true -> sim_instrumented_slots(o)(pdprops(slot_p));
	case .isword then
	    true -> sim_instrumented_slots(o)(slot_p);
	    endswitchon;
    enddefine;




;;; Recording slot accesses and updates.
define in_range(agent,obj)->answer;
    lvars dist=sim_distance(agent,obj),
      	 (proc,range,num)=destvector(hd(sim_sensors(agent)));

    (dist<=range)->answer;
enddefine;

;;; Wrapper to record an access to object <o> by accessor <acc_p>.
define sim_record_slot_access(o, acc_p);
    lvars slot_name = pdprops(wrapper_invoker), ;
    
    ;;; Assume that we only want to collect data when sim_agent is running,
    ;;; i.e., when sim_myself has a value.
    if (sim_glbl_instrument_on) then
    unless isundef(sim_myself) then
    	    if (sim_instrumented_slots(o)(slot_name) or
	    	sim_instrumented_objects(o)(slot_name) or
	    	sim_instrumented_classes(o)(conspair(datakey(o), slot_name))) then
		false->sim_glbl_instrument_on;
		;;; Make sure the access is in range
		if(in_range(sim_myself,o)) then
	    	    cons(instance sim_instrument_datum;
	    	     	     sim_datum_actor = sim_name(sim_myself);
	    	     	     sim_datum_action = "access";
	    	     	     sim_datum_slot = slot_name;
	    	     	     sim_datum_object = sim_name(o);
	    	     	     sim_datum_cycle = sim_cycle_number;
			     sim_datum_class = datakey(sim_myself);
	     	     	 endinstance, sim_instrumentation_data) -> sim_instrumentation_data;
	    	endif;
		true->sim_glbl_instrument_on;
    	    endif;
    	endunless;
    endif;
enddefine;


;;; Wrapper to record an update to object <o> by updater <upd_p>.
define sim_record_slot_update(o, upd_p);
    lvars slot_name = pdprops(wrapper_invoker), ;

    ;;; Assume that we only want to collect data when sim_agent is running,
    ;;; i.e., when sim_myself has a value.
    unless isundef(sim_myself) then
    	if (sim_instrumented_slots(o)(slot_name) or
	    sim_instrumented_objects(o)(slot_name) or
	    sim_instrumented_classes(o)(conspair(datakey(o), slot_name))) then
	    false->sim_glbl_instrument_on;
	    ;;; Make sure the update is occuring within range
	    if(in_range(sim_myself,o)) then
	    	cons(instance sim_instrument_datum;
	    	     	 sim_datum_actor = sim_name(sim_myself);
	    	     	 sim_datum_action = "update";
	    	     	 sim_datum_slot = slot_name;
	    	     	 sim_datum_object = sim_name(o);
	    	     	 sim_datum_cycle = sim_cycle_number;
		     	 sim_datum_class = datakey(sim_myself);
	     	     endinstance, sim_instrumentation_data) -> sim_instrumentation_data;
	    endif;
	    true->sim_glbl_instrument_on;
	    unless member(sim_myself,sim_all_actors) then 	
	    	cons(sim_myself,sim_all_actors)->sim_all_actors;
	    endunless;
	endif;
    endunless;
enddefine;


define :method sim_run_sensors(agent:sim_object, agents) -> sensor_data;
    ;;; Default method for running all the sensors associated with an
    ;;; agent. Done just before an agent is "run" by the scheduler.
    lvars
	 other, sensor, sensor_proc, sensor_range,
	 sensors = sim_sensors(agent), dist;
    
    if null(sensors) then []->sensor_data;
    else
	;;; make a list of records of detected agents
	[%
	  for other in agents do
	      ;;; Turn off instrumenting while checking if things are in range
	      false->sim_glbl_instrument_on;
	      sim_distance(agent,other) -> dist;
	      true->sim_glbl_instrument_on;
	      for sensor in sensors do
		  ;;; assume each sensor is a vector in format
		  ;;; {sensorname range}, where sensorname is name of a method
		  ;;; of form: sensor(agent1, agent2, distance)
		  ;;; ??? should this use recursive_valof on the args ???
		  appdata(sensor, recursive_valof)
		  -> (sensor_proc, sensor_range);
		  if dist <= sensor_range then
		      ;;; this may leave result on stack, a list to go into
		      ;;; the database, later
		      sensor_proc(agent, other, dist);
		  endif;
	      endfor
	  endfor
	  %] -> sensor_data
    endif
enddefine;

;;; sim_instrument ends here
vars sim_instrument = true;
