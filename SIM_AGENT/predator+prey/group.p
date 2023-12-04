/*
PROCEDURE: count_box (x, y, prey_list) -> count
INPUTS   : x, y, prey_list
  Where  :
    x is the x coordinate of the bottom left hand corner of a box
    y is the y coordinate of the bottom left hand corner of a box
    prey_list is a list of all prey
OUTPUTS  : count is the number of prey agents in the box
USED IN  : box_prey_slave
CREATED  : Feb 1999
PURPOSE  : Given the coordinates of the bottom right hand corner of a square of
size flocking_radius, count how many prey agents are within this square
TESTS:
setup();
add_prey(6,2,300,400,400,20,200,4,200,4,200,4);
run_agents(1);
count_box(0,0,visible_prey(py1))=>

*/

define count_box(x,y,prey_list) -> count;

    lvars prey_details;
    0 -> count;

    ;;; Iterate through list of prey
    for prey_details in prey_list do

        lvars prey_name,prey;

        ;;; Get individual prey
        prey_details --> ! [?prey_name ==];
        valof(prey_name) -> prey;

        ;;; If the prey's coordinates lie within the box, add 1 to the count
        if rc_picx(prey) <= x + flocking_radius and rc_picx(prey) >= x and
            rc_picy(prey) <= y + flocking_radius and rc_picy(prey) >= y then

            count + 1 -> count;

        endif;

    endfor;

enddefine;



/*
PROCEDURE: box_prey_slave (prey_list, quarter) -> count_list
INPUTS   : prey_list, quarter
  Where  :
    prey_list is a list of all prey
    quarter is a number representing a quarter of a square
--------------------
|        |         |
|   1    |    2    |
|        |         |
--------------------
|        |         |
|   3    |    4    |
|        |         |
--------------------

OUTPUTS  : count_list is a list containing lists with a quarter number and the
number of prey agents in that quarter
USED IN  : box_prey
CREATED  : Feb 1999
PURPOSE  : Given the coordinates of a prey agent, creates a list containing
4 lists, each lists has as its head the number of a quarter, and then the
number of prey agents in that quarter.
TESTS:
setup();
add_prey(20,2,300,400,400,20,200,4,200,4,200,4);
run_agents(1);
box_prey_slave(rc_picx(py15),rc_picy(py15),visible_prey(py15),1)=>
*/

define box_prey_slave(prey_x,prey_y,prey_list,quarter) -> count_list;

    lvars quarter_count;

    ;;; For each quarter
    if quarter = 1 then

        ;;; Count the number of prey agents in the quarter using coordinates
        ;;; calculated from the preys coordinates
        count_box((prey_x - flocking_radius),prey_y,prey_list)
            -> quarter_count;

        ;;; Call box_prey_slave recursively to calculate the next quarter
        box_prey_slave(prey_x,prey_y,prey_list,quarter + 1) -> count_list;

        ;;; Create the list for the quarter and add it to the return list
        [1 ^quarter_count]::count_list -> count_list;

    elseif quarter = 2 then
        count_box(prey_x,prey_y,prey_list) -> quarter_count;
        box_prey_slave(prey_x,prey_y,prey_list,quarter + 1) -> count_list;
        [2 ^quarter_count]::count_list -> count_list;

    elseif quarter = 3 then
        count_box((prey_x - flocking_radius),(prey_y - flocking_radius),
            prey_list) -> quarter_count;
        box_prey_slave(prey_x,prey_y,prey_list,quarter + 1) -> count_list;
        [3 ^quarter_count]::count_list -> count_list;

    elseif quarter = 4 then
        count_box(prey_x,(prey_y - flocking_radius),prey_list)
            -> quarter_count;
        box_prey_slave(prey_x,prey_y,prey_list,quarter + 1) -> count_list;
        [4 ^quarter_count]::count_list -> count_list;

    ;;; Stopping condition, pass back the empty list
    elseif quarter = 5 then
        [] -> count_list;
        return();

    endif;

enddefine;


/*
PROCEDURE: box_prey (prey_list) -> count_list
INPUTS   : prey_list is a list of all prey
OUTPUTS  : count_list is a list containing lists with a quarter number and the
number of prey agents in that quarter for every prey agent in prey_list
USED IN  : flock_to_a_group
CREATED  : Feb 1999
PURPOSE  : Creates a list of count_lists (from box_prey_slave) for each prey
agent within the prey list and appends the name of the prey to the head of
the list.
TESTS:
setup();
add_prey(20,2,300,400,400,20,200,4,200,4,200,4);
run_agents(1);
box_prey(visible_prey(py15))=>
*/

define box_prey(prey_list) -> count_list;

    [] -> count_list;
    lvars prey_details,prey_name,prey,prey_count,temp_list;

    ;;; For each prey agent in prey_list
    for prey_details in prey_list do

        ;;; Get the prey
        prey_details --> ! [?prey_name ==];
        valof(prey_name) -> prey;

        ;;; Count the number of prey surrounding it
        box_prey_slave(rc_picx(prey),rc_picy(prey),prey_list,1) -> prey_count;

        ;;; Create with the count_list and the prey's name
        [%[^prey_name]<>prey_count%] -> prey_count;

        ;;; Add this list to the list to be returned
        count_list<>prey_count -> count_list;

    endfor;

enddefine;


/*
PROCEDURE: max_in_list (list) -> element
INPUTS   : list is a list of lists with a number as there last element
OUTPUTS  : element is an element of list
USED IN  : flock_to_a_group
CREATED  : Feb 1999
PURPOSE  : Returns an element from a list which has the highest number as its
last elemnt
TESTS:
max_in_list([py [q1 1] [q2 2] [q3 1] [q4 1]])=>
*/

define max_in_list(list) -> element;

    lvars match,value,max_value = 0;

    ;;; For every element in list
    for match in list do

        ;;; If it is a list
        if islist(match) then

            ;;; Get the last value and compare it to the current maximum
            match --> ! [== ?value];
            if value > max_value then
                ;;; If it is greater, make it the current maximum
                value -> max_value;
            endif;

        endif;

    endfor;

    ;;; Return the element of list containing the maximum value
    is_member2([== ^max_value],list) -> element;


enddefine;


/*
PROCEDURE: box_target_coords (details, prey_list) -> (new_x, new_y)
INPUTS   : args, prey_list
  Where  :
    details is a list of 3 arguments (prey_name,quarter,count)
    prey_list is a list of all prey
OUTPUTS  : new_x, new_y
  Where  :
    new_x is a the new x coordinate
    new_y is a the new y coordinate
USED IN  : flock_to_a_group
CREATED  : Feb 1999
PURPOSE  : Returns the coordinates of the centre of the quarter of a box
(from box_prey) for the prey who's name is given in args
TESTS:
setup();
add_prey(20,2,300,400,400,20,200,4,200,4,200,4);
run_agents(1);
box_target_coords([py6 2 2],visible_prey(py1))=>
py6=>
*/

define box_target_coords(details,prey_list) -> (new_x,new_y);

    lvars name,quarter,count,prey_details,py_name,py;

    ;;; Get the variables from details
    explode(details) -> (name,quarter,count);
    valof(name) -> py;

    ;;; The centre of the box will at be half the overall size
    lvars box_centre = (flocking_radius/2);

    ;;; For the relevant quarter add or subtract box_centre from the prey's
    ;;; coordinates to calculate the centre of the box
    if quarter = 1 then
        rc_picx(py) - box_centre -> new_x;
        rc_picy(py) + box_centre -> new_y;
    elseif quarter = 2 then
        rc_picx(py) + box_centre -> new_x;
        rc_picy(py) + box_centre -> new_y;
    elseif quarter = 3 then
        rc_picx(py) - box_centre -> new_x;
        rc_picy(py) - box_centre -> new_y;
    elseif quarter = 4 then
        rc_picx(py) + box_centre -> new_x;
        rc_picy(py) - box_centre -> new_y;
    else
        ;;; Default case
        rc_picx(py) -> new_x;
        rc_picy(py) -> new_y;
    endif;


enddefine;

/*
METHOD   : flock_to_a_group (prey) -> (new_x, new_y)
INPUTS   : prey is a prey agent
OUTPUTS  : new_x, new_y
  Where  :
    new_x is the new x coordinate
    new_y is the new y coordinate
USED IN  : perform_actions_ruleset flock_boids
CREATED  : Feb 1999
PURPOSE  : Calculates the coordinates for a move to take the prey agent
towards the point in the world most densly covered by prey agents, and then
keep the agent a minimum distance from that point
TESTS:
setup();
'group' -> flock_type;
add_prey(8,2,200,350,150,50,400,4,222,2,333,3);
run_agents(50);
*/

define :method flock_to_a_group(prey:prey_agent) -> (new_x,new_y);

    lvars prey_density_list,prey_box_count,quarter_count,
        prey_max,global_max_list=[],global_max,tar_x,tar_y,dist;

    ;;; Create a list of counts for all the quarters for all the prey
    box_prey(visible_prey(prey)) -> prey_density_list;
    ;;;prey_density_list=>

    ;;; For each prey, take the quarter with the highest count and add it to
    ;;; the global_max_list
    for prey_box_count in prey_density_list do

        max_in_list(prey_box_count) -> prey_max;
        hd(prey_box_count)::prey_max->prey_max;
        prey_max::global_max_list->global_max_list;

    endfor;

    ;;; Reverse the global_max_list to get the closest max at the front of the
    ;;; list
    rev(global_max_list)->global_max_list;

    ;;; Get the maximum from the global_max_list (ie the highest global count)
    max_in_list(global_max_list)->global_max;
    ;;;global_max=>

    ;;; Get the coordinates of the centre of the quarter containing the highest
    ;;; global count, these are the target coordinates
    box_target_coords(global_max,visible_prey(prey)) -> (tar_x,tar_y);

    ;;; Calculate the distance to the target coordinates
    distance(rc_picx(prey),rc_picy(prey),tar_x,tar_y) -> dist;

    ;;; If the distance is greater than the allowed flocking range, move to the
    ;;; nearest prey (at least 1 prey agent will be range to cause flocking)
    if dist > prey_flocking_range(prey) then
        flock_to_prey(prey) -> (new_x,new_y);

    ;;; If the distance is within the allowed range, and greater than the
    ;;; flock separation, move towards the target
    elseif dist > flock_separation and dist <= prey_flocking_range(prey) then
        move_towards_point(rc_picx(prey),rc_picy(prey),
            prey_flocking_speed(prey),tar_x,tar_y) -> (new_x,new_y);

    ;;; If the distance is less than the flcok separation, move away from the
    ;;; target
    elseif dist < flock_separation then
        move_away_from_point(rc_picx(prey),rc_picy(prey),
            prey_flocking_speed(prey),tar_x,tar_y) -> (new_x,new_y);
    ;;; Default case
    else
        random_search(prey) -> (new_x,new_y);
    endif;

    ;;;If move would take prey into an object, make a random move
    ;;; instead.
    if in_object(prey,new_x,new_y) then
        0 -> random_target_count(prey);
        random_search(prey) -> (new_x,new_y);
    endif;


enddefine;
