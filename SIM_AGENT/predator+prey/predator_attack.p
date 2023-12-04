/*
PROCEDURE: jump_too_large (x1, y1, x2, y2) -> bool
INPUTS   : x1, y1, x2, y2
  Where  :
    x1 is the first x coordinate
    y1 is the first y coordinate
    x2 is the second x coordinate
    y2 is the second y coordinate
OUTPUTS  : bool is a boolean value
USED IN  : check_target
CREATED  : Feb 1999
PURPOSE  : Returns true if the difference between 2 points is greater than a
value (8). Used to check when a prey agent has moved off screen and back on in
another place.
TESTS:
jump_too_large(0,0,2,1)=>
jump_too_large(0,-98,20,10)=>
*/

define jump_too_large(x1,y1,x2,y2) -> bool;

    lvars diff_x,diff_y;

    ;;; Get the differences between the 2 sets of coordinates
    difference(x1,x2) -> diff_x;
    difference(y1,y2) -> diff_y;

    ;;; Compare the differences to a value. If a difference is greater than the
    ;;; value, return true
    if diff_x > 8 then
        true -> bool;
        return();
    elseif diff_y > 8 then
        true -> bool;
        return();
    else
        ;;; If not return false
        false -> bool;
        return();
    endif;

enddefine;

/*
PROCEDURE: check_target (ag, old_name, old_x, old_y)
INPUTS   : old_name, new_name
  Where  :
    ag is a basic agent
    old_name is the name of the target prey from last cycle
    old_x is the x coordinate of the target prey from last cycle
    old_y is the y coordinate of the target prey from last cycle
OUTPUTS  : NONE
USED IN  : predator_attack_ruleset
CREATED  : Feb 1999
PURPOSE  : Checks whether a prey agent has moved offscreen and onto another
part of the world. If it has, it places a "ghost" target just offsceen for the
predator to head for, thus chasing the prey offscreen and onto the another
part
TESTS:
setup();
add_prey(1,2,200,550,450,10,400,4,400,4,400,4);
add_predators(1,2,200,550,450,10,400,4);
run_agents(100);
*/

define :method checking_target(ag:basic_agent,old_name,old_x,old_y);

    lvars new_name,old,name,dist;

    lvars x_max = world_width/2,
        y_max = world_height/2;

    ;;; Get the current target name
    hd(visible_prey(ag)) --> ! [?new_name ==];

    ;;; If the new target and the old target are the same, then return
    if old_name == new_name and length(visible_prey(ag)) > 1 then
        return();

    ;;; Else if the old_target is still visible but not the current target
    elseif is_member([^old_name ==],visible_prey(ag)) then

        ;;; Get the old_target's current position
        is_member2([^old_name ==],visible_prey(ag)) --> ! [?name ?dist];
        valof(name) -> old;

        ;;; Check whether the difference in old and new positions is too large
        if jump_too_large(old_x,old_y,rc_picx(old),rc_picy(old)) then
            ;;;'follow it' =>

            ;;; If it is place a ghost at the position relevant to the side of
            ;;; the world the prey loved off

            lvars ghost_details,ghost_dist;

            ;;; For each of the 4 possible sides of the world
            if old_x < -x_max + agent_size then
                ;;;'left'=>

                ;;; Calculate how far it is to the side of the world
                difference(rc_picx(ag),(-x_max-agent_size)) -> ghost_dist;

                ;;; Create a list holding information about the ghost
                [ghost %-x_max - agent_size% ^old_y  ^ghost_dist]
                    ->ghost_details;

                ;;; Add the list to the list of visible prey
                ghost_details::visible_prey(ag)-> visible_prey(ag);

                ;;; Sort the list into ascending distance order
                syssort(visible_prey(ag),nearer_than)-> visible_prey(ag);

                ;;; Also place the ghost information into a slot in the
                ;;; predator
                ghost_details -> ghost_prey(ag);

                ;;; Calculate the amount of time for the ghost to exist, based
                ;;; on the distance to the ghost and the speed of the
                ;;; predator
                round(ghost_dist/predator_attack_speed(ag)) -> ghost_count(ag);
                return();

                ;;; Similar for each side

            elseif old_x > x_max - agent_size then
                ;;;'right' =>
                difference(rc_picx(ag),(x_max + agent_size)) -> ghost_dist;
                [ghost %x_max + agent_size% ^old_y ^ghost_dist]
                    ->ghost_details;
                ghost_details::visible_prey(ag)-> visible_prey(ag);
                syssort(visible_prey(ag),nearer_than)-> visible_prey(ag);
                ghost_details -> ghost_prey(ag);
                round(ghost_dist/predator_attack_speed(ag)) -> ghost_count(ag);
                return();
            elseif old_y > y_max - agent_size then
                ;;;'top' =>
                difference(rc_picy(ag),(y_max+agent_size)) -> ghost_dist;
                [ghost ^old_x %y_max + agent_size% ^ghost_dist]
                    ->ghost_details;
                ghost_details::visible_prey(ag)-> visible_prey(ag);
                syssort(visible_prey(ag),nearer_than)-> visible_prey(ag);
                ghost_details -> ghost_prey(ag);
                round(ghost_dist/predator_attack_speed(ag)) -> ghost_count(ag);
                return();
            elseif old_y < -y_max + agent_size then
                ;;;'bottom' =>
                difference(rc_picy(ag),(-y_max-agent_size)) -> ghost_dist;
                [ghost ^old_x %-y_max - agent_size% ^ghost_dist]
                    ->ghost_details;
                ghost_details::visible_prey(ag)-> visible_prey(ag);
                syssort(visible_prey(ag),nearer_than)-> visible_prey(ag);
                ghost_details -> ghost_prey(ag);
                round(ghost_dist/predator_attack_speed(ag)) -> ghost_count(ag);
                return();

            endif;

        else
            ;;; The new target prey is just closer than the old target
            ;;;'dont worry' =>
        endif;

    endif;

enddefine;
