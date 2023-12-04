/*
Creates a prey flocking behaviour based on Craig Reynolds Boids model. This
moves the agent towards the local average of prey positions, whilst keeping
a certain separation.
*/

/*
PROCEDURE: average_coords (list) -> (avg_x, avg_y)
INPUTS   : list is of lists in the form [name x_coord y_coord distance]
OUTPUTS  : avg_x, avg_y
  Where  :
    avg_x is a the average of the x coordinates
    avg_y is a the average of the y coordinates
USED IN  : flock_boids
CREATED  : Mar 1999
PURPOSE  : Calculates the average of a list of coordinates

TESTS:
*/

define average_coords(list) -> (avg_x,avg_y);

    lvars py_name,py,dist,count = 0,x_total = 0,y_total = 0,details;
    ;;; Iterate through the list
    for details in list do
        ;;; Get the coordinates
        details --> ! [?py_name ?dist];
        valof(py_name) -> py;
        ;;; If they are within the allowed flocking distance add them to the
        ;;; sum for the relevant coordinate and iterate the counter
        if dist <= flocking_radius then
            rc_picx(py) + x_total -> x_total;
            rc_picy(py) + y_total -> y_total;
            1 + count -> count;
        endif;
    endfor;

    ;;; Calculate the average
    x_total/count -> avg_x;
    y_total/count -> avg_y;


enddefine;


/*
METHOD   : flock_boids (prey) -> (new_x, new_y)
INPUTS   : prey is a ???
OUTPUTS  : new_x, new_y
  Where  :
    new_x is the new x coord for the prey
    new_y is the new y coord for the prey
USED IN  : perform_action_ruleset
CREATED  : Mar 1999
PURPOSE  : Calculates new coordinates for the prey agent using a simplified
Boids model
TESTS:
trial_setup();
'boids' -> flock_type;
;;;add_food2(10,2);
;;;add_obstacles(15,2);
add_predators(2,2,200,350,150,50,400,4);
add_prey(20,2,1000,350,150,50,100,3,22,2,600,3);
run_agents(600);

*/

define :method flock_boids(prey:prey_agent) -> (new_x,new_y);

    lvars py_name,dist,a_x,a_y;
    ;;; Get the details of the closest prey agent
    hd(visible_prey(prey)) --> ! [?py_name ?dist];

    ;;; If the closest prey is outside the local flocking radius then move
    ;;; towards the biggest group of prey
    if dist > flocking_radius then
        flock_to_a_group(prey) -> (new_x,new_y);
    ;;; Else move towards the local average of prey positions
    else
        average_coords(visible_prey(prey)) -> (a_x,a_y);
        move_towards_point(rc_picx(prey),rc_picy(prey),
            prey_flocking_speed(prey),a_x,a_y) -> (new_x,new_y);
    endif;

    ;;; Adjust movement to keep the minimum separation
    in_personal_space(prey,new_x,new_y) -> (new_x,new_y);

    ;;; Check whether the new coordinates are within an obstacle, another
    ;;; prey agent or a predator, and if so adjust accordingly
    if in_object(prey,new_x,new_y) then
        0 -> random_target_count(prey);
        random_search(prey) -> (new_x,new_y);
    endif;

enddefine;
