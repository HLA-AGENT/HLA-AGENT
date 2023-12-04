
/*
Creates a prey flocking behaviour based on a follow-the-leader algorithm.
*/

/*
METHOD   : find_leader (wld) -> py
INPUTS   : wld is the world agent
OUTPUTS  : py is a prey agent
USED IN  : assign_leader
CREATED  : Mar 1999
PURPOSE  : Finds which prey is the current leader from all the posiible prey
agents
TESTS:
trial_setup();
add_prey(8,2,400,650,150,50,100,4,22,2,600,3);
run_agents(1);
true -> leader(py1);
find_leader(sim_world)=>
*/

define :method find_leader(wld:world) -> py;

    lvars details,agent_name,agent,py_name;
    ;;; Iterate through list of visible prey
    for details in visible_prey(wld) do
        ;;; Check to see whether agent is the leader
        details --> ! [?agent_name =];
        valof(agent_name) -> agent;
        ;;; If it is then return it
        if leader(agent) = true then
            agent -> py;
            return();
        endif;
    endfor;

    ;;; If no leader is present then just return the first agent from the list
    hd(visible_prey(wld)) --> ! [?py_name =];
    valof(py_name) -> py;

enddefine;


/*
METHOD   : assign_leader (wld)
INPUTS   : wld is the world agent
OUTPUTS  : NONE
USED IN  : assign_leader_ruleset
CREATED  : Mar 1999
PURPOSE  : Every x cycles (random number between 0 & 30 although this can be
altered) picks a new leader and stops the old one.

TESTS:
*/

define :method assign_leader(wld:world);

    ;;; Stop if there are no prey on the world
    if visible_prey(wld) = [] then
        return();
    endif;
    lvars py_name,py,f_lead,lead2,details,agent_name,agent;

    ;;; Pick a prey agent at random
    lvars lead = oneof(visible_prey(wld));

    ;;; If the leader has run all its cycles
    if leader_count(wld) = 1 then
        ;;; Find the leader, stop it being the leader and return its colour to
        ;;; normal
        find_leader(wld) -> f_lead;
        false -> leader(f_lead);
        [[COLOUR ^prey_colour WIDTH 2 [CIRCLE {0 0 ^agent_size}]]]
            -> pic_list(f_lead);
        ;;; Get the new leader, make it the leader and turn it red
        lead --> ! [?py_name ==];
        valof(py_name) -> py;
        true -> leader(py);
        [[COLOUR 'red' WIDTH 2 [CIRCLE {0 0 ^agent_size}]]]
            -> pic_list(py);
        ;;; Get the number of cycles the leader will run for
        random(30) -> leader_count(wld);
    ;;; Else decrement the number of cycles the leader has left
    else
        leader_count(wld) - 1 -> leader_count(wld);
    endif;

    find_leader(wld) -> lead2;

    ;;; Let all the prey know the leader's position
    for details in visible_prey(wld) do
        details --> ! [?agent_name ==];
        valof(agent_name) -> agent;
        rc_picx(lead2) -> leader_x(agent);
        rc_picy(lead2) -> leader_y(agent);
    endfor;

enddefine;


/*
METHOD   : flock_to_leader (prey) -> (new_x, new_y)
INPUTS   : prey is a prey agent
OUTPUTS  : new_x, new_y
  Where  :
    new_x is the new x coordinate
    new_y is the new y coordinate
USED IN  : perform_action_ruleset
CREATED  : Mar 1999
PURPOSE  : Calculates the new coordinates for a prey agent to head towards
the current leader
TESTS:
trial_setup();
'leader' -> flock_type;
add_prey(10,2,400,650,150,50,100,4,22,2,600,3);
run_agents(200);
*/

define :method flock_to_leader(prey:prey_agent) -> (new_x,new_y);

    ;;; If the agent is the leader then move randomly
    if leader(prey) = true then
        random_search(prey) -> (new_x,new_y);
        return();

    ;;; If the leader is within flocking range move towards it
    elseif distance(rc_picx(prey),rc_picy(prey),leader_x(prey),leader_y(prey))
        <= prey_flocking_range(prey) then
        move_towards_point(rc_picx(prey),rc_picy(prey),
            prey_flocking_speed(prey),leader_x(prey),leader_y(prey))
            -> (new_x,new_y);

    ;;; If the agent is too close to the leader then move away from it
    elseif distance(rc_picx(prey),rc_picy(prey),leader_x(prey),leader_y(prey))
        < flock_separation then
        move_away_from_point(rc_picx(prey),rc_picy(prey),
            prey_flocking_speed(prey),leader_x(prey),leader_y(prey))
            -> (new_x,new_y);

    ;;; If the leader is too far away then move to the nearest prey agent
    elseif distance(rc_picx(prey),rc_picy(prey),leader_x(prey),leader_y(prey))
        > prey_flocking_range(prey) then
        flock_to_prey(prey) -> (new_x,new_y);
    ;;; Default move randomly
    else
        random_search(prey) -> (new_x,new_y);
    endif;

    ;;; Adjust movement to keep a minimum distance away from other agents
    in_personal_space(prey,new_x,new_y) -> (new_x,new_y);

    if in_object(prey,new_x,new_y) then
        0 -> random_target_count(prey);
        random_search(prey) -> (new_x,new_y);
    endif;

enddefine;
