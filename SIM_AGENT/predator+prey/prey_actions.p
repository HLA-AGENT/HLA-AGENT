
/*
RULESET  : prey_decision_1_ruleset
CREATED  : Feb 1999
PURPOSE  : Chooses what action to perform
*/

define :ruleset prey_decision_1_ruleset;

    RULE hungry
    [WHERE energy(sim_myself) < hunger_limit(sim_myself)]
    ==>
    ;;; Update graphics
    [POP11 update_pic(sim_myself,'yellow')]
    ;;; Update state of mind
    [state_of_mind hungry]

    RULE full_up
    [WHERE energy(sim_myself) > full_up_limit(sim_myself)]
    ==>
    [POP11 update_pic(sim_myself,'green')]
    [state_of_mind full_up]

    RULE normal_predator_visible
    [WHERE predator_in_range(sim_myself)]
    ==>
    [POP11 remove_pic(sim_myself)]
    ;;; Update graphics
    [POP11 update_string(sim_myself,'E')]
    [action escape]
    ;;; Add an encounter with predator to counter
    [POP11  if action(sim_myself) /= "escape" then
                "escape" -> action(sim_myself);
                1.0 + prey_encounter_count -> prey_encounter_count;
            endif;]

    RULE normal_prey_visible
    [WHERE prey_in_range(sim_myself)]
    ==>
    [POP11 remove_pic(sim_myself)]
    [POP11 update_string(sim_myself,'F')]
    [action flock]
    [POP11 "flock" -> action(sim_myself)]

    RULE normal_food_visible
    [WHERE food_in_range(sim_myself)]
    ==>
    [POP11 remove_pic(sim_myself)]
    [POP11 update_string(sim_myself,'G')]
    [action forage]
    [POP11 "forage" -> action(sim_myself)]

    RULE normal_no_food_visible
    ==>
    [POP11 remove_pic(sim_myself)]
    [POP11 update_string(sim_myself,'S')]
    [action search]
    [POP11 "search" -> action(sim_myself)]

enddefine;

/*
RULESET  : prey_decision_2_ruleset
CREATED  : Feb 1999
PURPOSE  : Chooses what action to perform
*/

define :ruleset prey_decision_2_ruleset;

    RULE full_up_predator_visible
    [state_of_mind full_up]
    [WHERE predator_in_range(sim_myself)]
    ==>
    [action escape]
    [POP11  if action(sim_myself) /= "escape" then
                "escape" -> action(sim_myself);
                1.0 + prey_encounter_count -> prey_encounter_count;
            endif;]
    [POP11 update_string(sim_myself,'E')]
    [NOT state_of_mind ==]

    RULE full_up_prey_visible
    [state_of_mind full_up]
    [WHERE prey_in_range(sim_myself)]
    ==>
    [action flock]
    [POP11 "flock" -> action(sim_myself)]
    [POP11 update_string(sim_myself,'F')]
    [NOT state_of_mind ==]

    RULE full_up_no_prey_visible
    [state_of_mind full_up]
    ==>
    [action rest]
    [POP11 "rest" -> action(sim_myself)]
    [POP11 update_string(sim_myself,'R')]
    [NOT state_of_mind ==]

enddefine;

/*
RULESET  : prey_decision_3_ruleset
CREATED  : Feb 1999
PURPOSE  : Chooses what action to perform
*/

define :ruleset prey_decision_3_ruleset;

    RULE starving
    [state_of_mind hungry]
    [WHERE energy(sim_myself) < starving_limit(sim_myself)]
    ==>
    [POP11 update_pic(sim_myself,'red')]
    [NOT state_of_mind ==]
    [state_of_mind starving]


    RULE hungry_predator_visible
    [state_of_mind hungry]
    [WHERE predator_in_range(sim_myself)]
    ==>
    [action escape]
    [POP11  if action(sim_myself) /= "escape" then
                "escape" -> action(sim_myself);
                1.0 + prey_encounter_count -> prey_encounter_count;
            endif;]
    [POP11 update_string(sim_myself,'E')]
    [NOT state_of_mind ==]

    RULE hungry_food_visible
    [state_of_mind hungry]
    [WHERE food_in_range(sim_myself)]
    ==>
    [action forage]
    [POP11 "forage" -> action(sim_myself)]
    [POP11 update_string(sim_myself,'G')]
    [NOT state_of_mind ==]

    RULE hungry_prey_visible
    [state_of_mind hungry]
    [WHERE prey_in_range(sim_myself)]
    ==>
    [action flock]
    [POP11 "flock" -> action(sim_myself)]
    [POP11 update_string(sim_myself,'F')]
    [NOT state_of_mind ==]

    RULE hungry_no_prey_visible
    [state_of_mind hungry]
    ==>
    [action search]
    [POP11 "search" -> action(sim_myself)]
    [POP11 update_string(sim_myself,'S')]
    [NOT state_of_mind ==]

enddefine;


/*
RULESET  : prey_decision_4_ruleset
CREATED  : Feb 1999
PURPOSE  : Chooses what action to perform
*/

define :ruleset prey_decision_4_ruleset;

    RULE starving_food_visible
    [state_of_mind starving]
    [WHERE food_in_range(sim_myself)]
    ==>
    [action forage_fast]
    [POP11 "forage_fast" -> action(sim_myself)]
    [POP11 update_string(sim_myself,'G')]
    [NOT state_of_mind ==]

    RULE starving_no_food_visible
    [state_of_mind starving]
    ==>
    [action search_fast]
    [POP11 "search_fast" -> action(sim_myself)]
    [POP11 update_string(sim_myself,'S')]
    [NOT state_of_mind ==]

enddefine;
