/*
PROCEDURE: add_obstacles (amount, method)
INPUTS   : amount, method
  Where  :
    amount is an amount of obstacles
    method is a number (either 1 or 2)
OUTPUTS  : NONE
USED IN  : world_create_panel
CREATED  : Feb 1999
PURPOSE  : Adds obstacles to the window using either a pattern (method = 1) or
randomly (method = 2)
TESTS:
setup();
add_obstacles(30,1);
add_obstacles(20,2)
*/

define add_obstacles(amount,method);

    ;;; If manual arrangement is required
    if method = 1 do
        lvars count = 1,
              ;;; Calculate position for the first obstacle
              start_x = -2*world_width/5,
              start_y = -world_height/3;

        ;;; For each obstacle to be added
        for count to amount do
            lvars line_number,line_amount,x,y;

            ;;;Calculate how many can fit on a line
            ((world_width - 20)div(obstacle_size+8)) -> line_amount;

            ;;; What line are we currently on
            obstacle_count div line_amount -> line_number;

            ;;; Calculate coordinates
            start_x + (14 * (obstacle_count - (line_number*line_amount))) -> x;
            start_y + (14 * line_number) -> y;

            ;;; Create new obstacle
            new_obstacle([^x ^y %gensym("ob")%]) ->;
        endfor;

    ;;; If random arrangement required
    elseif method = 2 do
        lvars x=1;

        ;;; For each obstacle to be added
        for x to amount do
            ;;; Create a new obstacle in a random position in the world
            new_obstacle([%random0(world_width) - (world_width/2)%
                    	   %random0(world_height) - (world_height/2)% 
			   %gensym("ob")%]) ->;
        endfor;
    endif;


enddefine;



/*
PROCEDURE: add_food2 (amount, method)
INPUTS   : amount, method
  Where  :
    amount is an amount of food
    method is a number (either 1 or 2)
OUTPUTS  : NONE
USED IN  : world_create_panel
CREATED  : Feb 1999
PURPOSE  : Adds food obstacles to the window using either a pattern
(method = 1) or randomly (method = 2)
TESTS:
setup();
add_food2(40,1);
add_food2(40,2);
*/

define add_food2(amount,method);

    ;;; If manual arrangement required
    if method = 1 do
        lvars count = 1,
              ;;; Calculate position for the first obstacle
              start_x = -2*world_width/5,
              start_y = -world_height/6;

        ;;; For each food object to be added
        for count to amount do
            lvars line_number,line_amount,x,y;

            ;;;Calculate how many can fit on a line
            ((world_width - 20)div(obstacle_size+8)) -> line_amount;

            ;;; What line are we currently on
            food_count div line_amount -> line_number;

            ;;; Calculate coordinates
            start_x + (14 * (food_count - (line_number*line_amount))) -> x;
            start_y + (14 * line_number) -> y;

            ;;; Create new food object
            new_food([^x ^y %gensym("fd")%]) ->;
        endfor;

    ;;; If random arrangement required
    elseif method = 2 do
        lvars count = 1;

        ;;; For each food object to be added
        for count to amount do

            ;;; Create a new food object in a random position in the world
            new_food([%random0(world_width) - (world_width/2)%
                       %random0(world_height) - (world_height/2)% 
		       %gensym("fd")%]) ->;
        endfor;
    endif;

enddefine;


/*
PROCEDURE: add_predators (amount, method, energ, full, hung, starv,
range, speed)
INPUTS   : amount, method, energ, full, hung, starv, range, speed
  Where  :
    amount is the amount of predators
    method is a number either 1 or 2
    energ is the predators start energy
    full is a the predators fullup threshold
    hung is a the predators hunger threshold
    starv is a the predators starving threshold
    range is a the predators attack range
    speed is a the predators attack speed
OUTPUTS  : NONE
USED IN  : predator_create_panel
CREATED  : Feb 1999
PURPOSE  : Adds predators to the window using either a pattern (method = 1)
or randomly (method = 2). Also sets a number of variables within each predator
TESTS:
setup();
add_predators(30,1,200,350,150,50,400,4);
add_predators(30,2,200,350,150,50,400,4);
*/

define add_predators(amount,method,energ,full,hung,starv,range,speed);

    ;;; If manual arrangement is required
    if method = 1 do
        lvars count = 1,
              ;;; Calculate position for the first predator
              start_x = -2*world_width/5,
              start_y = world_height/8;

        ;;; For each predator to be added
        for count to amount do
            lvars line_number,line_amount,x,y;

            ;;; Calculate the number that can fit on a line
            ((world_width - 20)div((2*agent_size)+12)) -> line_amount;

            ;;; Calculate what line we are on
            predator_count div line_amount -> line_number;

            ;;; Calculate coordinates
            start_x + (22 * (predator_count - (line_number*line_amount))) -> x;
            start_y + (20 * line_number) -> y;

            ;;; Create new predator
            new_predator([^x ^y %gensym("pd")% ^energ ^full ^hung ^starv
                          ^range ^speed]) ->;
        endfor;

    ;;; If random arrangement required
    elseif method = 2 do
        lvars count = 1;

        ;;; For each predator to be added
        for count to amount do

            ;;; Create a predator at a random position in teh world
            new_predator([%random0(world_width) - (world_width/2)%
                    %random0(world_height) - (world_height/2)% %gensym("pd")%
                    ^energ ^full ^hung ^starv ^range ^speed]) ->;
        endfor;
    endif;

enddefine;





/*
PROCEDURE: add_prey (amount, method, energ, full, hung, starv, er, es,
gr, gs, fr, fs)
INPUTS   : amount, method, energ, full, hung, starv, er, es, gr, gs, fr, fs
  Where  :
    amount is the amount of prey
    method is a number either 1 or 2
    energ is the prey start energy
    full is a the prey fullup threshold
    hung is a the prey hunger threshold
    starv is a the prey starving threshold
    er is the preys escape range
    es is the preys escape speed
    gr is the preys grazing range
    gs is the preys grazing speed
    fr is the preys flocking range
    fs is the preys flocking speed
OUTPUTS  : NONE
USED IN  : prey_create_panel
CREATED  : Feb 1999
PURPOSE  : Adds prey to the window using either a pattern (method = 1)
or randomly (method = 2). Also sets a number of variables within each prey
TESTS:
setup();
add_prey(30,1,200,350,150,50,400,4,222,2,333,3);
add_prey(30,2,200,350,150,50,400,4,222,2,333,3);
*/

define add_prey(amount,method,energ,full,hung,starv,er,es,gr,gs,fr,fs);

    ;;; If manual arrangement required
    if method = 1 do
        lvars count = 1,
              ;;; Calculate the position of the first prey
              start_x = -2*world_width/5,
              start_y = world_height/4;

        ;;; For each prey to be added
        for count to amount do
            lvars line_number,line_amount,x,y;

            ;;; Calculate the number that can fit on a line
            ((world_width - 20)div((2*agent_size)+12)) -> line_amount;

            ;;; Calculate the line we are currently on
            prey_count div line_amount -> line_number;

            ;;; Calculate the coordinates
            start_x + (22 * (prey_count - (line_number*line_amount))) -> x;
            start_y + (20 * line_number) -> y;

            ;;; Create the new prey
            new_prey([^x ^y %gensym("py")% ^energ ^full ^hung ^starv
                        ^er ^es ^gr ^gs ^fr ^fs]) ->;
        endfor;

    ;;; If random arrangement required
    elseif method = 2 do
        lvars count = 1;

        ;;; For each prey to be added
        for count to amount do

            ;;; Create a new prey at a random position in the world
            new_prey([%random0(world_width) - (world_width/2)%
                    %random0(world_height) - (world_height/2)% %gensym("py")%
                    ^energ ^full ^hung ^starv ^er ^es ^gr ^gs ^fr ^fs]) ->;
        endfor;
    endif;


enddefine;
