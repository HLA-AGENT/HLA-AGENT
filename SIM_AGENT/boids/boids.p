/*  
    TO RUN:
    Load file into pop and type setup(num_cycles,num_boids);
    Where num_cycles = number of cycles to run for (false = infinite run)
          num_boids  = number of boids to run in simulation

    Authors:  Elizabeth Gordon & Michael Lees
    Description:  Simple flocking demo.

    This demo is based on the Boids work done by Craig Reynolds.  
    More information on Boids can be found on his website at 
    http://www.red3d.com/cwr/boids/.  

    A boid is a very simple agent, using three rules to navigate around 
    its environment.  The result is similar to the flocking behaviour of 
    fish or cattle.  The original Boids existed in a 3-d environment, and 
    were intended to suggest flocks of birds.

    The three behaviours are:  
    1.  avoid crowding  
    2.  fly the same direction as the flock
    3.  fly towards other members of the flock

    Each behaviour votes to steer in a particular direction, rating its vote 
    according to urgency.  The final direction is computed using a weighted 
    average of the three votes.

    This demo only adjusts the heading of the boids, not the speed.  This 
    means that it is quite easy for the boids to lose each other.  Really, 
    a boid that is far away from the flock should speed up.  

    CHANGES...  
    by: Michael Lees date: 19/03/03 
    
    Desc: Updated graphics to now use triangles rather than
    points. Also added rules to allow speed matching using
    acceleration. Added display for sensors.
    Fixed minor bug with movement, originally the rule 'update' had,
     [x = (cos(h) / sim_myself.boid_speed) + sim_myself.sim_x]
     [y = (sin(h) / sim_myself.boid_speed) + sim_myself.sim_y]
    The '/' here should in fact be '*'
    Added some global vars to set parameters of simulation (see below).
    Changed the voting system to use the accumulator as described in
    boids paper, rather than the proportion system.
    Changed moving away and toward flock so that the new heading is
    calculated correctly. 
*/
    

uses prblib;            ;;; poprulebase
uses rclib;             ;;; rc graphics library
uses simlib;            ;;; necessary for sim agent
uses sim_agent;         ;;; sim_agent provides base classes
uses sim_picagent;      ;;; extensions of sim_agent for graphics
uses rc_window_object;  ;;; for manipulation of windows

;;; Simulation Parameters and Globals
vars WIN_X=1150;      ;;; Size of xgraphic window
vars WIN_Y=800;       ;;; For full screen use x=1100 y=800
vars DRAW_GRAPHICS=true; ;;;false=no graphics
vars SHOW_SENS=false;  ;;; Draws circle and lines representing sensors
vars MAX_ACCEL=2;     ;;; Max accel of boid (values above are clipped to this)
vars MIN_ACCEL=(-2);  ;;; Min accel of boid (values below are clipped)
vars MAX_SPEED=10;     ;;; Max speed of boid
vars MIN_SPEED=1;     ;;; Min speed of boid
vars SEPERATION=50;   ;;; Distance that boids try to obtain between one another
vars SENSOR_RANGE=100; ;;;How far boids can see

;;; Other globals
vars boid_rulesystem; ;;; boids rule system
vars objectlist=[];   ;;; Objects in the environment

;;; First define the class.  It inherits from sim_movable_agent, which 
;;; means that it can be displayed in a window and moved with the mouse.  
;;; The inherited slots sim_x and sim_y specify the agent's position.
define :class boid; is sim_movable_agent, rc_rotatable;
    ;;;
    ;;; heading degrees:
    ;;;               90
    ;;;          180__|__0
    ;;;               |
    ;;;              270 
    slot boid_heading = 0;   ;;; the boid's heading
    slot boid_speed = MIN_SPEED;     ;;; the boid's speed
    slot boid_fieldofview = 180; ;;; Angle the boid can see in deg

    ;;; The rulesystem that the agent should use.  (used by sim_agent)
    slot sim_rulesystem = boid_rulesystem;

    ;;; Sets how much info to print at end of cycle 
    ;;; (0=none, 1=some, 2=lots, 3=heading+accel info only)
    slot printlevel = 0;
    slot rc_mouse_limit == 10;
    ;;; Specifies functions for obtaining information about other objects.  
    ;;; The number specifies how far the agent can see.
    slot sim_sensors = [{sim_sense_agent ^SENSOR_RANGE}];
enddefine;

;;; Constructor which allows you to specify heading and position
define newboid(x,y,heading)->aboid;
    instance boid; 
	boid_heading = heading;
    	sim_x = x;
    	sim_y = y;
    endinstance->aboid;
    
    ;;; The following code displays the boid using the rc_pic_strings
    ;;; slot of the sim_picagent class (see help sim_picagent)
 
    if(DRAW_GRAPHICS) then
    lvars label=''><aboid.sim_name;	
    lvars x1 = -SENSOR_RANGE;
    lvars y1 = SENSOR_RANGE;
    lvars startang= 64*(180-(0.5*aboid.boid_fieldofview));
    lvars width = 2*SENSOR_RANGE;
    lvars angleinc = 64*aboid.boid_fieldofview;
    lvars linex =sin(0.5*aboid.boid_fieldofview)*SENSOR_RANGE;
    lvars liney =cos(0.5*aboid.boid_fieldofview)*SENSOR_RANGE;
    lvars line2x = -linex;
 
    if(SHOW_SENS) then
    	[[COLOUR 'blue' CLOSED {-3 -3} {0 10} {3 -3}]
     	 ;;;     [COLOUR 'grey' rc_draw_arc {^x1 ^y1 ^width ^width ^startang ^angleinc}]
     	 [COLOUR 'grey' {^linex ^liney} {0 0} {^line2x ^liney}]
     	 [COLOUR 'grey' CIRCLE{0 0 ^y1}]]->rc_pic_lines(aboid);
    else
	[[COLOUR 'blue' CLOSED {-3 -3} {0 10} {3 -3}]]->rc_pic_lines(aboid);
    endif;

    ;;; Puts label of agents name on screen (nearby agent)
    [FONT  '-*-helvetica-medium-r-*-*-8-*-*-*-*-*-*-*' {10 0 ^label}] 
    -> rc_pic_strings(aboid);
endif;
enddefine;

;;;Generates valid random heaing and position
define randomboid()->aboid;
    newboid(random(WIN_X) - (0.5*WIN_X),
	    random(WIN_Y) - (0.5*WIN_Y),
	    random(360)-1)->aboid;
enddefine;

;;; sim_sense_agent specifies what info each agent gets about the things 
;;; it senses.  This function is for sensing other boids.
;;; Another sim_sense_agent method would be necessary for stationary objects
;;; if there were any.  The dist parameter is how far apart the boids are, 
;;; and a1 is the current boid.
define :method sim_sense_agent(a1:boid, a2:boid, dist);
    unless a1 = a2   ;;; check that the boid isn't sensing itself
	    then
	;;; Get heading in degrees to the object we are sensing
	;;; See SIM_GEOM for info on sim_heading_from and other geometry stuff
	lvars heading_to_object = 
	     sim_heading_from(a1.sim_x,a1.sim_y,a2.sim_x,a2.sim_y);
	;;; This specifies what information should be added to the database.
	;;; Then if the difference between my heading and heading to
	;;; sensed object is less than 1/2 field of view, the object
	;;; can be seen.
	if(abs(sim_degrees_diff(a1.boid_heading,heading_to_object)) < 
	   ((a1.boid_fieldofview)/2)) then
	    ;;; Information to be added to the database.
	    [new_sense_data	     
 	     ^(a2.boid_heading)
 	     ^(a2.boid_speed)
 	     ^(a2.sim_x)
 	     ^(a2.sim_y)
 	     ^dist
	     ]
	endif;
    endunless
enddefine;


    
;;;A simple redefinition for supressing graphics.
define :method sim_move_to(obj:boid,x,y,drawmode);
    if (DRAW_GRAPHICS) then
	call_next_method(obj, x, y, drawmode);
    else
	x -> sim_x(obj);
	y -> sim_y(obj);
    endif;
enddefine;   


;;; This determines overall accel from the three votes. It
;;; accumulates until the overall urgency has reached 100. So if u1 is
;;; 100 then v=v1. Whereas if u1=50 and u2=50 then v=(0.5*v1)+(0.5*v1)
;;; etc.
define accumulate_total(v1,u1,v2,u2,v3,u3)->v;
    ;;;This uses the accumulator method
    
    lvars amount_left=0,x;
    ;;; We use 100 as the accumulative total so firstly we make sure
    ;;; the sum of u1,u2, and u3 >= 100. If not we scale them by
    ;;; appropriate even amounts. 
    if ((u1+u2+u3)<100) then
    	u1/(u1+u2+u3)*100->u1;
    	u2/(u1+u2+u3)*100->u2;
    	u3/(u1+u2+u3)*100->u3;
    endif;

    ;;;All individual u values should be =< 100
    ;;;This shouldn't happen but to ensure correctness
    if(u1>100) then 100->u1; endif;    
    if(u2>100) then 100->u2; endif;    
    if(u3>100) then 100->u3; endif;    
    
    (u1/100)*v1->v;
    100-u1->amount_left;
    if(amount_left>u2) then u2->amount_left; endif;	
    
    for x from 1 to amount_left do
	v+(v2/100)->v;		
    endfor;
    
    100-u1-u2->amount_left;
    
    for x from 1 to amount_left do
	v+(v3/100)->v;
    endfor;
enddefine;

define accumulate_angle(v1,u1,v2,u2,v3,u3)->v;

     lvars amount_left=0;
     lvars vector=conspair(0,0);
    ;;;This uses the accumulator method for heading
     ;;;    conspair(0,0)->vector;

    ;;; We use 100 as the accumulative total so firstly we make sure
    ;;; the sum of u1,u2, and u3 >= 100. If not we scale them evenly by
    ;;; appropriate amounts. 
    if ((u1+u2+u3)<100) then
    	u1/(u1+u2+u3)*100->u1;
    	u2/(u1+u2+u3)*100->u2;
    	u3/(u1+u2+u3)*100->u3;
    endif;
    
    ;;;All u values should be =< 100
    ;;;This shouldn't happen but to ensure correctness
    if(u1>100) then 100->u1; endif;    
    if(u2>100) then 100->u2; endif;    
    if(u3>100) then 100->u3; endif;    


    ;;; We use vector format to accumulate angles
    cos(v1)*(u1/100)+front(vector)->front(vector);
    sin(v1)*(u1/100)+back(vector)->back(vector);
 
    100-u1->amount_left;
    if(amount_left>u2) then u2->amount_left; endif;
 
    cos(v2)*(amount_left/100)+front(vector)->front(vector);
    sin(v2)*(amount_left/100)+back(vector)->back(vector);
 
    100-u1-u2->amount_left;
    if(amount_left < 0) then 0->amount_left; endif;
    cos(v3)*(amount_left/100)+front(vector)->front(vector);
    sin(v3)*(amount_left/100)+back(vector)->back(vector);    
     
    ;;; Translate vector back into a heading
    sim_angle_xy(front(vector),back(vector))->v;
enddefine;


;;; Used when determining if an object is on the left or right of a
;;; boid 
define cross_product(my_x, my_y,
		     flock_x,flock_y, 
		     flock_heading)->d;
    lvars d = cos(flock_heading)*(my_y-flock_y)
	 -sin(flock_heading)*(my_x-flock_x);
enddefine;


;;; This is used in the rule system to determine an appropriate new
;;; heading when we are too close to the flock. The procedure uses the
;;; cross product to determine if we are on the left or right of the
;;; flock and then deviates to avoid the flock in the appropriate
;;; direction
define move_away(my_x, my_y, 
		 flock_x,flock_y, 
		 flock_heading, my_heading)->new_heading;    
    lvars old_heading, new_heading;
    ;;;calculate cross product
    lvars d = cross_product(my_x, my_y,flock_x,flock_y,flock_heading);
    ;;; How fast we turn should reflect how close we are to the flock
    ;;; relative to the ideal seperation
    lvars rate_of_turn = 
	 (1/sim_distance_from(my_x,my_y,flock_x,flock_y))*SEPERATION;

    if (d==0) then ;;; On the same line as flock so deviate slightly
	new_heading+1->new_heading;
    elseif (d>0) then ;;; On left hand side of flock so deviate left to avoid
	my_heading->old_heading;
        my_heading + rate_of_turn->new_heading;
    else ;;; Were on the right hand side so deviate right to avoid 
	my_heading->old_heading;
       	my_heading - rate_of_turn->new_heading;
    endif;
enddefine;   

;;; This is the same as above except the turning is reversed, that is
;;; if we're on the left of the flock then we turn right to aim for
;;; the centre of the flock.
define move_toward(my_x, my_y, 
		 flock_x,flock_y, 
		 flock_heading, my_heading)->new_heading;    
    lvars old_heading, new_heading;
    lvars d = cross_product(my_x, my_y,flock_x,flock_y,flock_heading);
    lvars rate_of_turn = 
	 (1/sim_distance_from(my_x,my_y,flock_x,flock_y))*SEPERATION;
    if (d==0) then ;;; On the same line as flock set heading to flock heading 
	flock_heading->new_heading;
    elseif (d>0) then ;;; On left hand side of flock so deviate right to join
	my_heading->old_heading;
        my_heading - rate_of_turn->new_heading;
    else ;;; On right hand side so deviate left to join	
	my_heading->old_heading;
       	my_heading + rate_of_turn->new_heading;
    endif;
enddefine;   
    

;;; Define the rulesystem, and specify what rulesets it includes.
define :rulesystem boid_rulesystem;
    include: boid_rules 
enddefine;

;;; Define the ruleset.
define :ruleset boid_rules;

    ;;; Rule to create database entries  
    RULE start_heading
    [NOT flock_heading =]
    [NOT flock_vector_x =]
    [NOT flock_vector_y =]
    [NOT flock_speed =]
    [NOT flock_xpos =]
    [NOT flock_ypos =]
    [NOT min_dist =] 
    [NOT flock_size =]
    ==> 
    [flock_heading 0]
    [flock_vector_x 0]
    [flock_vector_y 0]
    [flock_speed 0]
    [flock_xpos 0]
    [flock_ypos 0]
    [flock_size 0]
    [min_dist 500]

  ;;; Resets values for calculating new averages this cycle
    RULE reset
    [flock_heading =]  
    [flock_speed =]
    [flock_xpos =]
    [flock_ypos =]
    [flock_size =]
    [min_dist =]
    [flock_vector_x =]
    [flock_vector_y =]
    ==>
    [LVARS
     [range = SENSOR_RANGE]
     ]
    [REPLACE 1 [flock_heading 0]]
    [REPLACE 2 [flock_speed 0]]
    [REPLACE 3 [flock_xpos 0]]
    [REPLACE 4 [flock_ypos 0]]
    [REPLACE 5 [flock_size 0]]
    [REPLACE 6 [min_dist ?range]]
    [REPLACE 7 [flock_vector_x 0]]
    [REPLACE 8 [flock_vector_y 0]]
  

    ;;; For each new agent sensed, recompute the averages

    ;;; The first rule applies when the distance is less than 
    ;;; the current minimum.
    ;;; The rule checks for new sensory data.
    ;;; Then it checks that all the correct entries are in the database.  
    ;;; The WHERE condition specifies that the distance is less than the 
    ;;; minimum.  The last rule makes sure this rule doesn't run when we've 
    ;;; already computed the new heading.

    ;;; The first two rules 'accumulate' the totals for this cycle,
    ;;; the third rule then calculates the mean for each value.
    RULE accumulate_totals1
    [new_sense_data ?heading ?speed ?x ?y ?dist]
    [flock_heading ?fh]
    [flock_vector_x ?fvx]
    [flock_vector_y ?fvy]
    [flock_speed ?fs]
    [flock_xpos ?fx]
    [flock_ypos ?fy]
    [flock_size ?sz]
    [min_dist ?m]
    [WHERE dist < m]
    [NOT update ==]
    ==>
    [LVARS 
     [new_size = (sz + 1)]     
     [new_flock_vector_x = fvx + cos(heading)]
     [new_flock_vector_y = fvy + sin(heading)]
     [new_speed = (fs + speed) ]
     [new_xpos = (fx + x)]
     [new_ypos = (fy + y)]
     ]    
    [DEL 1]
    [DEL 2]
    [DEL 3]
    [DEL 4]
    [DEL 5]
    [DEL 6]
    [DEL 7]
    [DEL 8]
    [DEL 9]
    [flock_heading ?fh]
    [flock_speed ?new_speed]
    [flock_xpos ?new_xpos]
    [flock_ypos ?new_ypos]
    [flock_size ?new_size]
    [flock_vector_x ?new_flock_vector_x]
    [flock_vector_y ?new_flock_vector_y]
    [min_dist ?dist]


    ;;; The second rule applies when the distance is not less than 
    ;;; the current minimum
    RULE accumulate_totals2
    [new_sense_data ?heading ?speed ?x ?y ?dist]
    [flock_heading ?fh] 
    [flock_vector_x ?fvx]
    [flock_vector_y ?fvy]
    [flock_speed ?fs]
    [flock_xpos ?fx]
    [flock_ypos ?fy]
    [flock_size ?sz]
    [min_dist ?m]
    [WHERE dist >= m]
    [NOT update ==]
    ==>
    [LVARS 
     [new_size = (sz + 1)]     
     [new_flock_vector_x = fvx + cos(heading)]
     [new_flock_vector_y = fvy + sin(heading)]
     [new_speed = (fs + speed) ]
     [new_xpos = (fx + x)]
     [new_ypos = (fy + y)]
     ]
    [DEL 1]
    [DEL 2]
    [DEL 3]
    [DEL 4]
    [DEL 5]
    [DEL 6]
    [DEL 7]
    [DEL 8]
    [flock_heading ?fh]
    [flock_speed ?new_speed]
    [flock_xpos ?new_xpos]
    [flock_ypos ?new_ypos]
    [flock_size ?new_size]
    [flock_vector_x ?new_flock_vector_x]
    [flock_vector_y ?new_flock_vector_y]
    

    ;;; We have totaled all values from new sense data now we average them
    RULE compute_averages
    [NOT new_sense_data ==]
    [flock_heading ?fh] 
    [flock_vector_x ?fvx]
    [flock_vector_y ?fvy]
    [flock_speed ?fs]
    [flock_xpos ?fx]
    [flock_ypos ?fy]
    [flock_size ?sz]
    [WHERE sz>0]
    ==>
    [LVARS 
     [new_flock_vector_x = fvx / sz]
     [new_flock_vector_y = fvy /sz]
     [new_speed = fs /sz ]
     [new_xpos = fx / sz]
     [new_ypos = fy / sz]
     [new_heading = sim_angle_xy(new_flock_vector_x,new_flock_vector_y)]
     ]
    [DEL 1]
    [DEL 2]
    [DEL 3]
    [DEL 4]
    [DEL 5]
    [DEL 6]
    [flock_heading ?new_heading]
    [flock_speed ?new_speed]
    [flock_xpos ?new_xpos]
    [flock_ypos ?new_ypos]    
    [flock_vector_x ?new_flock_vector_x]
    [flock_vector_y ?new_flock_vector_y]
    

    ;;; The first boids behaviour tries to move away from 
    ;;; other boids when conditions are too crowded.  The first and 
    ;;; basic rule computes a heading that will move the boid away from the 
    ;;; flock, and assigns it an urgency according to how crowded it is.
    RULE avoid_crowding 
    [flock_heading ?fh]
    [flock_size ?sz]
    [flock_xpos ?x]
    [flock_ypos ?y]
    [min_dist ?m] ;;; The maximum m can possibly be is 'SENSOR_RANGE'
    [WHERE m <= SEPERATION] ;;; Too close for ideal seperation
    [WHERE sz>0]
    [WHERE x  /== sim_myself.sim_x]
    [WHERE y  /== sim_myself.sim_y]
    [NOT desired_heading_1 ==]
    [NOT desired_accel_1 ==]    
    ==>
    [LVARS 
     ;;; deviate away from flock heading in proportion to how close I
     ;;; am to the flock
     [heading = move_away(sim_myself.sim_x, 
		      	  sim_myself.sim_y,x,y,
		      	  fh,			  
		      	  sim_myself.boid_heading)]
     ;;; If I'm very close to someone (small m) increase the urgency. Urgency
     ;;; can be a maximum of 100
     [urgency = ((SENSOR_RANGE - m)/SENSOR_RANGE) * 100]
     [accel = ((m/SENSOR_RANGE)-(0.5*MAX_ACCEL)) * MAX_ACCEL]
     ]
    [desired_heading_1 ?heading ?urgency]
    [desired_accel_1 ?accel ?urgency]
    
    ;;; If the flock is really far away, vote for the flock heading with 
    ;;; low urgency. ie cohesion should have more chance (move closer to flock)
    ;;; Use max accel*3/4 with low urgency also (try and catch flock up).
    RULE avoid_crowding2
    [flock_heading ?fh]
    [flock_size ?sz]
    [min_dist ?m]
    [WHERE m > SEPERATION]
    [WHERE sz > 0]
    [NOT update ==]
    [NOT desired_heading_1 ==]
    [NOT desired_accel_1 ==]    
    ==>
    [LVARS 
     [heading = fh]
     [accel = (MAX_ACCEL*0.75)]
     ]
    [desired_heading_1 ?heading 20]
    [desired_accel_1 ?accel 20]

    ;;; For crowded situations where the boid is at the center of the flock 
    ;;; (so direction to move is difficult to compute).  Votes for the 
    ;;; current flock heading (other boids should move away from this one).
    ;;; Also maintain current speed (ie 0 acceleration)
    RULE avoid_crowding3
    [flock_heading ?fh]
    [flock_size ?sz]
    [WHERE sz>0]
    [NOT desired_heading_1 ==]
    [NOT desired_accel_1 ==]    
    [NOT update ==]
    ==>
    [LVARS 
     [heading = fh]]
    [desired_heading_1 ?heading 50]
    [desired_accel_1 0 50]

    ;;; The second boids behaviour tries to steer in the same 
    ;;; direction as other boids.  It always votes for the current flock 
    ;;; heading.  The urgency is assigned according to the difference between 
    ;;; the boid's current heading and the flock heading. Also change
    ;;; my own speed to try and match the speed of the flock
    RULE match_heading
    [flock_heading ?fh]
    [flock_speed ?fs]
    [flock_size ?sz]
    [WHERE sz>0]
    [NOT update ==]
    [NOT desired_heading_2 ==]
    [NOT desired_accel_2 ==]    
    ==>
    [LVARS 
     [heading = fh]
     [urgency = (abs(sim_degrees_diff(sim_myself.boid_heading,fh)) / 180.0) * 100.0]
     [accel = (fs - sim_myself.boid_speed)]
     [max_speed_diff = MAX_SPEED - MIN_SPEED]
     [accel_urgency = (abs(accel)/max_speed_diff) *100]
    ]     
    [desired_heading_2 ?heading ?urgency]
    [desired_accel_2 ?accel ?accel_urgency]

    ;;; The third boids behaviour tries to move towards other boids 

    ;;; The first rule votes for heading toward the center of the flock, 
    ;;; with urgency according to how far away from the nearest boid it is.
    RULE match_position
    [flock_size ?sz]
    [flock_heading ?fh]
    [flock_xpos ?x]
    [flock_ypos ?y]
    [WHERE sz>0]
    [WHERE x /== sim_myself.sim_x]  ;;;We're not centre of flock
    [WHERE y /== sim_myself.sim_y]
    [NOT update ==]
    [NOT desired_heading_3 ==]
    [NOT desired_accel_3 ==]    
    ==>
    [LVARS 
     [d = sim_distance_from(x, y, sim_myself.sim_x, sim_myself.sim_y)]
     [h = move_toward(sim_myself.sim_x,sim_myself.sim_y,x,y,fh,sim_myself.boid_heading)]   
     [urgency = (d/SENSOR_RANGE) * 100]
     [accel = (d/SENSOR_RANGE)*MAX_ACCEL]
    ]
    [desired_heading_3 ?h ?urgency]
    [desired_accel_3 ?accel ?urgency]


    ;;; The second rule is 
    ;;; for cases where the boid is already at the center of the flock, 
    ;;; in which case it votes to keep the same heading (with medium
    ;;; urgency) and speed.
    RULE match_position2
    [NOT desired_heading_3 ==]
    [NOT desired_accel_3 ==]    
    [flock_size ?sz]
    [WHERE sz>0]
    [NOT update ==]
    ==>
    [LVARS 
     [h = sim_myself.boid_heading]
    ]
    [desired_heading_3 ?h 50]
    [desired_accel_3 0 50] 

    
    ;;; When a boid becomes seperated from the flock (ie flock_size=0)
    ;;; it takes a random heading with random acceleration
    RULE move_randomly
    [NOT desired_heading_1 ==]
    [NOT desired_accel_1 ==]    
    [NOT desired_heading_2 ==]
    [NOT desired_accel_2 ==]   
    [NOT desired_heading_3 ==]
    [NOT desired_accel_3 ==]   
    [flock_size ?sz]
    [WHERE sz=0]
    [NOT update ==]
    ==>
    [LVARS 
     [h= sim_myself.boid_heading+(random(10) -5)] ;;;[-5,5]
     [a = ((random(3) -2)/2)] ;;; random accel (-0.1,0,0.1)
    ]
    [desired_heading_4 ?h 100]
    [desired_accel_4 ?a 100] 

    
    
    ;;; This rule takes the three votes, indicated by desired_heading entries 
    ;;; in the database, and computes a weighted average.  The result is 
    ;;; added as an update entry, and the three desired headings are deleted.
    RULE calculate_heading
    [desired_heading_1 ?h1 ?u1]
    [desired_heading_2 ?h2 ?u2]
    [desired_heading_3 ?h3 ?u3]
    [POP11 if(printlevel(sim_myself)=3) then
    	 ['I want to go' ^h1 'with' ^u1]=>
    	 ['I want to go' ^h2 'with' ^u2]=>
         ['I want to go' ^h3 'with' ^u3]=> endif; 
     ]
    [NOT update ==]
    ==>
    [LVARS 
     [h = (accumulate_angle(h1,u1,h2,u2,h3,u3) mod 360)]
    ]
    [POP11 if(printlevel(sim_myself)=3) then
    	 ['I ended up going' ^h]=> endif;]
    [DEL 1]
    [DEL 2]
    [DEL 3]
    [update ?h]

    ;;; This does the same as the rule above except for acceleration    
    RULE calculate_accel
    [desired_accel_1 ?a1 ?u1]
    [desired_accel_2 ?a2 ?u2]
    [desired_accel_3 ?a3 ?u3]
    [POP11 if(printlevel(sim_myself)=3) then
    	 ['I want to accel' ^a1 'with' ^u1]=>
    	 ['I want to accel' ^a2 'with' ^u2]=>
    	 ['I want to accel' ^a3 'with' ^u3]=>
     endif;]

    [NOT update_accel ==]
    ==>    
    [LVARS     
     [a = accumulate_total(a1,u1,a2,u2,a3,u3)]
    ]
    [POP11 if(printlevel(sim_myself)=3) then
    	 ['I ended up accel' ^a]=>endif;]
    [DEL 1]
    [DEL 2]
    [DEL 3]
    [update_accel ?a]

    ;;; The case when boid is on its own (seperated from flock)
    RULE calculate_single
    [desired_heading_4 ?h =]
    [desired_accel_4 ?a =] 
    [POP11 if(printlevel(sim_myself)=3) then
    	 ['Im on my own accel: '^a' heading: ' ^h]=> endif;]
    [NOT update_accel ==]
    [NOT update]
    ==>
    [DEL 1]
    [DEL 2]
    [update_accel ?a]
    [update ?h]

    ;;; Update speed to take into account new accel value
    ;;; This rule must run before update as when making the move the
    ;;; new speed should be used (ie after accel has been applied)
    RULE update_accel
    [update_accel ?a]
    ==>
    ;;;Scale accel to withing max and min allowed
    [POP11 if(a>MAX_ACCEL) then MAX_ACCEL->a; endif;]
    [POP11 if(a<MIN_ACCEL) then MIN_ACCEL->a; endif;]
    ;;;apply new accel to speed
    [POP11 sim_myself.boid_speed+a->sim_myself.boid_speed]
    ;;;Scale new speed to within max and min allowed
    [POP11 if(sim_myself.boid_speed>MAX_SPEED) then 
	 MAX_SPEED->sim_myself.boid_speed; endif;]
    [POP11 if(sim_myself.boid_speed<MIN_SPEED) then 
	 MIN_SPEED->sim_myself.boid_speed; endif;]
    [POP11 if(printlevel(sim_myself)=3) then
	 ['My new speed: ' ^(sim_myself.boid_speed)]=> endif;]
    [NOT update_accel ==]

    ;;; The update rule sets the new heading and moves the boid in the 
    ;;; correct direction.  The procedure sim_move_to assigns the sim_x and 
    ;;; sim_y variables and updates the graphics.  
    RULE update
    [update ?h]
    ==>   
    [POP11 h -> sim_myself.boid_heading]
    ;;; Calculate what are new x and y position should be given our
    ;;; speed and heading
    [LVARS      
     [x = (cos(h) * sim_myself.boid_speed) + sim_myself.sim_x]
     [y = (sin(h) * sim_myself.boid_speed) + sim_myself.sim_y]
     ]
    ;;;Rotate the pic to face new heading
    [POP11 
     if(DRAW_GRAPHICS) then
	 rc_set_axis(sim_myself, (sim_myself.boid_heading-90), true);
     endif;]
    ;;;Move boid to new x and y position
    [POP11 sim_move_to(sim_myself, x, y, true);]   
    [POP11 if(printlevel(sim_myself)=3) then
    	 ['My new heading: ' ^(sim_myself.boid_heading)]=> endif;]
    [NOT update ==]

       
    ;;; This last set of rules makes the boids wrap around if they fly off 
    ;;; the edge of the window.
    RULE wrap_posx
    [LVARS
     [xpos = (0.5*WIN_X)]
     ]
    [WHERE sim_myself.sim_x > xpos]
    ==>
    [POP11 sim_move_to(sim_myself, -xpos, sim_myself.sim_y, true)]

    RULE wrap_negx
    [LVARS
     [xpos = (0.5*WIN_X)]
     ]
    [WHERE sim_myself.sim_x < -xpos]
    ==>
    [POP11 sim_move_to(sim_myself, xpos, sim_myself.sim_y, true)]

    RULE wrap_posy
    [LVARS
     [ypos = (0.5*WIN_Y)]
     ]
    [WHERE sim_myself.sim_y > ypos]
    ==>
    [POP11 sim_move_to(sim_myself, sim_myself.sim_x, -ypos, true)]

    RULE wrap_negy
     [LVARS
     [ypos = (0.5*WIN_Y)]
     ]
    [WHERE sim_myself.sim_y < -ypos]
    ==> 
    [POP11 sim_move_to(sim_myself, sim_myself.sim_x, ypos, true)]
enddefine;


;;; This function creates the window, adds the agents, and starts the 
;;; simulation.    
define setup(num, num_boids);    
    lvars x,win,aboid;

    ;;; First create a window
    if(DRAW_GRAPHICS) then
    	rc_new_window_object(20, 20, WIN_X, WIN_Y, 
			     true,'---- Boids ----', 
			     newsim_picagent_window) ->win;
    endif;
    ;;; Create num_boids with random initial positions and heading
    for x from 1 to num_boids do
	randomboid()->aboid;
	if(DRAW_GRAPHICS) then rc_add_containers(aboid, [^win]); endif;
        aboid :: objectlist -> objectlist;
	endfor;
	
;;;	newboid(-20,10,0)->boid1;
;;;	newboid(20,-10,0)->boid2;
;;;	rc_add_containers(boid1, [^win]);
;;;	rc_add_containers(boid2, [^win]);
;;;	[^boid1 ^boid2]->objectlist;

    ;;; This allows all rules to fire, and creates a new copy of each rule 
    ;;; for each set of matching variables.  If this were not true, each 
    ;;; agent would process only the first set of sensory data.
    true->prb_allrules;

    ;;; Start sim_agent, running each agent for num cycles.
    sim_scheduler(objectlist, num);
enddefine;
    
;;;Resume simulation for num cycles
define continue(num);
    sim_scheduler(objectlist, num);      
enddefine;



;;;Below can be ignored I've redefined standard cycle printing stuff 
;;;-----------------------------------------------------------------------
;;; set printlevel=0 in boid for no printing
;;;               =1 for some printing
;;;               =2 for lots of printing

;;; vars for dumping images (making video)
;;; How many cycle per pic
vars team_dump_images = false;
;;; Filename of output
vars team_dump_file = "pic";


define vars procedure sim_scheduler_finished(objects, cycle);
    lvars objects, cycle;
    ;;; user definable
    pr('\n============== Finished. Cycle ' >< cycle >< ' ============\n');
enddefine;

define vars procedure sim_scheduler_pausing_trace(objects, cycle);
    lvars objects, cycle,input,agentnum;
    ;;; code for dumping a snapshot every team_dump_images cycles
    if isinteger(team_dump_images) and cycle mod team_dump_images == 0 then
    	sysobey('import -window "---- Boids ----" -silent ' >< 
		gensym(team_dump_file)><'.yuv');
    endif;    
enddefine;

define :method sim_agent_messages_out_trace(agent:sim_agent);
    lvars agent;
    if printlevel(agent) > 0 then
        lvars messages = sim_out_messages(agent);
        [New messages ^(sim_name(agent)) ^messages] ==>
    endif;
enddefine;

define :method sim_agent_messages_in_trace(agent:sim_agent);
    lvars agent;
    if printlevel(agent) > 0 then
        lvars messages = sim_in_messages(agent);
        [New messages ^(sim_name(agent)) ^messages] ==>
    endif;
enddefine;

define :method sim_agent_actions_out_trace(object:sim_object);
    lvars object;
    lvars actions = sim_actions(object);
    if printlevel(object) > 0 then
      	[New actions ^(sim_name(object)) ^actions] ==>
      	pr('\n\n');
    endif;
enddefine;

define :method sim_agent_messages_out_trace(agent:sim_agent);
    lvars agent;
    if printlevel(agent) > 0 then
        lvars messages = sim_out_messages(agent);
        [New messages ^(sim_name(agent)) ^messages] ==>
    endif;
enddefine;

define :method sim_agent_endrun_trace(object:sim_object);
    lvars object;
    if printlevel(object) > 0 then
        ['Data in' ^(sim_name(object)) ]==>
        prb_print_table( (sim_data(object)) );
    endif;
enddefine;

define :method sim_agent_rulefamily_trace(object:sim_object, rulefamily);
    lvars object, rulefamily;
    if printlevel(object) > 0 then
      	if printlevel(object) > 1 then
    	    ['sim_agent_rulefamily_trace '^object]==>
      	endif;
      	['Try rulefamily' ^rulefamily 'with object' ^(sim_name(object))]==>
      	pr('With Data:\n');
      	prb_print_table( (sim_data(object)) );
    endif;
enddefine;
