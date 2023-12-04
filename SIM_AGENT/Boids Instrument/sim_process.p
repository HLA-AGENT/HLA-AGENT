load sim_boids.p
load sim_instrument.p
uses profile

999999999->popmemlim;
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
;;; least two different agents. 
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
;;; A rollback pattern occurs if agent1 updates a slot i and then some
;;; other agent (say agent2) accesses the same slot within some number
;;; of cycles. This number of cycles is defined by 'delay'
define count_rollback(listofcommonaccesses, delay)->num_rollback_patterns;
    lvars access,update; 
    0->num_rollback_patterns;
    for update in listofcommonaccesses do
	if(update.sim_datum_action="update") then
	    for access in listofcommonaccesses do
		if(access.sim_datum_action="access") and
		  (access.sim_datum_cycle > update.sim_datum_cycle) and
		  (access.sim_datum_cycle =< (update.sim_datum_cycle+delay)) 
		  and (access.sim_datum_actor /= update.sim_datum_actor)
		  then ;;; We have a rollback
		    1+num_rollback_patterns->num_rollback_patterns;
		    ;;; Can't have more than one rollback pattern per update
		    quitloop;
		endif;
	    endfor;
	endif;
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

define run_experiment(num_runs,num_cycles /* &optional config */);
    lvars num_runs,num_cycles;
    ;;; Places where changes for different simulations are required
    ;;; are marked ***

    lvars print_to_file = false,
	 sample_length = 20,
	 sample_gap = 20,
	 config=false,
	 common_access_list=[],
	 common_accesses=[],
	 rollbacktotals=[],
	 networkdelay=3,
	 rollbackpair,
	 x,y;
    
    ARGS num_runs,num_cycles, &OPTIONAL config:islist;

    if config then
	dl(config)->(print_to_file, sample_length, sample_gap)
    endif;
    print_to_file=>
    if(print_to_file) then
	;;;    	lvars stream=discout('results');
	lvars stream2=discout('resultsformat');
    	;;;	stream->cucharout;	
    endif;

    ;;; Define which slots/classes we will instrument in this exp
    sim_instrument_class(class_example(boid_key), [sim_x sim_y]);

    for y from 1 to num_runs do
	;;;Print info to screen and file indicating start of run
	['----Starting run number: ' ^y' ----']=>
	if print_to_file then 
	    fprintf(stream2,'----Run number %p----\n',[% y %]); 
	endif;
	
	;;;Reset lists for this run
	
	[]->sim_instrumentation_data;
	[]->sim_all_actors;
	lvars actors=[],slots=[],processedactors=[],processedslots=[];
	
	;;; ***Execute users simulation
	setup(num_cycles,2);
	
	;;;Process Actor information, processedactors is a lits of
	;;;sim_processed_instrument_actor
	process_data(sim_instrumentation_data,num_cycles) -> processedactors;
	
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
	    get_shared_accesses_in(agent1,
			       	   agent2,
			       	   x,
			       	   (x+sample_length-1))::common_access_list
	    ->common_access_list;
	    [^x ' to ' ^(x+sample_length-1)]=>
    	endfor;
	
	;;; Count the rollback patterns and number of updates. While
	;;; we cycle through we also print the number of common
	;;; accesses in each period to file (by checking the length of
	;;; the common_access for each period)
	for common_accesses in common_access_list do
	    count_rollback(common_accesses,networkdelay)::rollbacktotals
	    ->rollbacktotals ;
    	endfor;
	rollbacktotals=>
	if print_to_file then
	    ;;; Print common accesses as in 'reads' to file
	    
	    fprintf(stream2,'Common accesses',[]);
	    
	    for common_accesses in common_access_list do
		fprintf(stream2,' %p,',[% length(common_accesses) %]);
	    endfor;
	    fprintf(stream2,'\n',[]);
	    
	    ;;;;;; Print common update stuff to file
	    ;;;	    fprintf(stream2,'Common Updates',[]);
	    ;;;	    for rollbackpair in rollbacktotals do
	    ;;;	    	fprintf(stream2, '%p', [% back(rollbackpair) %]);
	    ;;;	    endfor;
	    ;;;	    fprintf(stream2,'\n',[]);
	    
	    ;;; Print rollback pattern numbers
	    fprintf(stream2,'Rollback Patterns',[]);
	    for rollbackpair in rollbacktotals do
	    	fprintf(stream2, ' %p,', [% rollbackpair %]);
	    endfor;
	    fprintf(stream2,'\n',[]);
	endif;
    endfor;    
enddefine;

;;; sim_process.p ends here
vars sim_process = true;
