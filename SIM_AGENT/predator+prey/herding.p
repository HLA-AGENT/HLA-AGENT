/*
Creates a flocking behaviour based on Dave Brogan and ...., Herding algorithm
*/

;;; Variable representing distance desired position is away from prey
global vars D = 20;

/*
PROCEDURE: weighted_average_dsq (list, x, y) -> (avg_x, avg_y)
INPUTS   : list, x, y
  Where  :
    list is a list in the form [[x y distance].....]
    x is a x coordinate
    y is a y coordinate
OUTPUTS  : avg_x, avg_y
  Where  :
    avg_x is the average x coordinate
    avg_y is the average y coordinate
USED IN  : get_des_position
CREATED  : Mar 1999
PURPOSE  : Calculates the weighted average (weighted by 1/distance) of the
coordinates in the list.
TESTS:
weighted_average_dsq([[20 -20 20][-30 20 20][10 -60 40][-30 60 40]],20,150)=>
*/

define weighted_average_dsq(list,x,y)->(avg_x,avg_y);

    ;;;rc_draw_blob(x,y,2,'blue');
    lvars x_sep_counter=0,y_sep_counter=0,x_sum=0,y_sum=0,element,d_x,d_y,dist,xs=0,ys=0;

    ;;; For each element in the list
    for element in list do
        lvars x_sep,y_sep;

        ;;; Get variables
        element --> ! [?d_x ?d_y ?dist]; ;;;fine
        ;;;rc_draw_blob(d_x,d_y,2,'black');

        ;;; Calculate the separation between them and x and y
        x - d_x -> x_sep;
        y - d_y -> y_sep;

        ;;; Update counters for weighted sums with the if loops to prevent
        ;;; division by 0
        if x_sep /= 0 then
            x_sep_counter + (1.0/x_sep**2) -> x_sep_counter;
        endif;
        if y_sep /= 0 then
            y_sep_counter + (1.0/y_sep**2) -> y_sep_counter;
        endif;
        if x_sep /= 0 then
            x_sum + (d_x/x_sep**2) -> x_sum;
        endif;
        if y_sep /= 0 then
            y_sum + (d_y/y_sep**2) -> y_sum;
        endif;
    endfor;

    ;;; Calculate the averages

    ;;; If loops to prevent division by 0
    if x_sep_counter = 0 then
        d_x -> avg_x;
    else
        x_sum/x_sep_counter -> avg_x;
    endif;

    ;;; If loops to prevent division by 0
    if y_sep_counter = 0 then
        d_y -> avg_y;
    else
        y_sum/y_sep_counter -> avg_y;
    endif;

    ;;;rc_draw_blob(avg_x,avg_y,2,'green');

enddefine;


/*
METHOD   : des_position (py1, py2_x, py2_y) -> (x, y)
INPUTS   : py1, py2_x, py2_y
  Where  :
    py1 is a prey agent
    py2_x is the x coordinate of a prey agent
    py2_y is the y coordinate of a prey agent
OUTPUTS  : x, y
  Where  :
    x is the x coordinate of the desired position
    y is the y coordinate of the desired position
USED IN  : get_des_position
CREATED  : Mar 1999
PURPOSE  : Calculates the "desired position" between 2 prey agents using
trigonometry
TESTS:
setup();
add_prey(2,2,200,350,150,50,400,4,222,2,333,3);
rc_picx(py1)=>
rc_picy(py1)=>
rc_picx(py2)=>
rc_picy(py2)=>
des_position(py1,rc_picx(py2),rc_picy(py2))=>
rc_draw_blob(87.776472,32.303968,3,'black')
*/

define :method des_position(py1:prey_agent,py2_x,py2_y) -> (x,y);

    lvars diff_x,diff_y,angle_tan,angle,diff2_x,diff2_y;

    ;;; Calculate the difference between the 2 sets of coordinates
    difference(rc_picx(py1),py2_x) -> diff_x;
    difference(rc_picy(py1),py2_y) -> diff_y;

    ;;; Calculate cases for which difference is 0
    if diff_x = 0 then
        if py2_y > rc_picy(py1) then
            py2_x -> x;
            py2_y - D -> y;
        else
            py2_x -> x;
            py2_y + D -> y;
        endif;
    elseif diff_y = 0 then
        if py2_x > rc_picx(py1) then
            py2_x - D -> x;
            py2_y -> y;
        else
            py2_x + D -> x;
            py2_y -> y;
        endif;
    else
    ;;; If neither of the differences is 0, calculate the desired position
    ;;; using cos, sin and tan.
        diff_y/diff_x -> angle_tan;
        arctan(angle_tan) -> angle;
        cos(angle) * D -> diff2_x;
        sin(angle) * D -> diff2_y;

        if py2_x > rc_picx(py1)
        and py2_y > rc_picy(py1) then
            py2_x - diff2_x -> x;
            py2_y - diff2_y -> y;
        elseif py2_x > rc_picx(py1)
        and py2_y < rc_picy(py1) then
            py2_x - diff2_x -> x;
            py2_y + diff2_y -> y;
        elseif py2_x < rc_picx(py1)
        and py2_y < rc_picy(py1) then
            py2_x + diff2_x -> x;
            py2_y + diff2_y -> y;
        elseif py2_x < rc_picx(py1)
        and py2_y > rc_picy(py1) then
            py2_x + diff2_x -> x;
            py2_y - diff2_y -> y;
        endif;
    endif;


enddefine;


/*
METHOD   : get_des_position (py) -> (new_x, new_y)
INPUTS   : py is a prey agent
OUTPUTS  : new_x, new_y
  Where  :
    new_x is the new x position
    new_y is the new y position
USED IN  : flock_to_a_herd
CREATED  : Mar 1999
PURPOSE  : Calculates a desired position in the world by taking a weighted
average of all the local desired positions.
TESTS:
setup();
add_prey(4,2,200,350,150,50,400,4,222,2,333,3);
run_agents(1);
visible_prey(py1)=>
get_des_position(py1)=>
*/

define :method get_des_position(py:prey_agent) -> (new_x,new_y);

    lvars prey_details,prey_name,prey,dist,ps_x,ps_y,position_list=[];

    ;;; For all the visible prey
    for prey_details in visible_prey(py) do

        ;;;Get prey
        prey_details --> ! [?prey_name ?dist];
        valof(prey_name) -> prey;

        ;;; if the prey fall within local flocking radius
        if dist <= flocking_radius then

            ;;; Get the desired position for that prey and add it to a
            ;;; list of desired positions
            des_position(py,rc_picx(prey),rc_picy(prey)) -> (ps_x,ps_y);
            ;;;rc_draw_blob(ps_x,ps_y,2,'black');
            [^ps_x ^ps_y ^dist]::position_list->position_list;
        endif;
    endfor;
    ;;;position_list=>

    ;;; Calculate the weighted average of the desired positions
    weighted_average_dsq(position_list,rc_picx(py),rc_picy(py))->(new_x,new_y);
    ;;;rc_draw_blob(new_x,new_y,3,'black');

enddefine;



/*
METHOD   : flock_to_a_herd (prey) -> (new_x, new_y)
INPUTS   : prey is a prey agent
OUTPUTS  : new_x, new_y
  Where  :
    new_x is the new x coordinate
    new_y is the new y coordinate
USED IN  : perform_action_ruleset
CREATED  : Mar 1999
PURPOSE  : Calculates new coordinates for a move towards other prey agents
using a herding algorithm
TESTS:
setup();
'herd' -> flock_type;
add_prey(10,2,200,350,150,50,400,4,222,2,333,3);
run_agents(200);
*/

define :method flock_to_a_herd(prey:prey_agent) -> (new_x,new_y);

    ;;;prey=>
    lvars py_name,py,dist,x,y,tar_x,tar_y;

    ;;; Get nearest prey
    hd(visible_prey(prey)) --> ! [?py_name ?dist];
    valof(py_name) -> py;

    ;;; If the nearest prey is outside the local flocking radius, move towards
    ;;; the nearest prey
    if dist > flocking_radius then
        move_towards_point(rc_picx(prey),rc_picy(prey),
            prey_flocking_speed(prey),rc_picx(py),rc_picy(py))
            -> (new_x,new_y);
    ;;; Else calculated the desired position and move towards it
    else
        get_des_position(prey) -> (tar_x,tar_y);
        move_towards_point(rc_picx(prey),rc_picy(prey),
            prey_flocking_speed(prey),tar_x,tar_y) -> (new_x,new_y);
    endif;

    ;;;If move would take prey into an object, make a random move
    ;;; instead.
    if in_object(prey,new_x,new_y) then
        0 -> random_target_count(prey);
        random_search(prey) -> (new_x,new_y);
    endif;

enddefine;
