
;;;This is the user simulation on which to perform analysis and obtain
;;;traces. There must be a setup(num_cycles, num_agents) procedure
;;;implemented.
load sim_boids.p

;;;Other includes.
load sim_instrument.p
uses profile

999999999->popmemlim;

vars unique_timestamp=false;

;;; The form produced in sim_instrument is a global list called
;;; sim_instrumentation_data. This is a flat list of the class
;;; sim_instrument_datum. Which is basically a flat list of all
;;; accesses and updates that occurred during the simulation. The
;;; procedures in this file take this flat list and produce a new list
;;; of sim_processed_instrument_actor where each
;;; sim_processed_instrument_actor in the list represents an actor in
;;; the simulation, ie something which updated or accessed an
;;; instrumented slot.
;;;TW.new_hole_prob     = 0.1;
;;;TW.new_tile_prob     = 0.1;
;;;TW.new_obstacle_prob = 0.1;

define :class sim_processed_instrument;
    slot num_accesses==0;  ;;;Number of accesses made
    slot num_updates==0;   ;;;Number of updates made
    slot accesslist;   ;;;vector of sim_instrument_datum, one for each access
    slot updatelist;   ;;;vector of sim_instrument_datum, one for each update
    enddefine;

;;; This final output once sim_instrumentation_data has been processed
;;; is a list of these objects. There is an instance of this class
;;; created for each actor (something which made at least one access
;;; or update) in the simulation. 
define :class sim_processed_instrument_actor is sim_processed_instrument;
    slot sim_datum_actor;  ;;;Actors name eg. Agent23
    slot sim_datum_class;
      enddefine;

define newsim_processed_instrument_actor(name,array_size)->actor;
    instance sim_processed_instrument_actor;
	sim_datum_actor=name;
	accesslist=newarray([1 ^array_size], procedure(x); []; endprocedure);
	updatelist=newarray([1 ^array_size], procedure(x); []; endprocedure);
    endinstance ->actor;
enddefine;

    
;;; The sim_instrumentation_data can be processed in a different way
;;; so the resulting output is given in terms of slots (state
;;; variables) rather than actors
define :class sim_processed_instrument_slot is sim_processed_instrument;
    slot sim_datum_slot;   ;;;The slot concerned
    slot sim_datum_object; ;;;The object which the slot belongs
    slot actor_updates==[]; ;;; A list of the actors which updated the slot
    slot actor_accesses==[]; ;;; A list of the actors which accessed the slot
    enddefine;

define :class sim_actor_total;
    slot sim_datum_actor;
    slot sim_number==0;
enddefine;
    
;;; Defined as fprintf in C, prints to specified stream (file)
define fprintf(stream, format_str, args);  
    procedure();
        dlocal cucharout = stream;	
	printf(format_str, args);
    endprocedure();
enddefine;

  
;;; Prints out the final processed list of data (ie. list of
;;; sim_processed_instrument_actor)
define print_actors(processedactors);
    lvars actor;
    for actor in processedactors do
	[********]=>
	[actor name: ^(actor.sim_datum_actor)]=>
	[Number of accesses made by 
	 ^(actor.sim_datum_actor): ^(actor.num_accesses)]=>
	[Number of updates made by 
	 ^(actor.sim_datum_actor): ^(actor.num_updates)]=>
	[Class: ^(actor.sim_datum_class)]=>
	[********]=>
    endfor;
enddefine;

;;; Used when printing to file, in comma seperated format (eg. for excel)
define print_actors2(processedactors,stream);
    lvars actor;
    for actor in processedactors do
	fprintf(stream,'%p %p, ',[% actor.sim_datum_actor, 
				    actor.sim_datum_class %]);
	fprintf(stream,'%p, ',[% actor.num_accesses %]);
	fprintf(stream,'%p, ',[% actor.num_updates %]);
	fprintf(stream,'\n',[]);
    endfor;
enddefine;

define print_slots(processedslots);
    lvars aslot;
    for aslot in processedslots do
	[********]=>
	[Slot: ^(aslot.sim_datum_slot)]=>
	[Object which contains slot: ^(aslot.sim_datum_object)]=>
	[Number of accesses made to  this slot: ^(aslot.num_accesses)]=>
	[Number of updates made to this slot: ^(aslot.num_updates)]=>
	[The actors which accessed the slot are:  ^(aslot.actor_accesses)]=>
	[The actors which updated the slot are:  ^(aslot.actor_updates)]=>
	[********]=>
    endfor;
enddefine;

;;; Used in create_processed_data. Object list is a list of
;;; sim_processed_instrument_actor The procedure returns true if it
;;; contains a sim_processed_instrument_actor with its sim_datum_actor
;;; slot equal to actor_name. 
define member_actor(actor_name,object_list)->result;
    lvars name;
    false->result;
    for name in object_list do
	if actor_name=name.sim_datum_actor then
	    true->result;
	endif;
    endfor;
enddefine;

;;; This procedure is used in create_processed_data_slots,
;;; get_shared_accesses,get_slots. list_of_accesses is a list of
;;; sim_processed_instrument_slot. It returns true if the list already
;;; contains an instance of sim_processed_instrument_slot with the
;;; same object and slot names
define member_access(access, list_of_accesses)->result;
    lvars name;
    false->result;
    for name in list_of_accesses do
	if access.sim_datum_object=name.sim_datum_object 
		and access.sim_datum_slot=name.sim_datum_slot then
	    true->result;
	endif;
    endfor;
enddefine;

define member_cycle(access,accesslist)->result;
    lvars item;
    false->result;
    for item in accesslist do
	if (item.sim_datum_object=access.sim_datum_object) and 
		(item.sim_datum_slot=access.sim_datum_slot) then
	    true->result;
	endif;
    endfor;
enddefine;

;;; Creates a list of all the actors in the sim_instrumentation_data,
;;; ie all the agents/objects who accessed an instrumented slot.
define get_actors(instrumentation_data)->actorlist;
    lvars data,actorlist=[];
    for data in instrumentation_data do
	if member(data.sim_datum_actor,actorlist) then 	
	else
	    cons(data.sim_datum_actor,actorlist)->actorlist;
	endif;
    endfor;
enddefine;

;;; Creates a list of all the slots updated or accessed during the simulation
define get_slots(instrumentation_data)->slotlist;
    lvars data,slotlist=[];
    for data in instrumentation_data do
	unless member_access(data,slotlist) then 	
	    cons(data,slotlist)->slotlist;
	endunless;
    endfor;
enddefine;



;;; Takes a list of actors names ([agent26 agent27...] obtained as the
;;; result of using get_actors) and creates a list of corresponding
;;; sim_processed_instrument_actor objects, with sim_datum_actor
;;; instantiated to actors name. 
define create_processed_data(actorlist,num_cycles)->listofproccessedobjects;
    lvars name,newdata,listofproccessedobjects=[];
    for name in actorlist do
	unless member_actor(name,listofproccessedobjects) then 	
	    newsim_processed_instrument_actor(name,num_cycles)->newdata;
	    cons(newdata,listofproccessedobjects)->listofproccessedobjects;
	endunless;
    endfor;
enddefine;

;;; Takes a list of slots and creates a list of corresponding
;;; sim_processed_instrument_slots objects.
define create_processed_data_slots(slotlist)->listofproccessedslots;
    lvars aslot,newdata,listofproccessedslots=[];
    for aslot in slotlist do
	if member_access(aslot, listofproccessedslots) then 	

	else
	    newsim_processed_instrument_slot()->newdata;
	    aslot.sim_datum_object->newdata.sim_datum_object;
	    aslot.sim_datum_slot->newdata.sim_datum_slot;
	    cons(newdata,listofproccessedslots)->listofproccessedslots;
	endif;
    endfor;
enddefine;


;;; This can be used on the list of processed actors before running
;;; get_shared_accesses. This way you only get the common accesses
;;; between the tile_agents in the simulation and ignore the
;;; environment. Specific for tileworld but tile_agent_key can be replaced.
;;;define get_tile_agents(listofprocessedactors)->tileagents;
;;;    lvars i;
;;;    []->tileagents;
;;;    for i in listofprocessedactors do
;;;    	if(sim_datum_class(i)=tile_agent_key) then
;;;    	    cons(i,tileagents)->tileagents;
;;;	endif;
;;;    endfor;
;;;enddefine;

;;; After the property is converted to the format produced by datalist
;;; (inside get_shared_accesses_in), this procedure goes through the
;;; list removing items which haven't been accessed/updated by at
;;; least two different agents. Each list in propertylist is of the form...
;;;[[agent3|sim_y]
;;;     [<actor: agent3 action: update slot: sim_y object: agent3 cycle: 5>
;;;      <actor: agent3 action: access slot: sim_y object: agent3 cycle: 5>
;;;      <actor: agent3 action: update slot: sim_y object: agent3 cycle: 4>
;;;      <actor: agent3 action: access slot: sim_y object: agent3 cycle: 4>
;;;      <actor: agent3 action: update slot: sim_y object: agent3 cycle: 3>
;;;      <actor: agent3 action: access slot: sim_y object: agent3 cycle: 3>
;;;      <actor: agent3 action: update slot: sim_y object: agent3 cycle: 2>
;;;      <actor: agent3 action: access slot: sim_y object: agent3 cycle: 2>
;;;      <actor: agent3 action: update slot: sim_y object: agent3 cycle: 1>
;;;      <actor: agent3 action: access slot: sim_y object: agent3 cycle: 1>]]
;;; This indicates who accessed/updated the sim_y slot of agent3. 

define get_common(propertylist)->shared;
    []->shared;
    
    lvars multipleactors=false,list,item;
    for list in propertylist do
	;;;First check if more than one access made, if not theres no
	;;;point checking if two different agents accessed it.
	if (length(last(list))>1) then
	    lvars actorname = sim_datum_actor(hd(last(list)));
	    false->multipleactors;
 	    ;;; If more than one access was made we then check to see
	    ;;; if if at least two agents updated accessed it
	    for item in last(list) do
		if (actorname /= sim_datum_actor(item)) then
		    true->multipleactors;
		    quitloop;
		endif;
	    endfor;
	    ;;; If we saw multiple actors (agent accesses) then add these
	    ;;; accesses to shared list
	    if(multipleactors) then
		last(list)<>shared->shared;
	    endif;
	endif;
    endfor;
enddefine;

;;; This procedure is similar to the get_shared_accesses_in method except a
;;; list of actors is passed and the result is the shared access (reads and
;;; write) between all actors in the list. <listofactors> is a list of
;;; sim_processed_instrument_actor, usually produced by the process data
;;; procedure.

define get_all_shared_accesses_in(listofactors,initialtime, endtime)->shared;
    lvars slot_datalist,i,item,o1, shared =[];
    
    ;;; Join the lists into a single property
    lvars slot_prop = 
	  newanyproperty([], 20, 1, false, syshash, nonop =, "tmparg", [], false);
    	  
    ;;; Put all the relavant (within time window) values into the
    ;;; property. Each (key,value) pair in the property is of the form.
    ;;; the key is itself a pair, (objectID, slotID)
    ;;; and each key maps to Value:- list of all access/updates made
    ;;; to that slot
    for o1 in listofactors do
    	for i from initialtime to endtime do
	    ;;; Store all the access made by o1
	    for item in accesslist(o1)(i) do
	    	lvars objId = item.sim_datum_object,
	     	      slotId = item.sim_datum_slot;	    
	    	cons(item,slot_prop(conspair(objId,slotId)))->
	    	slot_prop(conspair(objId,slotId));
	    	endfor;
    	    ;;; Store all the updates made by o1
    	    for item in updatelist(o1)(i) do
	    	lvars objId = item.sim_datum_object,
	      	      slotId = item.sim_datum_slot;	    
	    	cons(item,slot_prop(conspair(objId,slotId)))->
	    	slot_prop(conspair(objId,slotId));
	    	endfor;	    
    	    endfor;
	endfor;

    ;;; Flatten the property into a flat list
    datalist(slot_prop)->slot_datalist;
    
    ;;; Then pick out all the common access/updates made by o1 and o2
    get_common(slot_datalist)->shared;
    enddefine;    


;;; This returns the common accesses which occurred between two actors
;;; during the time period indicated by the third and fourth argument
;;; (time is in cycles)
define :method get_shared_accesses_in(o1:sim_processed_instrument_actor,
		       		   o2:sim_processed_instrument_actor,
				       initialtime, endtime)->shared;
    lvars access, access2, update, update2,
	 accessin, access2in,
	 accessinmatched=false,
	 slot_datalist,i,item;

    []->shared;

    ;;; Join the lists into a single property
    lvars slot_prop = newanyproperty([], 20, 1, false, syshash, 
	       				  nonop =, "tmparg", [], false);

    ;;; Put all the relavant (within time window) values into the
    ;;; property. Each (key,value) pair in the property is of the form.
    ;;; the key is itself a pair, (objectID, slotID)
    ;;; and each key maps to Value:- list of all access/updates made
    ;;; to that slot
    for i from initialtime to endtime do
	;;; Store all the access made by o1
	for item in accesslist(o1)(i) do
	    lvars objId = item.sim_datum_object,
	     	 slotId = item.sim_datum_slot;	    
	    cons(item,slot_prop(conspair(objId,slotId)))->
	    slot_prop(conspair(objId,slotId));
	endfor;
	;;; Store all the updates made by o1
	for item in updatelist(o1)(i) do
	    lvars objId = item.sim_datum_object,
	     	 slotId = item.sim_datum_slot;	    
	    cons(item,slot_prop(conspair(objId,slotId)))->
	    slot_prop(conspair(objId,slotId));
	endfor;
	;;; Store all the accesses made by o2
	for item in accesslist(o2)(i) do
	    lvars objId = item.sim_datum_object,
	     	 slotId = item.sim_datum_slot;	    
	    cons(item,slot_prop(conspair(objId,slotId)))->
	    slot_prop(conspair(objId,slotId));
	endfor;
	;;; Store all the accesses made by o2
	for item in updatelist(o2)(i) do
	    lvars objId = item.sim_datum_object,
	     	 slotId = item.sim_datum_slot;	    
	    cons(item,slot_prop(conspair(objId,slotId)))->
	    slot_prop(conspair(objId,slotId));
	endfor;
    endfor;

    ;;; Flatten the property into a flat list
    datalist(slot_prop)->slot_datalist;
    
    ;;; Then pick out all the common access/updates made by o1 and o2
    get_common(slot_datalist)->shared;
enddefine;






;;; This procedure takes the list of common accesses produced by
;;; get_shared_accesses_in and counts the number of rollback patterns.
;;; A rollback pattern occurs if agentx updates a slot i and then some
;;; other agent (say agenty) accesses the same slot within some number
;;; of cycles. This number of cycles is defined by <delay>
define count_rollback(listofcommonaccesses, delay)->rollbacks;
    lvars access,
	  update,
    	  rollbacks=[],
	  seen_actors=[];
    for update in listofcommonaccesses do
	if(update.sim_datum_action="update") then
	update.sim_datum_actor::seen_actors->seen_actors;
	    for access in listofcommonaccesses do
		if(access.sim_datum_action="access") and 
		    (access.sim_datum_cycle > update.sim_datum_cycle) and 
		    (access.sim_datum_cycle =< (update.sim_datum_cycle+delay)) 
		    and (access.sim_datum_slot = update.sim_datum_slot)
		    and (access.sim_datum_object = update.sim_datum_object) 
 		    and (not(member(access.sim_datum_actor,seen_actors))) then 
		    ;;; We have a rollback
		    conspair(update,access)::rollbacks->rollbacks;
	            ;;;We've had a rollback on this actor so we can't have
	            ;;;another as a consequence of this update
                    access.sim_datum_actor::seen_actors->seen_actors;
			endif;
	    	    endfor;
		endif;
    	    ;;;Reset seen_actors for next update
    	    []->seen_actors;
    	    endfor;
	enddefine;
     
    
;;; instantiates the accesslist and updatelist of the
;;; sim_processed_instrument_actor;
define get_accesses(processedactors,instrumentation_data, num_cycles);
    lvars actor,data,newactors=[],i;
    for actor in processedactors do	  
	for i from 1 to num_cycles do
	    for data in instrumentation_data do
		
	      	if(data.sim_datum_actor=actor.sim_datum_actor) then
	      	  data.sim_datum_class->actor.sim_datum_class;
		  
		  if(data.sim_datum_action="access") and
			  (data.sim_datum_cycle = i)  then

		      unless(member_cycle(data,accesslist(actor)(i))) then
	      	      cons(data,accesslist(actor)(i))->accesslist(actor)(i);
 		      actor.num_accesses+1->actor.num_accesses;
		  endunless;
		  elseif(data.sim_datum_action="update") and
			  (data.sim_datum_cycle = i)  then
		      unless(member_cycle(data,updatelist(actor)(i))) then
		      cons(data,updatelist(actor)(i))->updatelist(actor)(i);
		      actor.num_updates+1->actor.num_updates;
		  endunless;
    		  endif;		  
	      endif;
	  endfor;	  	 	  
      endfor;
  endfor;
enddefine;


;;; instantiates the accesslist and updatelist of the
;;; sim_processed_instrument_slots;
define get_accesses_slots(processedslots,instrumentation_data);
    lvars aslot,data,newactors=[];
    for aslot in processedslots do	  
	for data in instrumentation_data do
	    if(data.sim_datum_slot=aslot.sim_datum_slot) and 
		    (data.sim_datum_object=aslot.sim_datum_object) then
		;;;If its an access
		if(data.sim_datum_action="access") then
		    cons(data,aslot.accesslist)->aslot.accesslist;
		    aslot.num_accesses+1->aslot.num_accesses;
		    unless(member(data.sim_datum_actor,aslot.actor_accesses)) then
	      	      	cons(data.sim_datum_actor, 
			     aslot.actor_accesses)->aslot.actor_accesses
		    endunless;
		;;;If its an update
		elseif(data.sim_datum_action="update") then
		    cons(data,aslot.updatelist)->aslot.updatelist;
		    aslot.num_updates+1->aslot.num_updates;
		    unless(member(data.sim_datum_actor,aslot.actor_updates)) then
		      	cons(data.sim_datum_actor, 
			     aslot.actor_updates)->aslot.actor_updates
		    endunless;
		endif;		  
	    endif;
	endfor;	  	 	  
    endfor;
enddefine;


;;; Process_data takes the raw data produced in
;;; sim_instrumentation_data and re-organises making it possible to
;;; analyse the data. process_data returns a list of
;;; sim_process_instrument_actors
define process_data(sim_instrumentation_data,num_cycles)->processedactors;    
    lvars processedactors = [],actors=[];
    ;;;Firstly get the names of actors
    get_actors(sim_instrumentation_data)->actors;
    ;;;Then create instances of sim_processed_instrument_actor for each
    create_processed_data(actors,num_cycles)->processedactors;
    ;;;Then instantiate the access, update lists of the
    ;;;sim_processed_instrument_actors
    get_accesses(processedactors,sim_instrumentation_data,num_cycles);
enddefine;


define remove_duplicates(list)->newlist;
    lvars newlist=[];
    for item in list do
	if(not(member(item,newlist))) then
	item::newlist->newlist;
	    endif;
	endfor;
    enddefine;


;;; Performs trace outputs and analyses critical accesses The user
;;; simulation loaded at the top of the file must implement the
;;; setup(num_cycles, num_agents) procedure
define run_experiment(num_runs,num_cycles /* &optional config */);
    lvars num_runs,num_cycles;
    ;;; Places where changes for different simulations are required
    ;;; are marked ***
    lvars num_agents=2,
	  print_to_file = false,
	  sample_length = 20,
	  sample_gap = 20,
	  config=false,	 
	  networkdelay=3,
	  rollbackpair,
	  period,
	  x,y;
    
    ARGS num_runs,num_cycles, &OPTIONAL config:islist;
    
    if config then
	dl(config)->(num_agents, WIN_X, WIN_Y, print_to_file, sample_length, sample_gap);
    	endif;
    
    if(num_cycles mod (sample_length+sample_gap) /= 0) then
	['**Warning Incorrect sample length and gap specified']=>
	sysexit();
	endif;
    
    
    
    ;;; Define which slots/classes we will instrument in this exp
    sim_instrument_class(class_example(boid_key), [sim_x sim_y]);
    
    for y from 1 to num_runs do
	cleargensymproperty();
	;;;Print info to screen and file indicating start of run
	sysobey('mkdir run'><y);
	'run'><y->current_directory;

	if(print_to_file) then
	;;;    	lvars stream=discout('results');
	lvars stream2=discout('resultsformat '><WIN_X><'x'><WIN_Y><' - '><sys_convert_date(sys_real_time(),true));
	lvars stream3=discout('variables.dat');
    	;;;	stream->cucharout;	
    	endif;
	
	['----Starting run number: ' ^y' ----']=>
	
	if print_to_file then 
	    fprintf(stream2,'----Run number %p----\n',[% y %]); 
	    endif;
	
	;;;Reset lists for this run
	
	[]->sim_instrumentation_data;
	[]->sim_all_actors;
	lvars actors=[],
	      slots=[],
	      Processedactors=[],
	      processedslots=[],
	      common_access_list=[],
	      common_accesstotals=[],
	      common_accesses=[],
	      rollbacktotals=[],
	      cycle_periods=[],
	      variablelist=[];
	
	;;; ***Execute users simulation
	setup(num_cycles,num_agents);
	
	;;;Process Actor information, processedactors is a lits of
	;;;sim_processed_instrument_actor
	process_data(sim_instrumentation_data,num_cycles) -> processedactors;


	;;;Print out all variables in the simulation, for read into AASSK
	if print_to_file then 
	    ;;;	    lvars sim_instrumentation_data_rev = 
	    ;;;		  remove_duplicates(sim_instrumentation_data);
	    for item in sim_instrumentation_data do
		lvars v=conspair(item.sim_datum_object,item.sim_datum_slot);

		if(not(member(v,variablelist))) then
     		    v::variablelist->variablelist;
		    fprintf(stream3, '%p %p = 0\n', [%v.front, v.back%]);
	    	    endif
		
	    	endfor;
	    variablelist=>
	    endif;


	;;;Print out all the accesses (reads and writes) to the dat file so
	;;;it can be read in by AASSK.
	for item in processedactors do
	    lvars stream4=discout(item.sim_datum_actor><'.dat');	
 	    lvars accesses = newarray([1 %num_cycles%]);
	    for x from 1 to num_cycles do	
		accesslist(item)(x)<>updatelist(item)(x) -> accesses(x);
	  	endfor;
	    lvars next_time_stamp=0;
	    for x from 1 to num_cycles do 
		for item in accesses(x) do
		    next_time_stamp+1->next_time_stamp;
		    if unique_timestamp then 
		    	next_time_stamp->sim_datum_cycle(item);
			endif;
	    	    fprintf(stream4, '%p\n', [%item%]);
		    endfor;
		endfor;	
	    endfor;	    
	;;;Print info on actors to screen and file
	print_actors(processedactors);
	if print_to_file then print_actors2(processedactors,stream2); endif;
	
	;;;***Select which actors we testing for common accesses
	lvars agent1 = hd(processedactors);
	lvars agent2 = hd(tl(processedactors));
	
	;;; When checking for common accesses the user specifies a
	;;; period of cycles in which to check for common
	;;; accesses. The user also specifies a gap (in number of
	;;; cycles) between each of these periods.
	;;; For example if we had a simulation of 200 cycles, a
	;;; sample_gap of 20 and a sample_length of 20 then 5 calls
	;;; would be made to get_shared_accesses_in. The first would
	;;; be between the cycle period [1,20], the second [41,60],
	;;; third [81,100] etc. If the gap was changed from 20 to 40
	;;; the periods would be [1,20], [61,80] [121,140] etc.
	;;; The results from get_shared_accesses_in is itself a list,
	;;; common_access_list is therefore a list of lists.
	for x from 1 by (sample_gap+sample_length) to num_cycles do
	    lvars rollbacks=[];
	    lvars sa=get_all_shared_accesses_in(processedactors,
			       	       		      x,
			       	       		      (x+sample_length-1));
		
  	    ;;; Common access made between agents
  	    sa::common_access_list->common_access_list;
   	    length(sa)::common_accesstotals->common_accesstotals; 
	  
	    ;;; Info on cycle periods used in analysis
	    conspair(x, (x+sample_length-1))::cycle_periods->cycle_periods;

	    ;;;Rollback info
	    count_rollback(sa,networkdelay)->rollbacks;
	    length(rollbacks)::rollbacktotals->rollbacktotals;

	    ['Common access total' ^(length(sa)) 'rollbacks' ^(length(rollbacks)) 'for' ^x 'to' ^(x+sample_length-1)]=>	   

    	    endfor;

	;;; Count the rollback patterns and number of updates. While
	;;; we cycle through we also print the number of common
	;;; accesses in each period to file (by checking the length of
	;;; the common_access for each period)

	if print_to_file then
	    ;;; Print cycle periods in which we are measuring.	    
	    fprintf(stream2,'Common periods,',[]);
	    for period in cycle_periods do
		fprintf(stream2,' %p to %p,',
			[% front(period), back(period) %]);
	    	endfor;
	    fprintf(stream2,'\n',[]);
	    
	    ;;; Print common accesses to file
	    fprintf(stream2,'Common accesses,',[]);
	    for common_accesses in common_access_list do
		fprintf(stream2,' %p,',[% length(common_accesses) %]);
	    	endfor;
	    fprintf(stream2,'\n',[]);	    	    	    
	    
	    ;;; Print rollback pattern numbers
	    
	    fprintf(stream2,'Rollback Patterns,',[]);
	    for rollbackpair in rollbacktotals do
	    	fprintf(stream2, ' %p,', [% rollbackpair %]);
	    	endfor;
	    fprintf(stream2,'\n',[]);
	    endif;
	    '..'->current_directory;
    	endfor;    
    enddefine;

;;; sim_process.p ends here
vars sim_process = true;
