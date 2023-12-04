/*
File to create new behaviour for the predator. If attack_type is 'isolate'
then the predator will head for the nearest prey surrounded by the least
amount of other prey.
*/

/*
PROCEDURE: min_in_list (list) -> element
INPUTS   : list is a list with a number as its last value
OUTPUTS  : element is an element from the list
USED IN  : head_to_isolated_prey
CREATED  : Mar 1999
PURPOSE  : Returns the element in the list with the lowest final value

TESTS:
min_in_list([[a 1][b 2][c 3]])=>
min_in_list([[a a 1][b b 2][c c 3]])=>
*/

define min_in_list(list) -> element;

    lvars match,value,min_value = 1000000000;

    ;;; Iterate through the list
    for match in list do

        if islist(match) then

            ;;; Get the last value in the list
            match --> ! [== ?value];
            ;;; Compare it to the current list minimum
            if value < min_value then
            ;;; If lower then make it current minimum
                value -> min_value;
            endif;

        endif;

    endfor;

    ;;; Get the member of the list which contains the minimum value
    is_member2([== ^min_value],list) -> element;


enddefine;


/*
PROCEDURE: sum_list (list) -> sum
INPUTS   : list is a list with a number as its last value
OUTPUTS  : sum is the sum of the last values
USED IN  : head_to_isolated_prey
CREATED  : Mar 1999
PURPOSE  : Calculates the sum of final values of the lists, and returns the
head of the list (which willl represent the name of a prey within the program)
and the sum.
TESTS:
sum_list([[a 1][b 2][c 3]])=>
sum_list([[a a 1][b b 2][c c 3]])=>
*/

define sum_list(list) -> sum;

    lvars match,value,add = 0;

    ;;; Iterate through the list
    for match in list do

        if islist(match) then

            ;;; Get last value and add it to sum
            match --> ! [== ?value];
            value + add -> add;

        endif;

    endfor;

    ;;; Create list to return
    [%hd(list)% ^add] ->  sum;

enddefine;


/*
PROCEDURE: target_coords (details, prey_list) -> (new_x, new_y)
INPUTS   : details, prey_list
  Where  :
    details is a list of one preys details
    prey_list is of all prey
OUTPUTS  : new_x, new_y
  Where  :
    new_x is the x coord of a prey agent
    new_y is the y coord of a prey agent
USED IN  : head_to_isolated_prey
CREATED  : Mar 1999
PURPOSE  : Takes the name of one prey agent (the nearest isolated prey) and
then finds it within the list of prey given and returns the x and y coords
of that prey.
TESTS:
target_coords([py4 6],[[py4 -168 -22 72.4431] [py5 -189 117 73.0068]])=>
target_coords([py8 6],[[py4 -168 -22 72.4431] [py5 -189 117 73.0068]
    [py2 -79 66 82.5651] [py8 -49 110 126.178] [py1 43 -46 224.555]
    [py6 78 -35 252.723] [py7 160 4 323.289] [py3 176 138 347.333]])=>

*/

define target_coords(details,prey_list) -> (new_x,new_y);

    lvars name,count,prey;
    ;;; Get info from details
    explode(details) -> (name,count);

    /*
    ;;; Get relevant prey from list and then return its coordinates
    is_member2([^name ==],prey_list)->prey_details;
    prey_details --> ! [= ?new_x ?new_y =];
    */

    valof(name) -> prey;
    rc_picx(prey) -> new_x;
    rc_picy(prey) -> new_y;

enddefine;



/*
METHOD   : head_to_isolated_prey (pred) -> (new_x, new_y)
INPUTS   : pred is a predator agent
OUTPUTS  : new_x, new_y
  Where  :
    new_x is the new x coord for the predator
    new_y is the new y coord for the predator
USED IN  : perform_action_ruleset
CREATED  : Mar 1999
PURPOSE  : Calculates new coordinates for the predator agent, these either
take it towards the most isolated prey agent, or towards the closest prey
agent if it is within a certain range (attack_radius)

TESTS:
trial_setup();
'isolate' -> attack_type;
'group' -> flock_type;
add_prey(8,2,200,350,150,50,400,4,222,2,333,3);
add_predators(1,1,200,350,150,50,400,4);
run_agents(200);
is_member([ghost ==], [[1 2 3][ghost wd d]])=>
*/

define :method head_to_isolated_prey(pred:predator_agent) -> (new_x,new_y);

    lvars prey_density_list,prey_box_count,quarter_count,
        prey_min,global_count_list=[],global_min,tar_x,tar_y,dist,p_dist,
        prey_name,prey;

    ;;; If the chasing an agent off-screen use direct approach
    if is_member([ghost ==],visible_prey(pred)) then
        head_to_prey(pred) -> (new_x,new_y);
        return();
    endif;

    ;;; Count the number of prey surrounding each individual prey agent
    box_prey(visible_prey(pred)) -> prey_density_list;
    ;;;prey_density_list=>

    ;;; Iterate through list of all of the counts and create a list of the
    ;;; all the sums and the agents they belong to
    for prey_box_count in prey_density_list do

        sum_list(prey_box_count) -> prey_min;
        ;;;hd(prey_box_count)::prey_min->prey_min;
        prey_min::global_count_list->global_count_list;

    endfor;
    ;;;global_count_list=>

    ;;; Reverse the list to place the closest at the head, and then return the
    ;;; minimum from the list
    rev(global_count_list)->global_count_list;
    min_in_list(global_count_list)->global_min;
    ;;;global_min=>

    ;;; Get the coords of the global mininmum (the most isolated prey) and
    ;;; calculate the distance to it
    target_coords(global_min,visible_prey(pred)) -> (tar_x,tar_y);
    distance(rc_picx(pred),rc_picy(pred),tar_x,tar_y) -> dist;

    ;;; Get details of the closest prey agent
    hd(visible_prey(pred)) --> ! [?prey_name ?p_dist];
    valof(prey_name) -> prey;

    ;;; If the closest prey is within the close attack range then move
    ;;; towards it
    if p_dist < attack_radius then
        move_towards_point(rc_picx(pred),rc_picy(pred),
            predator_attack_speed(pred),rc_picx(prey),rc_picy(prey))
            -> (new_x,new_y);

    ;;; If the most isolated prey is out of range move to a nearer one
    elseif dist > predator_attack_range(pred) then
        head_to_prey(pred) -> (new_x,new_y);

    ;;; Else, move towards the most isolated prey
    else
        move_towards_point(rc_picx(pred),rc_picy(pred),
            predator_attack_speed(pred),tar_x,tar_y) -> (new_x,new_y);
    endif;

    ;;; Check whether the new coordinates are within an obstacle or other
    ;;; predator, and if so adjust accordingly
    if in_object(pred,new_x,new_y) then
        0 -> random_target_count(pred);
        random_search(pred) -> (new_x,new_y);
    endif;

enddefine;
