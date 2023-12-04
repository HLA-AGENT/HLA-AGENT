/*
RULESET  : start_turn_ruleset
CREATED  : Nov 1998
PURPOSE  : Ruleset which places any required initial information into the
objects database.
*/

define :ruleset start_turn_ruleset;

    [DLOCAL [prb_allrules = true]];

    ;;; Rule to place agents starting position into its database.
    RULE start_move
    [WHERE isworld(sim_myself) = false]
    [NOT started]
        ==>
    [started]
    [LVARS  [posit_x = rc_picx(sim_myself)]
            [posit_y = rc_picy(sim_myself)]
    ]
    [last_position ?posit_x ?posit_y]


    RULE get_visible_obstacles
    [new_sense_data obstacle ?obs ==]
        ==>
    ;;;[SAY 'I am working']
    [LVARS [name = sim_name(obs)]
           [x = rc_picx(obs)]
           [y = rc_picy(obs)] ]
    [POP11 update_obstacles(sim_myself,name,x,y)]

    ;;; Check whether the agent has been caught
    RULE already_caught
    [caught]
    ==>
    [STOPAGENT]

    RULE remove_old_predators
        ==>
    [POP11 [] -> visible_predators(sim_myself)]


    RULE get_visible_predators
    [new_sense_data predator ?pr ?dist ==]
        ==>
    [POP11 update_predators(sim_myself,pr,dist)]


    RULE remove_old_prey
        ==>
    [POP11 [] -> visible_prey(sim_myself)]


    RULE get_visible_prey
    [new_sense_data prey ?pr ?dist ==]
        ==>
    [POP11 update_prey(sim_myself,pr,dist)]

    ;;; Check agents energy level
    RULE check_energy
    [WHERE isworld(sim_myself) = false]
    ;;; If energy is 0 then the agent is dead
    [WHERE energy(sim_myself) <= 0]
        ==>
    ;;; Remove agent from simulation
    [POP11 remove_agent(sim_myself)]
    ;;; Update counters
    [POP11 if ispredator_agent(sim_myself) then
                1 + predator_starve -> predator_starve;
           elseif isprey_agent then
                1 + prey_starve -> prey_starve;
           endif;]
    [STOPAGENT]

enddefine;


/*
RULESET  : get_food_ruleset
CREATED  : Feb 1999
PURPOSE  : Gets all visible food in the world, and updates food lists
*/

define :ruleset get_food_ruleset;

    [DLOCAL [prb_allrules = true]];

    RULE remove_old_food
        ==>
    [POP11 [] -> visible_food(sim_myself)]

    RULE get_visible_food
    [new_sense_data food ?fd ?dist ==]
      ==>
    [POP11 update_food(sim_myself,fd,dist)]

enddefine;

/*
RULESET  : clear_data_ruleset
CREATED  : Nov 1998
PURPOSE  : Removes old data from agent's database
*/

define :ruleset clear_data_ruleset;

    [DLOCAL [prb_allrules = true]];

    RULE remove_old_data
    [new_sense_data ==]
    ==>
    [NOT new_sense_data ==]

    RULE remove_old_ghosts
    [ghost ==]
    ==>
    [NOT ghost ==]

enddefine;



/*
RULESET  : am_i_caught_ruleset
CREATED  : Nov 1998
PURPOSE  : Checks whether a prey agent has been caught, and removes it if it
has.
*/

define :ruleset am_i_caught_ruleset;

    RULE have_i_been_caught
    [WHERE have_i_been_caught(sim_myself)]
        ==>
    [caught]
    [STOPAGENT]

enddefine;


/*
RULESET  : have_i_eaten_ruleset
CREATED  : Feb 1999
PURPOSE  : Checks whether a prey agent has eaten any food.
*/

define :ruleset have_i_eaten_ruleset;

    RULE eat
        ==>
    [POP11 have_i_eaten_food(sim_myself)]

enddefine;



/*
RULESET  : reduce_vars_ruleset
CREATED  : Feb 1999
PURPOSE  : Reduces energy levels and random_heading_count in agents
*/

define :ruleset reduce_vars_ruleset;

    [DLOCAL [prb_allrules = true]];

    RULE reduce_energy
    ==>
    [POP11 reduce_energy(sim_myself)]

    RULE reduce_heading_count
    [WHERE random_target_count(sim_myself) > 0]
    ==>
    [POP11 random_target_count(sim_myself) - 1
            -> random_target_count(sim_myself)]


enddefine;


/*
RULESET  : perform_action_ruleset
CREATED  : Feb 1999
PURPOSE  : Performs the required action for an agent
*/

define :ruleset perform_action_ruleset;

    RULE search
    [action search]
        ==>
    [LVARS new_x new_y]
    ;;; Produce new coordinates
    [POP11 random_search(sim_myself) -> (new_x,new_y)]
    ;;; Move agent
    [do move_agent ?new_x ?new_y]
    ;;; Update position data
    [NOT action ==]

    RULE search_fast
    [action search_fast]
        ==>
    [LVARS new_x new_y]
    ;;; Produce new coordinates
    [POP11 random_search_fast(sim_myself) -> (new_x,new_y)]
    ;;; Move agent
    [do move_agent ?new_x ?new_y]
    ;;; Update position data
    [NOT action ==]

    RULE forage
    [action forage]
    ==>
    [LVARS new_x new_y]
    ;;; Produce new coordinates
    [POP11 head_to_food(sim_myself) -> (new_x,new_y)]
    [do move_agent ?new_x ?new_y]
    ;;; Update position data
    [NOT action ==]

    RULE forage_fast
    [action forage_fast]
    ==>
    [LVARS new_x new_y]
    ;;; Produce new coordinates
    [POP11 head_to_food_fast(sim_myself) -> (new_x,new_y)]
    [do move_agent ?new_x ?new_y]
    ;;; Update position data
    [NOT action ==]

    RULE flock
    [action flock]
        ==>
    [LVARS new_x new_y]
    ;;; Produce new coordinates
    [POP11
        ;;; Check what type of flocking is being used
        if flock_type = 'none' then
            random_search(sim_myself) -> (new_x,new_y);

        elseif flock_type = 'point' then
            flock_to_prey(sim_myself) -> (new_x,new_y);

        elseif flock_type = 'group' then
            flock_to_a_group(sim_myself) -> (new_x,new_y);


        elseif flock_type = 'herd' then
            flock_to_a_herd(sim_myself) -> (new_x,new_y);

        elseif flock_type = 'leader' then
            flock_to_leader(sim_myself) -> (new_x,new_y);

        elseif flock_type = 'boids' then
            flock_boids(sim_myself) -> (new_x,new_y);
        endif]
    [do move_agent ?new_x ?new_y]
    ;;; Update position data
    [NOT action ==]

    RULE escape
    [action escape]
        ==>
    [LVARS new_x new_y]
    ;;; Produce new coordinates
    [POP11 escape_from_predator(sim_myself) -> (new_x,new_y)]
    [do move_agent ?new_x ?new_y]
    ;;; Update position data
    [NOT action ==]

    RULE attack
    [action attack]
        ==>
    [LVARS new_x new_y]
    ;;; Produce new coordinates
    [POP11
        ;;; Check what predator attack style is being used
        if attack_type = 'direct' then
            head_to_prey(sim_myself) -> (new_x,new_y);
        elseif attack_type = 'isolate' then
            head_to_isolated_prey(sim_myself) -> (new_x,new_y);
        endif]
    ;;; Move agent
    [do move_agent ?new_x ?new_y]
    ;;; Update position data
    [NOT action ==]

    RULE attack_fast
    [action attack_fast]
        ==>
    [LVARS new_x new_y]
    ;;; Produce new coordinates
    [POP11 head_to_prey_fast(sim_myself) -> (new_x,new_y)]
    ;;; Move agent
    [do move_agent ?new_x ?new_y]
    ;;; Update position data
    [NOT action ==]


    RULE rest
    [action rest]
    ==>
    ;;; If resting perform no action
    [NOT action ==]

enddefine;



/*
RULESET  : assign_leader_ruleset
CREATED  : Mar 1999
PURPOSE  : Picks a flock leader from all of the prey
*/

define :ruleset assign_leader_ruleset;

    RULE add_leader
    [WHERE flock_type = 'leader']
    ==>
    [POP11 assign_leader(sim_myself)]

enddefine;
