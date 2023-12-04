;;; Library files
;;;uses oldkit
;;;uses oldrclib
uses sim_agent;
uses rclib;
uses rc_window_object;
uses rc_linepic;
uses rc_mousepic;
uses rc_control_panel;

global vars

        ;;; Screen dump variables
        graphic_window = 'World',
        image_dump_frequency = 1,
        imagefile = "mysim",

        ;;; Global attack information
        attack_type = 'isolate',
        attack_radius = 60,

        ;;; Global flocking information
        flocking_radius = 40,
        flock_type = 'herd',

        ;;; Counters to supply simulation information
        predator_starve = 0,
        prey_starve = 0,
        prey_encounter_count = 0.0,
        prey_caught_count = 0,

        ;;; Variables used for interface tasks
        world_create = false,
        panel_list = [],
        world_flag = 0,
        pred_flag = 0,
        prey_flag = 0,
        vars_flag = 0,
        run_flag = 0,

        ;;; Global lists to hold the objects
        world_list =  [],
        predator_list = [],
        prey_list = [],
        obstacle_list = [],
        food_list = [],

        ;;; How much extra energy is used when implementing the fast actions
        extra = 1,

        ;;; Default number of cycles that  ghost prey stay in the simulation
        ghost_memory = 20,

        ;;; Flock Separation Variable
        flock_separation = 20,

        ;;; Counters to represent the number of different agents on screen.
        prey_count = 0,
        predator_count = 0,
        obstacle_count = 0,
        food_count = 0,

        ;;; Variable for the main graphics window
        mainwin,

        ;;; Variables for the width and heigth of the window
        world_width = 400,
        world_height = 400,

        ;;; Sense limit for all agents
        sense_limit = 10000000,

        ;;; How much energy the food sources are worth
        prey_energy_value = 100,
        food_energy_value = 100,

        ;;; Default values for agent slots
        hunger = 100,
        starving = 50,
        full_up = 300,
        food_range = 300,
        forage_speed = 3,
        start_energy = 100,
        attack_range = 300,
        attack_speed = 3,
        escape_range = 50,
        escape_speed = 3,
        flocking_speed = 3,
        flocking_range = 400,
        prey_colour = 'blue',
        agent_size = 8,
        obstacle_size = 10;

max(popmemlim, 8000000) -> popmemlim;

;;; Load other files
load definitions.p
load sensing.p
load predator_9_3_99.p
load interface_utilities.p
load predator_attack.p
load group.p
load herding.p
load follow_leader.p
load boids.p
load new_preds.p
load rule_systems.p
load pred_actions.p
load prey_actions.p
load tracing1.p
load gui_world.p
load gui_pred.p
load gui_prey.p
load gui_sched.p
load gui_vars.p
load gui_startup.p

;;; Stop extra characters being printed
;;;'' -> pop_=>_flag;

;;; Prevent output going to a ved window
false -> async_vedbuffer;

;;; Some startup information
''=>
''=>
'To run the simulator just compile the following line,'=>
''=>
        'go();'=>
