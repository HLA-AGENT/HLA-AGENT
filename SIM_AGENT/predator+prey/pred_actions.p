/*
RULESET  : predator_decision_1_ruleset
CREATED  : Feb 1999
PURPOSE  : Chooses what action to perform
*/

define :ruleset predator_decision_1_ruleset;

    RULE hungry
    [WHERE energy(sim_myself) < hunger_limit(sim_myself)]
    ==>
    ;;; Update graphics
    [POP11 update_pic_pred(sim_myself,'yellow')]
    ;;; Update state of mind
    [state_of_mind hungry]

    RULE full_up
    [WHERE energy(sim_myself) > full_up_limit(sim_myself)]
    ==>
    [POP11 update_pic_pred(sim_myself,'green')]
    [state_of_mind full_up]

    RULE normal_prey_visible
    [WHERE prey_in_range(sim_myself)]
    ==>
    [POP11 remove_pic(sim_myself)]
    ;;; Update graphics
    [POP11 update_string(sim_myself,'A')]
    [action attack]

    RULE normal_no_prey_visible
    ==>
    [POP11 remove_pic(sim_myself)]
    [POP11 update_string(sim_myself,'S')]
    [action search]

enddefine;

/*
RULESET  : predator_decision_2_ruleset
CREATED  : Feb 1999
PURPOSE  : Chooses what action to perform
*/

define :ruleset predator_decision_2_ruleset;

    RULE full_up_prey_visible
    [state_of_mind full_up]
    [WHERE prey_in_range(sim_myself)]
    ==>
    [action rest]
    [POP11 update_string(sim_myself,'R')]
    [NOT state_of_mind ==]

    RULE full_up
    [state_of_mind full_up]
    ==>
    [action rest]
    [POP11 update_string(sim_myself,'R')]
    [NOT state_of_mind ==]

enddefine;


/*
RULESET  : predator_decision_3_ruleset
CREATED  : Feb 1999
PURPOSE  : Chooses what action to perform
*/

define :ruleset predator_decision_3_ruleset;

    RULE starving
    [state_of_mind hungry]
    [WHERE energy(sim_myself) < starving_limit(sim_myself)]
    ==>
    [POP11 update_pic_pred(sim_myself,'red')]
    [NOT state_of_mind ==]
    [state_of_mind starving]

    RULE hungry_prey_visible
    [state_of_mind hungry]
    [WHERE prey_in_range(sim_myself)]
    ==>
    [action attack]
    [POP11 update_string(sim_myself,'A')]
    [NOT state_of_mind ==]

    RULE hungry_no_prey_visible
    [state_of_mind hungry]
    ==>
    [action search]
    [POP11 update_string(sim_myself,'S')]
    [NOT state_of_mind ==]

enddefine;


/*
RULESET  : predator_decision_4_ruleset
CREATED  : Feb 1999
PURPOSE  : Chooses what action to perform
*/

define :ruleset predator_decision_4_ruleset;

    RULE starving_prey_visible
    [state_of_mind starving]
    [WHERE prey_in_range(sim_myself)]
    ==>
    [action attack_fast]
    [POP11 update_string(sim_myself,'A')]
    [NOT state_of_mind ==]

    RULE starving_no_prey_visible
    [state_of_mind starving]
     ==>
    [action search_fast]
    [POP11 update_string(sim_myself,'S')]
    [NOT state_of_mind ==]

enddefine;


/*
RULESET  : update_ghost_ruleset
CREATED  : Feb 1999
PURPOSE  : Checks information regarding "ghost" prey
*/

define :ruleset update_ghost_ruleset;

    [DLOCAL [prb_allrules = true]];

    RULE check_for_ghost
    [WHERE ghost_prey(sim_myself) /= []]
    ==>
    [ghost prey]

    ;;; A predator can only have 1 ghost at a time
    RULE only_one_ghost
    [ghost prey]
    [WHERE is_member(ghost_prey(sim_myself),visible_prey(sim_myself))]
    ==>
    [NOT ghost prey]

    ;;; Add the ghost to visible_prey
    RULE add_ghost
    [ghost prey]
    ==>
    [POP11 ghost_prey(sim_myself)::visible_prey(sim_myself)
                        ->visible_prey(sim_myself)]
    [POP11 syssort(visible_prey(sim_myself),nearer_than)
                        ->visible_prey(sim_myself)]

    RULE remove_ghost_count
    [WHERE ghost_count(sim_myself) /= 0 ]
    ==>
    [POP11 ghost_count(sim_myself) - 1 -> ghost_count(sim_myself)]

    ;;; Ghost count reaches 0 so remove ghost
    RULE remove_ghost
    [WHERE ghost_count(sim_myself) = 0 ]
    ==>
    [POP11 [] -> ghost_prey(sim_myself)]

enddefine;



/*
RULESET  : predator_attack_ruleset
CREATED  : Feb 1999
PURPOSE  : Checks the predators target
*/

define :ruleset predator_attack_ruleset;

    ;;;[DLOCAL [prb_allrules = true]];

    RULE no_previous_target
    [OR [action attack] [action attack_fast]]
    [NOT target ==]
    [LVARS new_target_name new_tar]
    ;;; Get target
    [POP11 hd(visible_prey(sim_myself)) --> ! [?new_target_name =]]
    [POP11 valof(new_target_name) -> new_tar]
    [LVARS [new_tar_x = rc_picx(new_tar)]
           [new_tar_y = rc_picy(new_tar)]]
        ==>
    ;;; Add a target to the database
    [target ?new_target_name ?new_tar_x ?new_tar_y]

    ;;; What to do if the target is a ghost
    RULE check_where_target_is_for_ghost
    [OR [action attack] [action attack_fast]]
    [target ?old_target_name ?old_tar_x ?old_tar_y]
    [LVARS new_target_name new_tar_x new_tar_y]
    [WHERE hd(visible_prey(sim_myself)) matches [ghost ==]]
    [POP11 hd(visible_prey(sim_myself)) --> !
                        [ghost ?new_tar_x ?new_tar_y =]]
    ;;; Check target against last target
    [POP11 checking_target(sim_myself,old_target_name,old_tar_x,old_tar_y)]
        ==>
    [NOT target ==]
    [target ?new_target_name ?new_tar_x ?new_tar_y]

    ;;; If target isn't a ghost
    RULE check_where_target_is
    [OR [action attack] [action attack_fast]]
    [target ?old_target_name ?old_tar_x ?old_tar_y]
    [LVARS new_target_name new_tar]
    [POP11 hd(visible_prey(sim_myself)) --> ! [?new_target_name =]]
    [POP11 valof(new_target_name) -> new_tar]
    [LVARS [new_tar_x = rc_picx(new_tar)]
           [new_tar_y = rc_picy(new_tar)]]
    ;;; Check target against last target
    [POP11 checking_target(sim_myself,old_target_name,old_tar_x,old_tar_y)]
        ==>
    [NOT target ==]
    [target ?new_target_name ?new_tar_x ?new_tar_y]



enddefine;
