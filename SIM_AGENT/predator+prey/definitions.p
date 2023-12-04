/*
RULESYSTEM: predator_rulesystem
CREATED  : Nov 1998
PURPOSE  : Rulesystem for the predator class.
*/

define :rulesystem predator_rulesystem;

    cycle_limit = 1;

    include: start_turn_ruleset
    include: predator_decision_1_ruleset
    include: predator_decision_2_ruleset
    include: predator_decision_3_ruleset
    include: predator_decision_4_ruleset
    include: update_ghost_ruleset
    include: predator_attack_ruleset
    include: perform_action_ruleset
    include: reduce_vars_ruleset
    include: clear_data_ruleset

enddefine;



/*
RULESYSTEM: prey_rulesystem
CREATED  : Nov 1998
PURPOSE  : Rulesystem for the prey class.
*/

define :rulesystem prey_rulesystem;

    cycle_limit = 1;

    include: start_turn_ruleset
    include: get_food_ruleset
    include: am_i_caught_ruleset
    include: have_i_eaten_ruleset
    include: prey_decision_1_ruleset
    include: prey_decision_2_ruleset
    include: prey_decision_3_ruleset
    include: prey_decision_4_ruleset
    include: perform_action_ruleset
    include: reduce_vars_ruleset
    include: clear_data_ruleset

enddefine;


/*
RULESYSTEM: world_rulesystem
CREATED  : Feb 1999
PURPOSE  : Rulesystem for the world class.
*/

define :rulesystem world_rulesystem;

    cycle_limit = 1;

    include: start_turn_ruleset
    include: get_food_ruleset
    include: assign_leader_ruleset
    include: clear_data_ruleset

enddefine;


/*
CLASS    : basic_agent
CREATED  : 5 Nov 1998
PURPOSE  : A basic agent class on which the predator and prey agents are
built.
*/

define :class basic_agent;
    is rc_selectable rc_linepic_movable sim_agent;

    ;;; Location
    slot rc_picx == 0;
    slot rc_pixy == 0;

    ;;; List containing graphics information
    slot name_list;
    slot pic_list;

    ;;; Counter to keep track of recursion in in_object
    slot recurse_count = 0;

    ;;; Variables used for random movement
    slot random_target_x;
    slot random_target_y;
    slot random_target_count = 0;
    slot random_target_speed;

    ;;; Agent size variable
    slot basic_agent_width = agent_size;

    ;;; Slots to hold the positions of all obstacles,prey and predators within
    ;;; sense range
    slot visible_obstacles = [];
    slot visible_prey = [];
    slot visible_predators = [];

    ;;; Sensing utilities
    slot sim_sensors = [{sim_sense_agent ^sense_limit}];

    ;;; Agent's energy level
    slot energy = start_energy;

    ;;; Slots to represent different energy thresholds
    slot hunger_limit = hunger;
    slot starving_limit = starving;
    slot full_up_limit = full_up;

enddefine;


/*
CLASS    : predator_agent
CREATED  : 11 Nov 1998
PURPOSE  : Predator agent which is a subclass of basic_agent.

*/

define :class predator_agent; is basic_agent;

    ;;; How many prey the predator has caught
    slot catch_count = 0;

    ;;; Slot for the agents target prey
    slot target_prey;

    ;;; Rulesystem
    slot sim_rulesystem = predator_rulesystem;

    ;;; Slot for dealing with prey moving off the edge of the world
    slot ghost_prey = [];
    slot ghost_count = 0;

    ;;; Slots to hold movement characteristics
    slot predator_attack_range = attack_range;
    slot predator_attack_speed = attack_speed;

enddefine;


/*
CLASS    : prey_agent
CREATED  : 11 Nov 1998
PURPOSE  : Prey agent which is a subclass of basic_agent.
*/

define :class prey_agent; is basic_agent;

    ;;; List to hold all visible food locations
    slot visible_food = [];

    ;;; Check slot if to see if eaten
    slot eaten = false;

    ;;; Rulesystem
    slot sim_rulesystem = prey_rulesystem;

    ;;; Slots to hold movement characteristics
    slot food_sense_range = food_range;
    slot prey_forage_speed = forage_speed;
    slot prey_escape_range = escape_range;
    slot prey_escape_speed = escape_speed;
    slot prey_flocking_speed = flocking_speed;
    slot prey_flocking_range = flocking_range;

    ;;; Slots to contain information about flock leadership
    slot leader = false;
    slot leader_x;
    slot leader_y;

    ;;; Slot to hold current action (used to calculate average number of
    ;;; encounters with predators
    slot action = "?";

enddefine;



/*
CLASS    : obstacle
CREATED  : Oct 1998
PURPOSE  : Class definition of the obstacles class.
*/

define :class obstacle;
    is rc_selectable rc_linepic_movable sim_object;

    ;;; Location
    slot rc_picx == 0;
    slot rc_pixy == 0;

    ;;; Obstacle size slot
    slot obstacle_width = obstacle_size;


enddefine;



/*
CLASS    : food
CREATED  : Feb 1999
PURPOSE  : Class definition of the food class.
*/

define :class food;
    is rc_selectable rc_linepic_movable sim_object;

    ;;; Location
    slot rc_picx == 0;
    slot rc_pixy == 0;

    ;;; Food size slot
    slot food_size = obstacle_size;

    ;;; Check slot if to see if eaten
    slot eaten = false;

enddefine;


/*
CLASS    : world
CREATED  : Feb 1999
PURPOSE  : Class definition of the world class. Used to keep track of certain
global events.
*/

define :class world; is sim_object;

    ;;; Slot to hold the visible_food list from last cycle
    slot last_world_food = [];

    ;;; Slots to hold all current objects
    slot visible_food = [];
    slot visible_obstacles = [];
    slot visible_prey = [];
    slot visible_predators = [];

    ;;; Sensing utilities
    slot sim_sensors = [{sim_sense_agent ^sense_limit}];

    ;;; Rulesystem
    slot sim_rulesystem = world_rulesystem;

    ;;; Number of cycles for a prey agent to lead the flock (gets altered)
    slot leader_count = 1;

enddefine;
