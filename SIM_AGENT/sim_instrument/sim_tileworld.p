/* ---- Copyright University of Nottingham 2002. All rights reserved. ------
 > File:            /home/bsl/pop/sim_tileworld/sim_tileworld.p
 > Purpose:         A simple Tileworld in SIM_AGENT.
 > Author:          Michael Lees, Nov 19 2002
 > Documentation:   
 > Related Files:   
 > RCS Info:        $Id: sim_tileworld.p,v 1.5 2002/11/21 10:42:09 bsl Exp $
 */


;;;Change memory limit and load libraries
9000000->popmemlim;
uses objectclass
uses newkit
uses poprulebase
uses sim_agent
uses sim_picagent
uses rc_linepic
uses rc_exceed
uses rc_mousepic
uses rc_window_object
uses rc_context
uses sim_geom

;;; Run all rules possible. Need this to enable agent to sense multiple
;;; objects of same type.
true->prb_allrules;

;;; This causes rules to run straight away
false->prb_sortrules;

;;; Switch off debugging.
false -> pop_debugging;
false -> popgctrace;


global vars 
     ;;; The environment
     TW,                
     ;;; Default configuration
     tw_xsize = 20,
     tw_ysize = 20,
     tw_ntiles = 1,
     tw_nholes = 1,
     tw_nobstacles = 5,
     tw_nagents = 2,
     ;;; Suppress drawing the tileworld
     tw_drawgraphics = true,
     ;;; For taking snapshots
     team_dump_images = false,
     team_dump_file = "pic",
     ;;; If true, pause at the end of each cycle for input
     cycle_pause = false;

vars environment_rulesystem, 
     agent_rulesystem;


;;; THE ENVIRONMENT CLASS
;;;-----------------------
define :class tileworld is sim_agent;

     ;;; Dimensions of world grid (how many squares)
     slot width   = 20;
     slot height  = 20;

     slot objects = []; ;;; all the objects in the environment
     slot startup_objects = []; ;;; all the objects in the environment

     slot sim_rulesystem = environment_rulesystem;  ;;;See below
     slot sim_sensors    = [];                      ;;;doesn't sense


     ;;; Number of each object - this is used when assiging a label to object
     slot num_tiles  = 0; ;;; Number of tiles in the environment
     slot num_holes  = 0; ;;; Number of holes in the environment
     slot num_agents = 0; ;;; Number of agents in the environemnt

     ;;; The adjustable knobs probabilities of new
     ;;; object occuring in a cycle
     slot new_hole_prob     = 0.5;
     slot new_tile_prob     = 0.5;
     slot new_obstacle_prob = 0.5;

     slot random_life = false; ;;; if true life of a hole is random(hole_life)

     ;;; Life time (in cycles) of each object
     slot hole_life     = 50;  ;;; num of cycles
     slot tile_life     = 50;
     slot obstacle_life = 50;

     ;;; Debug printing 0=none, 1=some, 2=all
     slot printlevel=0;

     ;;;Drawing stuff
     slot windows    = [];
     slot window     = false;
     slot background = 'white';
     slot colour      = 'black';
     slot textcolour  = 'black';
     slot xwindowX   = 30; ;;;Top left corner of window
     slot xwindowY   = 10;

     slot tile_colour  = 'green';
     slot hole_colour  = 'brown';
     slot agent_colour = 'blue';

     ;;; The following two slots are the dimensions of the xwindow
     ;;; automatically calculated later in terms of other measurements
     ;;; The size of the window is determined by the number of squares
     ;;; and the size of a single square
     ;;; ie. window_width = (Num_squares_x * Square_width) + plus_some_extra
     slot xwidth = 0; ;;; = 60+gridwidth*width
     slot xheight = 0; ;;; = 100+gridheight*height

     slot drawgraphics = true;
     slot xwin       = false;

     ;;; Name of window
     slot role       = '---- Tileworld ----';
     ;;; Size of a single square
     slot gridheight = 20;   ;;; The size of grid squares
     slot gridwidth  = 20;
enddefine;

;;; Make a tileworld of the specified size.
define make_tileworld(twidth, theight /* &optional draw_tileworld */) -> t;
    lvars twidth, theight, draw_tileworld = true, t;
    ARGS twidth, theight, &OPTIONAL draw_tileworld:isboolean;

    instance tileworld;
	width = twidth;
	height = theight;
	drawgraphics = draw_tileworld;
	endinstance -> t;
    enddefine;


;;;THE METHODS
;;;-------------
define :method display(tw:tileworld);
    if drawgraphics(tw) then
[about to draw the tileworld ^(drawgraphics(tw))] =>
    	lvars x, y, xtmp, n,agt,scalex,scaley,xorigin,yorigin,frame;
     	unless xwin(tw) then
	    ;;; create new ones
	    false -> rc_window;
	    
	    ;;; Setup the width and height of the xwindow
	    ;;; in relation to the grid dimensions
	    ;;; specified in class slots
	    gridwidth(tw)*(width(tw)+2)->xwidth(tw);
	    gridheight(tw)*(height(tw)+2)->xheight(tw);
	    
	    ;;; Calculate the coordinate frame to sim coord movements
	    ;;; map to correct display coord movements
	    ;;; This coord frame is then stored
	    
	    ;;; How much 1 sim_x/sim_y movement effects display x/y
	    ;;; In this case 1 sim_x/sim_y movement should correspond
	    ;;; to moving the object the width/height of one square(gridwidth/girdheight)
	    gridwidth(tw)->scalex;
	    gridheight(tw)->scaley;
	    -(xwidth(tw)-(gridwidth(tw)*3))/2->xorigin; ;;;set the origin
	    -(xheight(tw)-(gridheight(tw)*3))/2->yorigin;
	    {^xorigin ^yorigin ^scalex ^scaley} -> frame; ;;;store in temp variable
	    xwidth(tw)=>
	    xheight(tw)=>
	    xwindowX(tw)=>
	    xwindowY(tw)=>
	    role(tw)=>
	    
	    rc_new_window_object(xwindowX(tw), xwindowY(tw),
	     		      	 xwidth(tw), xheight(tw),
	     		      	 true, role(tw), newsim_picagent_window) -> xtmp;
	    xwidth(tw)=>
	    xheight(tw)=>
	    ;;; Store a list containing the window
	    ;;; which is used when calling newtile_agent
	    ;;; could have multiple windows (views)
	    [^xtmp]->windows(tw);
	    ;;; Store the actual window aswell(used elsewhere)
	    xtmp->window(tw);
	    xtmp -> xwin(tw);
	    
	    ;;; Store frame system into window just created
	    frame->sim_picframe(window(tw));
	    
	    background(tw) -> rc_background(rc_window);
	    XpwSetColor(rc_window, colour(tw))->n;
     	else
	    rc_clear_window();
     	    endunless;
     	
     	;;; Draw the grid...
     	
     	0 -> n;
     	fast_for y from 1 to height(tw) do
	    fast_for x from 1 to width(tw) do
	     	lvars Xx, Xy;
	     	n+1 -> n;
	     	gridwidth(tw)+((x-1)*gridwidth(tw))-(xwidth(tw)/2) -> Xx;
	     	(gridheight(tw)*2)+((y-1)*gridheight(tw))-(xheight(tw)/2) -> Xy;
	     	rc_jumpto(Xx, Xy);
	     	rc_draw_rectangle(gridwidth(tw),gridheight(tw));
	     	XpwSetColor(rc_window, textcolour(tw)) -> n;
	     	XpwSetColor(rc_window, colour(tw))->n;
         	endfast_for;
     	    endfast_for;
 	endif;
    enddefine;
	 
define :method print_instance(o:tileworld);
    printf('<tileworld: %p>', [% o.sim_name %]);
    enddefine;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Mixin - a tile object   ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	 
;;; NOTE the agent isn't a tile object The mixin is in fact used to
;;; distinguish the objects from the agent
define :mixin tile_obj;
    ;;; All tile objects (holes,tiles,obstacles) have a lifetime
    slot life == 0;
enddefine;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Mixin - a tile object   ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	 
;;; NOTE this is used by rc_move_to, so the correct method is called
;;; and the appropriate objects are constrained within the environment
;;; ie, tiles and holes 
define :mixin tw_constrained;
enddefine;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; TARGET class - used to indicate agent target location ;;;
;;;                it appears as a little cirlce          ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define :class target is sim_movable;
    slot sim_rulesystem =[];
    slot sim_sensors =[];
enddefine;

define newtarget(x,y,w,colour,env)->atarget;
    lvars qwidth=abs(w/2),
          qw=abs(qwidth/2);
    instance target;
        sim_x=x;
        sim_y=y;
        rc_pic_lines =[
        		[COLOUR ^colour WIDTH ^qwidth CIRCLE {0 1 1}]
        		[COLOUR 'red' WIDTH ^qw CIRCLE {0 1 1}]
        		];
    endinstance->atarget;
    rc_add_containers(atarget, windows(env));
enddefine;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;    The tileworld AGENT   ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; THE CLASS
;;;-------------
define :class tile_agent is sim_movable_agent;

    ;;; Heading, Integer defined below
    ;;; No heading-0,
    ;;; up - 1,
    ;;; left - 4,
    ;;; right - 2,
    ;;; down - 3
    slot heading   == 0;

    slot printlevel = 0;

    ;;; Number of tiles the agent is currently carrying (not used at present)
    slot n_tiles=0;

    ;;; The target object for this agent (see target class)
    slot target=false;

    ;;; Range in terms of grid squares
    slot sensor_range   == 5;

    ;;; Override default sensor method  and set range (number)
    slot sim_sensors == [{sim_sense_agent 5}];

    ;;;The rulesystem of the agent
    slot sim_rulesystem = agent_rulesystem;

    ;;; Drawing stuff
    slot rc_pic_lines ==[COLOUR 'black' WIDTH 3 RSQUARE {-5 5 10}];
    slot rc_mouse_limit == 10;
enddefine;

define :method print_instance(o:tile_agent);
    printf('<tile_agent: %p>', [% o.sim_name %]);
    enddefine;

;;; Used to make a new instance of a tile_agent
define newtile_agent(x, y, w, colour, label, env) -> agt;
    instance tile_agent;
        sim_x = x;
        sim_y = y;
    endinstance->agt;
    
    ;;; Add this object to the list of objects in the environment
    
    ;;;    objects(env) nc_<> [^agt] -> objects(env);

    ;;; Increase numnber of agents by 1
    1 + num_agents(env) -> num_agents(env);

    ;;; Draw the graphics if required.
    if drawgraphics(env) then
    	lvars qwidth = w* (-0.30);
        
	[{^qwidth ^qwidth ^label}] -> rc_pic_strings(agt);
        [COLOUR ^colour WIDTH ^w SQUARE {0 0 1}] -> rc_pic_lines(agt);
    	rc_add_containers(agt, windows(env));
    	;;; Create a target object for this agent (make it's start location
    	;;; off screen (-2,-2))
    	newtarget(-2,-2,w,colour,env)->target(agt);
	endif;
enddefine;

define make_tile_agent(colour, label, env) -> agent;
    lvars x, y;
    valid_coords(env) -> (x, y);
    newtile_agent(x, y, gridwidth(env), colour, label, env) -> agent;    
    enddefine;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;    The tileworld TILE    ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; THE CLASS
;;;-------------
define :class tile is tw_constrained,sim_movable, tile_obj;
    slot tilenumber     == 0;
    slot sim_rulesystem =[];   ;;; No rules or sensors
    slot sim_sensors =[];
enddefine;

define :method print_instance(o:tile);
    printf('<tile: %p>', [% o.sim_name %]);
    enddefine;

define newtile(x, y, w, colour, label, env) -> atile;
    instance tile;
        sim_x = x;
        sim_y = y;
        life = tile_life(env);
    endinstance->atile;
    ;;;     objects(env) nc_<> [^atile] -> objects(env);

    1 + num_tiles(env) -> num_tiles(env);

    ;;; Draw the graphics if required.
    if drawgraphics(env) then
    	lvars qwidth = w * (-0.30),
              fontsize = round(w*0.5) - 2,
              fontstring = '-*-helvetica-medium-r-*-*-'><fontsize><'-*-*-*-*-*-*-*';
	[FONT ^fontstring {^qwidth ^qwidth ^label}] -> rc_pic_strings(atile);
	[COLOUR ^colour WIDTH ^w SQUARE {0 0 1}] -> rc_pic_lines(atile);
	rc_add_containers(atile, windows(env));
	endif;
enddefine;

;;; Make a tile with the specified <colour> and <label> at a random position
;;; in <env> that isn't already occupied.
define make_tile(colour, label, env) -> tile;
    lvars x, y;
    valid_coords(env)->(x,y);
    newtile(x, y, gridwidth(env), colour, label, env) -> tile;
    enddefine;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;    The tileworld HOLE    ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; THE CLASS
;;;-------------
define :class hole is tw_constrained,sim_movable, tile_obj;

    slot holenumber    == 0;
    ;;; The following three slots will be used in a later implementation
    slot depth         == 1;
    slot tiles         == [];
    slot full          == false;
    slot sim_rulesystem = [];
    slot sim_sensors    = [];

enddefine;

define :method print_instance(o:hole);
    printf('<hole: %p>', [% o.sim_name %]);
    enddefine;

define newhole(x,y,w,colour,label,env)->ahole;

    instance hole;
        sim_x = x;
        sim_y = y;
        life = hole_life(env);
    endinstance -> ahole;

    ;;;    objects(env) nc_<> [^ahole] -> objects(env);

    1 + num_holes(env) -> num_holes(env);

    ;;; Draw the graphics if required.
    if drawgraphics(env) then
    	lvars qwidth = w * (-0.30),
              fontsize = round(w * 0.5) - 2,
              fontstring = '-*-helvetica-medium-r-*-*-'><fontsize><'-*-*-*-*-*-*-*';

	[FONT ^fontstring {^qwidth ^qwidth ^label}] -> rc_pic_strings(ahole);
	[COLOUR ^colour WIDTH ^w SQUARE {0 0 1}] -> rc_pic_lines(ahole);
	rc_add_containers(ahole, windows(env));
	endif;

enddefine;

;;; Make a hole with the specified <colour> and <label> a random position in
;;; <env> that isn't already occupied.
define make_hole(colour, label, env) -> tile;
    lvars x, y;
    valid_coords(env)->(x,y);
    newhole(x, y, gridwidth(env), colour, label, env) -> tile;
    enddefine;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  The tileworld OBSTACLE  ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; THE CLASS
;;;-------------
define :class obstacle is sim_movable, tile_obj;
    slot sim_rulesystem =[];
    slot sim_sensors =[] ;
enddefine;

define :method print_instance(o:obstacle);
    printf('<obstacle: %p>', [% o.sim_name %]);
    enddefine;

define newobstacle(x,y,w,colour,env)->obst;
    instance obstacle;
        sim_x=x;
        sim_y=y;
        life=obstacle_life(env);
    endinstance -> obst;

    ;;;objects(env) nc_<> [^obst] -> objects(env);

    ;;; Draw the graphics if required.
    if drawgraphics(env) then
    	lvars qwidth = w * (-0.30);

	[{^qwidth ^qwidth ''}] -> rc_pic_strings(obst);
	[COLOUR ^colour WIDTH ^w SQUARE {0 0 1}] -> rc_pic_lines(obst);
	rc_add_containers(obst, windows(env));
	endif;
enddefine;

;;; Make an obstacle with the specified <colour> at a random position in
;;; <env> that isn't already occupied.
define make_obstacle(colour, env) -> obstacle;
    lvars x, y;
    valid_coords(env)->(x,y);
    newobstacle(x, y, gridwidth(env), colour, env) -> obstacle;
    enddefine;


define make_border_obstacle(x, y, env) -> obstacle;
    newobstacle(x, y, gridwidth(env), 'black', env) -> obstacle;
    -100 -> life(obstacle);
    enddefine;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Redefining standard methods
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; This is the same as rc_move_by except it moves the object
;;; in terms of simulation coordinates rather than rc (graphic)
;;; coordinates (Not defined as standard!?)
define :method sim_move_by(obj:sim_multiwin_mobile,x,y,drawmode);
    if drawgraphics(TW) then
     	lvars x0,y0,xscale,yscale,Num,movex,movey,frame;
        ;;; Get details of the picture frame
      	sim_picframe(rc_current_window_object)->frame;
      	destvector(frame)->(x0,y0,xscale,yscale,Num);
	
      	;;;scale up the movement to pic_x and pic_y values
      	xscale*x->movex;yscale*y->movey;
	
      	;;; Move the object by calculated values
  	;;; This updates sim_x and sim_y to new coords
      	rc_move_by(obj, movex, movey, drawmode);
    else
	x+sim_x(obj) -> sim_x(obj);
	y+sim_y(obj) -> sim_y(obj);
	endif;
    enddefine;

define :method sim_move_to(obj:sim_multiwin_mobile,x,y,drawmode);
    if drawgraphics(TW) then
     	lvars x_move_by,y_move_by;
	x-sim_x(obj)->x_move_by;
	y-sim_y(obj)->y_move_by;
	;;; this updates sim_x and sim_y to new coords
      	sim_move_by(obj, x_move_by, y_move_by, drawmode);  
    else
	x -> sim_x(obj);
	y -> sim_y(obj);
	endif;
    enddefine;

;;;;;; This is the same as rc_move_by except it moves the object
;;;;;; in terms of simulation coordinates rather than rc (graphic)
;;;;;; coordinates (Not defined as standard!?)
;;;define :method sim_move_by(obj:sim_multiwin_mobile, x, y, drawmode);
;;;    if drawgraphics(TW) then
;;;    	lvars x0,y0,xscale,yscale,Num,movex,movey,frame;
;;;        ;;; Get details of the picture frame
;;;     	sim_picframe(rc_current_window_object)->frame;
;;;     	destvector(frame)->(x0,y0,xscale,yscale,Num);
	
;;;     	;;;scale up the movement to pic_x and pic_y values
;;;     	xscale*x->movex;yscale*y->movey;
	
;;;     	;;; Move the object by calculated values
;;;     	rc_move_by(obj, movex, movey, drawmode);  ;;; this updates sim_x and sim_y to new coords
;;;     	else
;;;	    x -> sim_x(obj);
;;;	    y -> sim_y(obj);
;;;	    endif;
;;; 	enddefine;

define  :method rc_move_to(pic:tile_agent, x, y, mode);
    	;;; This causes the agent to snap to the grid when moved with the mouse
     	lvars scrx,scry,scrwidth,scrheight,xorigin,yorigin,xscale,yscale,etc;
     	lvars rightboundary,leftboundary,topboundary,bottomboundary,frame;
      	;;; Get the conversion vector of current window
     	sim_picframe(rc_current_window_object)->frame;
	
     	;;; First check for boundaries stops
     	;;; the agent being dragged off the 'board'
     	destvector(rc_screen_frame(rc_current_window_object))->(scrx,scry,scrwidth,scrheight,etc);
     	destvector(frame)->(xorigin,yorigin,xscale,yscale,etc);
     	(scrwidth-(3*xscale))/2->rightboundary; ;;;righthand side of tileworld
     	-rightboundary->leftboundary;           ;;;lefthand side of TW
     	(scrheight-(3*yscale))/2->topboundary;  ;;;Top of TW
     	-topboundary->bottomboundary;           ;;;bottom of TW
     	if(x>rightboundary) then rightboundary->x endif;
     	if(x<leftboundary) then leftboundary->x endif;
     	if(y>topboundary) then topboundary->y endif;
     	if(y<bottomboundary) then bottomboundary->y endif;
	
     	lvars simx,simy;
     	;;; Get the conversion vector of current window
     	;;;sim_picframe(rc_current_window_object)->frame;
	
     	;;; Translate the picture coordinates to simulation coords
     	sim_transxy_from_pic(x,y,frame)->(simx,simy);
	
     	;;; Translate the simulations coords back to pic coords
     	;;; after they have been rounded to the nearest integer
     	sim_transxy_to_pic(round(simx),round(simy),frame)->(x,y);
	
     	;;; Call the rc_move_to as normal
     	call_next_method(pic, x, y, mode);
    enddefine;

define :method rc_move_to(pic:tw_constrained, x, y, mode);
     	;;; This causes the agent to snap to the grid when moved with the mouse
     	lvars scrx,scry,scrwidth,scrheight,xorigin,yorigin,xscale,yscale,etc;
     	lvars rightboundary,leftboundary,topboundary,bottomboundary,frame;
	
     	sim_picframe(rc_current_window_object)->frame;
     	;;; First check for boundaries stops
     	;;; the agent being dragged off the 'board'
     	destvector(rc_screen_frame(rc_current_window_object))->(scrx,scry,scrwidth,scrheight,etc);
     	destvector(frame)->(xorigin,yorigin,xscale,yscale,etc);
     	(scrwidth-(3*xscale))/2->rightboundary; ;;;righthand side of tileworld
     	-rightboundary->leftboundary;           ;;;lefthand side of TW
     	(scrheight-(3*yscale))/2->topboundary;  ;;;Top of TW
     	-topboundary->bottomboundary;           ;;;bottom of TW
     	if(x>rightboundary) then rightboundary->x endif;
     	if(x<leftboundary) then leftboundary->x endif;
     	if(y>topboundary) then topboundary->y endif;
     	if(y<bottomboundary) then bottomboundary->y endif;
	
     	lvars simx,simy;
     	;;; Get the conversion vector of current window
	
     	;;; Translate the picture coordinates to simulation coords
     	sim_transxy_from_pic(x,y,frame)->(simx,simy);
	
     	;;; Translate the simulations coords back to pic coords
     	;;; after they have been rounded to the nearest integer
     	sim_transxy_to_pic(round(simx),round(simy),frame)->(x,y);
	
     	;;; Call the rc_move_to as normal
     	call_next_method(pic, x, y, mode);
    enddefine;

define :method rc_move_to(pic:obstacle, x, y, mode);
     	;;; This causes the agent to snap to the grid when moved with the mouse
     	lvars frame, simx,simy;
     	sim_picframe(rc_current_window_object)->frame;
     	;;; Get the conversion vector of current window
     	;;; Translate the picture coordinates to simulation coords
     	sim_transxy_from_pic(x,y,frame)->(simx,simy);
	
     	;;; Translate the simulations coords back to pic coords
     	;;; after they have been rounded to the nearest integer
     	sim_transxy_to_pic(round(simx),round(simy),frame)->(x,y);
	
     	;;; Call the rc_move_to as normal
     	call_next_method(pic, x, y, mode);
    enddefine;

;;; Redefined in terms of squares/grids - the distance between two agents in
;;; tileworld is the maximum of x distance and y distance.  Now use huffman
;;; distance xdiff+ydiff
define :method sim_distance(a1:tile_agent, a2:sim_movable) -> dist;
    ;;; Compute distance between two agents.
    lvars dist = 0;
    ;;; This is in terms of number of squares
    ;;; not the actual distance
    lvars xdiff, ydiff;
    abs(sim_x(a1)-sim_x(a2))->xdiff;
    abs(sim_y(a1)-sim_y(a2))->ydiff;
    (xdiff+ydiff)->dist;
    ;;;max(xdiff,ydiff)->dist;
enddefine;

;;; This is redifined to pass the actual sim_object back rather than just
;;; its name and dist
define :method sim_sense_agent(a1:sim_object, a2:sim_object, dist);
    ;;; Default sensor for detecting other agents
    ;;; return information about name of other object.
    unless (a1 == a2 or istileworld(a2)) then    ;;; don't detect oneself
        [new_sense_data ^a2 ^dist]
    endunless
enddefine;



;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Other functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; When a new object is created it needs to be created
;;; in a blank space? This goes through the objects
;;; in the object list checking to see if there is an
;;; object at x,y.
define member_xy(x,y,list) -> answer;
    lvars item;
    false->answer;
    for item in list do
    	unless istileworld(item) then
    	    if((sim_x(item) =x)and(sim_y(item)=y)) then
            	true->answer;
    	    	endif;
    	    endunless;
    	endfor
    enddefine;

;;; Takes an environment class and returns valid random x and y coordinates
define random_coords(tw) -> (x, y);
  random(width(tw) - 1) -> x;
  random(height(tw) - 1) -> y;
enddefine;


;;;Debugging code

;;;Print all objects in environment
define print_all(env);
    lvars item1;
    for item1 in sim_objects do
        item1 =>
    endfor
enddefine;

;;;Print all tiles in environment
define print_tiles(env);
    lvars item2;
    for item2 in sim_objects do
        if(istile(item2))then
            item2 =>
        endif;
    endfor
enddefine;

;;;Print all holes in environment
define print_holes(env);
    lvars item3;
    for item3 in sim_objects do
        if(ishole(item3))then
            item3 =>
        endif;
    endfor
enddefine;

;;;Print all obstacles in environment
define print_obstacles(env);
    lvars item4;
    for item4 in sim_objects do
        if(isobstacle(item4))then
            item4=>
        endif;
    endfor
enddefine;

define print_agents(list);
    lvars item,count=1;
    for item in list do
        if(istile_agent(item))then
            count><': '><item=>
            count+1->count;
        endif;
	
    endfor
enddefine;

define print_agent_data(num,list);
   lvars item,count=0;
    for item in list do
        if(istile_agent(item))then
            count+1->count;
        endif;
        if(count=num) then
            ['Data in' ^(sim_name(item)) ]==>
            prb_print_table( (sim_data(item)) );
        endif;
    endfor
enddefine;


;;; This is used for debugging it prints
;;; out the life of all the objects in the list
define print_life(env);
    lvars item,count=1;
    for item in sim_objects do
	if(istile_obj(item)) then
    	    'Item'><count=>
    	    life(item)=>
    	    count+1->count;
	endif
    endfor
enddefine;

;;;------------


;;; This is called by the environment rules every cycle, it creates the new
;;; objects in the environment. The chance of a new object being created is
;;; related to its probability defined in the environment class
define create_new_objects(env);
    lvars tile_rnd=random(100),
	  hole_rnd=random(100),
	  obst_rnd=random(100),
	  tile_x=random(width(env)-1),
	  tile_y=random(height(env)-1),
	  hole_x=random(width(env)-1),
	  hole_y=random(height(env)-1),
	  obst_x=random(width(env)-1),
	  obst_y=random(height(env)-1),
	  atile,ahole,obst;

    ;;; If probability satisfied create a new tile pick a random location
    ;;; for tile If this location is taken keep picking another until a
    ;;; empty location is found.

    ;;;  --- Tile ---
    if (tile_rnd<(new_tile_prob(env)*100)) then
	while(member_xy(tile_x,tile_y,sim_objects)) do
	    random_coords(env)->(tile_x,tile_y);
	endwhile;
	;;; Create the new tile at new location
	newtile(tile_x,tile_y,gridwidth(env),tile_colour(env),'T'><(num_tiles(env)+1),env)->atile;
	 atile::sim_object_add_list->sim_object_add_list;
    endif;

    ;;;  --- Hole ---
    if (hole_rnd<(new_hole_prob(env)*100)) then
	while(member_xy(hole_x,hole_y,sim_objects)) do
	    random_coords(env)->(hole_x,hole_y);
	endwhile;
	;;; Create the new hole at new location
	newhole(hole_x,hole_y,gridwidth(env),'brown','H'><(num_holes(env)+1),env)->ahole;
	 ahole::sim_object_add_list->sim_object_add_list;
	
    endif;

    ;;; --- Obstacle ---
    if (obst_rnd<(new_obstacle_prob(env)*100)) then
	while(member_xy(obst_x,obst_y,sim_objects)) do
	    random_coords(env)->(obst_x,obst_y);
	endwhile;
	;;; Create the new obstacle at new location
	newobstacle(obst_x,obst_y,gridwidth(env),colour(env),env)->obst;
	obst::sim_object_add_list->sim_object_add_list;
    endif;
enddefine;

;;; This returns true if the two objects are at the same location (on the
;;; same square)
define same_location(o1,o2)->answer;
    if((sim_x(o1)=sim_x(o2)) and (sim_y(o1)=sim_y(o2))) then
	true->answer;
    else
	false->answer;
    endif;
enddefine;

;;; This method moves the agent in the specified direction It used as a 'do'
;;; action in the RULE move
define :method move_to(agt:tile_agent,dir);
    lvars dx=0,dy=0;
    if dir=1 then      ;;; Going Up
       	1->dy;
    elseif dir=2 then  ;;; Going Right
       	1->dx;
    elseif dir=3 then  ;;; Going Down
       	(-1)->dy;
    elseif dir=4 then  ;;; Going Left
       	(-1)->dx;
    endif;
    sim_move_by(agt,dx,dy,true);
enddefine;

;;; Returns true if the object is in range of agents sensors.
define in_range(agent,obj)->answer;
    lvars dist=sim_distance(agent,obj),
      	  (proc,range,num)=destvector(hd(sim_sensors(agent)));

    (dist<=range)->answer;
enddefine;


;;; Same as move_to but moves the tile too.
define :method push_to(agent:tile_agent,tile,dir);
    lvars tile_x=0,
      	  tile_y=0;
    move_to(agent,dir);
    if(dir=1) then
    	1->tile_y;
    elseif(dir=2) then
    	1->tile_x;
    elseif(dir=3) then
    	(-1)->tile_y;
    elseif(dir=4) then
    	(-1)->tile_x;
    endif;
    sim_move_by(tile,tile_x,tile_y,true);
enddefine;

;;; This removes the object from the environment It's removed from the
;;; scheduler list 'objects(env)' and from the graphical trace (window)
define :method remove_object(agt: tile_agent,object, env);
    ;;; remove object from list of agents
    ;;; ncdelete(object, objects(env)) -> objects(env);
    ;;; ncdelete(object, sim_objects) -> sim_objects;

;;; replaces code above to delete objects in correct manner 22/11/02
    object :: sim_object_delete_list -> sim_object_delete_list;

    ;;; remove object from current window
    if (drawgraphics(env)) then
    	rc_remove_container(object, window(env));
	endif;
enddefine;

;;; Non do-action version for use by remove_dead_items which is a do action
define remove_object_direct(object, env);
;;; replaces code above to delete objects in correct manner 22/11/02
    object :: sim_object_delete_list -> sim_object_delete_list;

    ;;; remove object from current window
    if (drawgraphics(env)) then
    	rc_remove_container(object, window(env));
	endif;
enddefine;

;;; This is called every cycle by the environment
;;; it decreases the life of all the tile objects (not agents)
;;; in the objects list by 1.
define :method decrease_life(env: tileworld);
    lvars item;

    for item in sim_objects do
    	if(istile_obj(item)) then
    	    life(item) - 1 -> life(item);
    	endif
    endfor
enddefine;

;;; This is called every cycle by the environment it removes the objects
;;; whose life has reached zero
define :method remove_dead_items(env: tileworld);
    lvars item;
    for item in sim_objects do
    	;;; If it's a tile object (Not an agent!)  and it's life has run out
    	;;; remove it
    	if(istile_obj(item) and life(item)>(-100) and life(item)<1) then
            remove_object_direct(item,env);
    	endif
    endfor
enddefine;

;;; The next two functions are used by the agent to calculate
;;; which square to move to next
define distance(x1,y1,x2,y2)->dist;
    lvars dist = 0;
    ;;; This is in terms of number of tiles not the actual distance
    lvars xdiff, ydiff;
    abs(x1-x2)->xdiff;
    abs(y1-y2)->ydiff;
    (xdiff+ydiff)->dist;
enddefine;

define calc_square_value(agent,sx,sy,target_x,target_y)->answer;
    lvars (my_x,my_y)=sim_coords(agent);
    if (distance(sx,sy,target_x,target_y) <
	distance(my_x,my_y,target_x,target_y)) then
    	0->answer;
    else
    	20->answer;
    endif;
enddefine;

;;; returns true if the coords are next to each other (not diagonal)
define next_to(x1,y1,x2,y2)->answer;
    ((abs(x1-x2)=1 and abs(y1-y2)=0)or(abs(x1-x2)=0 and abs(y1-y2)=1))->answer;
enddefine;

;;; returns a direction to go from (x1,y1) to (x2,y2)
define a_direction(x1,y1,x2,y2)->answer;
    if(x1=x2 and y1=y2-1) then
        ;;; go up
    	1->answer;
    elseif(x1=x2 and y1=y2+1) then
        ;;; go down
    	3->answer;
    elseif(x1=x2-1 and y1=y2) then
        ;;; go right
    	2->answer;
    elseif(x1=x2+1 and y1=y2) then
        ;;; go left
    	4->answer;
    endif
enddefine;


;;; The following procedures are currently not used.
define picked_up_tile(agent, tile, env);
    lvars (a, b, c, z) = destvector(hd(rc_pic_strings(agent)));

    ;;; remove graphical rep only, still in objects list
    rc_remove_container(tile,window(env)); 
    n_tiles(agent) + 1 -> n_tiles(agent);
    rc_undraw_linepic(agent);
    [{^a ^b ^('' >< n_tiles(agent))}] -> rc_pic_strings(agent);
    rc_draw_linepic(agent);
enddefine;

define drop_tile(agent, tile, env);
    lvars (a, b, c, z) = destvector(hd(rc_pic_strings(agent)));

    n_tiles(agent) - 1 -> n_tiles(agent);
    rc_undraw_linepic(agent);
;;;    rc_pic_strings(agent)=>
    [{^a ^b ^('' >< n_tiles(agent))}] -> rc_pic_strings(agent);
    rc_draw_linepic(agent);
enddefine;

define valid_coords(env) -> (valid_x, valid_y);
    random_coords(env)->(valid_x, valid_y);
    while(member_xy(valid_x, valid_y, startup_objects(env))) do
	random_coords(env) -> (valid_x, valid_y);
    endwhile;
enddefine;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;       Rulesets for agent
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define :ruleset agent_repercept_rules;
    ;;; Rules for removing seen,have,target and nearest facts when the
    ;;; objects have died/vanished. Or generally when the agent doesn't
    ;;; sense the object anymore
    
    ;;; NOTE:
    ;;; These rules must be run before percept rules otherwise all will be
    ;;; runable.  That is because the percept rules remove the
    ;;; new_sense_data facts. These rules are executed at begining of cycle

    ;;; This is when we have a tile and it vanishes ie. we can't sense it
    ;;; anymore
    [DLOCAL
     [prb_walk = false]
     [prb_chatty = false]
     [prb_show_conditions = false]
     ];

    ;;; Had a tile but can't see it anymore even tho it's in range
    RULE not_have_tile
    	[NOT stopped]
    	[have_tile ?tile]
    	[NOT new_sense_data ?tile ==]
    	[WHERE in_range(sim_myself,tile)]
    	==>
    	[NOT have_tile ?tile]

    ;;; This is when a target tile vanishes
    RULE not_target_tile
    	[NOT stopped]
    	[target_tile ?tile]
    	[NOT new_sense_data ?tile ==]
    	[WHERE in_range(sim_myself,tile)]
    	==>
    	[NOT target_location move ==]
    	[NOT target_tile ?tile]

    ;;; Had seen a tile but not anymore
    RULE not_seen_tile
    	[NOT stopped]
    	[seen_tile ?tile ?dist ?time]
    	[NOT new_sense_data ?tile ==]
    	[WHERE in_range(sim_myself,tile)]
    	==>
    	[NOT seen_tile ?tile ?dist ?time]

    ;;; The tile nearest to the agent has gone
    RULE not_near_tile
    	[NOT stopped]
    	[nearest_tile ?tile ==]
    	[NOT new_sense_data ?tile ==]
    	[WHERE in_range(sim_myself,tile)]
    	==>
    	[NOT nearest_tile ?tile ==]

    ;;; updates the distance of nearest tile
    RULE update_near_tile
    	[NOT stopped]
    	[nearest_tile ?tile ==]
    	[new_sense_data ?tile ?dist==]
    	==>
    	[LVARS [time=sim_cycle_number]]
    	[NOT nearest_tile ?tile ==]
    	[nearest_tile ?tile ?dist ?time]

    ;;; The agents target hole has vanished
    RULE not_target_hole
    	[NOT stopped]
    	[target_hole ?hole]
    	[NOT new_sense_data ?hole ==]
    	[WHERE in_range(sim_myself,hole)]
    	==>
    	[NOT target_location push ==]
    	[NOT target_hole ?hole]
    
    ;;; The hole nearest the agent has gone
    RULE not_near_hole
    	[NOT stopped]
    	[nearest_hole ?hole ==]
    	[NOT new_sense_data ?hole ==]
    	[WHERE in_range(sim_myself,hole)]
    	==>
    	[NOT nearest_hole ?hole ==]
	
    ;;; updates distance of nearest hole
    RULE update_near_hole
    	[NOT stopped]
    	[nearest_hole ?hole ==]
    	[new_sense_data ?hole ?dist ==]
    	==>
    	[LVARS [time=sim_cycle_number]]
    	[NOT nearest_hole ?hole ==]
    	[nearest_hole ?hole ?dist ?time]

    ;;; A hole that the agent had seen has now gone
    RULE not_seen_hole
    	[NOT stopped]
    	[seen_hole ?hole ?dist ?time]
    	[NOT new_sense_data ?hole ==]
    	[WHERE in_range(sim_myself,hole)]
    	==>
    	[NOT seen_hole ?hole ?dist ?time]

    ;;; An obstacle has gone
    RULE not_seen_obst
    	[NOT stopped]
    	[seen_obstacle ?obst ==]
    	[NOT new_sense_data ?obst ==]
    	[WHERE in_range(sim_myself,obst)]
    	==>
    	[NOT seen_obstacle ?obst ==]

    ;;; Decrease the square value every cycle
    ;;; so the agent is more likely to go back
    RULE decrease_square_value
    	[NOT stopped]
    	[square_value ?x ?y ?score ?num]
    	[LVARS[newscore=score-1]]
    	==>
    	[NOT square_value ?x ?y ?score ?num]
    	[square_value ?x ?y ?newscore ?num]

    ;;; move_square score must be updated as it may have been decreased due
    ;;; to last rule
    RULE update_move_square
    	[NOT stopped]
    	[square_value ?x ?y ?score]
    	[move_square ?x ?y ==]
    	==>
    	[NOT move_square ==]
    	[move_square ?x ?y ?score]
	
    ;;; Forget square values after 2 cycles
    RULE forget_old
    	[NOT stopped]
    	[square_value ?x ?y ?score ?num]
    	[LVARS [newnum = sim_cycle_number]]
    	[WHERE ((newnum-2)=num)]
    	==>
    	[NOT square_value ?x ?y ==]

    ;;; If tile is not on same square as agent then
    ;;; agent no longer has it.
    RULE lost_tile
    	[NOT stopped]
    	[have_tile ?tile]
    	[WHERE (same_location(sim_myself,tile)=false)]
    	==>
    	[NOT have_tile ?tile]

enddefine;

define :ruleset agent_tile_percept_rules;
    ;;; Perception rules for agent

    [DLOCAL
     [prb_walk = false]
     [prb_chatty = false]
     [prb_show_conditions = false]
     ];

    ;;; Tile stuff
    ;;;------------

    RULE see_a_tile ;;; add seen_tile fact to database
	[NOT stopped]
	[new_sense_data ?tile ?dist]
	[WHERE istile(tile)]
	==>
	[NOT new_sense_data ?tile ?dist]
	[NOT seen_tile ?tile ==]
	[LVARS [time=sim_cycle_number]]
	[seen_tile ?tile ?dist ?time]

    RULE see_first_nearest_tile  ;;;First tile seen
	[NOT stopped]
	[seen_tile ?thing ?dist ==]
	[WHERE istile(thing)]
	[NOT nearest_tile ==] ;;; Don't already have a nearest tile
	==>
	[LVARS [time=sim_cycle_number]] ;;; used as a timestamp
	[nearest_tile ?thing ?dist ?time]

    RULE see_nearer_tile  ;;; seen a tile nearer than the current nearest
	[NOT stopped]
	[seen_tile ?thing ?dist ==]
	[WHERE istile(thing)]
	[nearest_tile ?thing2 ?dist2 ==]
	[WHERE (dist<dist2)]
	==>
	[NOT nearest_tile ?thing2 ==]
	[LVARS [time=sim_cycle_number]]
	[nearest_tile ?thing ?dist ?time]

enddefine;

define :ruleset agent_hole_percept_rules;
    ;;;Hole Stuff
    ;;;----------
    [DLOCAL
     [prb_walk = false]
     [prb_chatty = false]
     [prb_show_conditions = false]
     ];

    RULE see_a_hole ;;; Seen a hole
	[NOT stopped]
	[new_sense_data ?thing ?dist]
	[WHERE ishole(thing)]
	==>
	[NOT new_sense_data ?thing ==]
	[NOT seen_hole ?thing ==]
	[LVARS [time=sim_cycle_number]]
	[seen_hole ?thing ?dist ?time]

    RULE see_first_nearest_hole ;;; haven't already seen nearest tile
	[NOT stopped]
	[seen_hole ?hole ?dist ==]
	[WHERE ishole(hole)]
	[NOT nearest_hole ==] ;;; don't have nearest tile
	==>
	[LVARS [time=sim_cycle_number]]
	[nearest_hole ?hole ?dist ?time]


    RULE see_nearer_hole ;;; seen a hole nearer than the nearest
	[NOT stopped]
	[seen_hole ?hole ?dist ==]
	[WHERE ishole(hole)]
	[nearest_hole ?hole2 ?dist2 ==]
	[WHERE (dist<dist2)]
	==>
	[NOT nearest_hole ?hole2 ==]
	[LVARS [time=sim_cycle_number]]
	[nearest_hole ?hole ?dist ?time]

enddefine;

define :ruleset agent_obstacle_percept_rules;
    ;;;Obstacle stuff
    ;;;--------------
    [DLOCAL
     [prb_walk = false]
     [prb_chatty = false]
     [prb_show_conditions = false]
     ];

    RULE see_obstacle
	[NOT stopped]
	[new_sense_data ?thing ?dist]
	[WHERE isobstacle(thing)]
	==>
	[NOT new_sense_data ?thing ?dist]
	[NOT seen_obstacle ?thing == ]
	[LVARS [time=sim_cycle_number]]
	[seen_obstacle ?thing ?dist ?time]

    RULE see_other_agent
	[NOT stopped]
	[new_sense_data ?agent ?dist]
	[WHERE istile_agent(agent)]
	==>
	[NOT new_sense_data ?agent ==]

enddefine;

define :ruleset agent_analyse_percept_rules;
    [DLOCAL
     [prb_walk = false]
     [prb_chatty = false]
     [prb_show_conditions = false]
     ];

    ;;; Rules for picking a tile
    ;;;-------------------------------

    RULE pick_nearest_tile
	[NOT stopped]
	;;;[NOT target_tile == ] ;;;Uncomment this condition to prevent reconsidering plan
	[nearest_tile ?thing ?dist ?time]
	==>
	[NOT target_tile ==]
	[NOT wander_to ==] ;;;Stop wandering now we have a target
	[target_tile ?thing]


;;; Rules for picking a hole
;;;------------------------------

    RULE pick_hole_near    ;;; pick near hole
	[NOT stopped]
	;;;[NOT target_hole ==]
	[nearest_hole ?thing ?dist ?time]
	[have_tile ?tile]  ;;; picked up a tile
	==>
	[NOT target_hole ==]
	[NOT wander_to ==] ;;;Stop wandering now we have a target
	[target_hole ?thing]

enddefine;

define :ruleset agent_move_rules;
    [DLOCAL
     [prb_walk = false]
     [prb_chatty = false]
     [prb_show_conditions = false]
     ];


    ;;;This rule fires when the agent reaches it's target tile - ie it picks
    ;;;up the tile
    RULE arrived_at_tile
	[NOT stopped]
	[target_tile ?thing]
	[NOT have_tile ?thing]
	[environment ?tw]
	[WHERE same_location(sim_myself,thing)]
	==>
	[have_tile ?thing]
	;;;[POP11 picked_up_tile(sim_myself,thing,tw)]

    ;;; This rule fires when the agent has a tile
    ;;; and has a target hole. It then sets its
    ;;; target location fact to be a push to the target hole
    RULE push_tile
	[NOT stopped]
	[have_tile ?tile]
	[target_hole ?hole]
	[WHERE same_location(tile,hole)=false]
	[LVARS[[holex holey]= sim_coords(hole)]]
	==>
	[NOT target_location ==]
	[target_location push ?holex ?holey]

    ;;; This rule is used when the agent has pushed
    ;;; its tile to the target hole. This removes
    ;;; the tile and hole from the environment
    RULE tile_in_hole
	[NOT stopped]
	[have_tile ?tile]
	[target_hole ?hole]
	[environment ?tw]
	[score ?oldscore]
	[WHERE same_location(tile,hole)]
	[LVARS[newscore=oldscore+1]]
	==>
	[NOT score ==]
	[score ?newscore]
	;;; Remove all records of the objects
	[NOT have_tile ?tile ==]
	[NOT target_hole ?hole ==]
	[NOT target_tile ?tile ==]
	[NOT nearest_hole ?hole ==]
	[NOT nearest_tile ?tile ==]
	[NOT seen_tile ?tile ==]
	[NOT seen_hole ?hole ==]
	[NOT target_location ==]
	[do remove_object ?tile ?tw]
	[do remove_object ?hole ?tw]

    ;;;This rule is used when the agent has a target tile
    ;;;it sets the agents target location fact to be a move
    RULE move_to
	[NOT stopped]
	[target_tile ?thing]
	[NOT have_tile ==]
	[LVARS[[newx newy] = sim_coords(thing)]]
	==>
	[NOT target_location ==]
	[target_location move ?newx ?newy]

    ;;; This rule moves the agent about randomly
    ;;; while it doesn't have a target tile
    RULE find_wander_location
	[NOT stopped]
	[NOT seen_tile ==]
	[NOT target_location wander ==]
	[environment ?tw]
	[LVARS[[newx newy]= random_coords(tw)]]
	==>
	[target_location wander ?newx ?newy]
	

    ;;; This rule is the same as above
    ;;; except it's for when the agent has
    ;;; a tile but doesn't have target_hole (can't see a hole)
    RULE find_wander_location_with_tile
    	[NOT stopped]
    	[have_tile ==]
    	[NOT seen_hole ==]
    	[NOT target_location wander ==]
    	[environment ?tw]
    	[LVARS[[newx newy]= random_coords(tw)]]
    	==>
    	[NOT target_location move ==]
    	[target_location wander ?newx ?newy]
    
    
    ;;; This rule removes the wander fact when the
    ;;; wander target has been reached
    RULE reached_wander_target
    	[NOT stopped]
    	[target_location wander ?x ?y]
    	[LVARS[[myx myy] = sim_coords(sim_myself)]]
    	[WHERE ((myx=x)and(myy=y))]
    	==>
    	[NOT target_location wander ?x ?y]
enddefine;

define :ruleset agent_moving_rules;
    [DLOCAL
     [prb_walk = false]
     [prb_chatty = false]
     [prb_show_conditions = false]
     ];

    ;;; If one of the adjacent squares hasn't got a value already it is
    ;;; given one according to how 'good' a move it is If moving to the
    ;;; square takes us closer to the target then the square is given value
    ;;; 2, otherwise it's given 4 (see calc_square_value).  Smaller
    ;;; value=better square.
    
    RULE get_above_square_value
    	[NOT stopped]
    	[LVARS[[my_x my_y] = sim_coords(sim_myself)]]
    	[LVARS[up_x=my_x][up_y=my_y+1]]
    	[NOT square_value ?up_x ?up_y ==]
    	[target_location = ?target_x ?target_y]
    	[LVARS [score = calc_square_value(sim_myself, up_x, up_y, 
					  target_x, target_y)]] ;;; give it a value
    	[LVARS [num = sim_cycle_number]]
    	==>
    	[square_value ?up_x ?up_y ?score ?num]
    	
    RULE get_right_square_value
    	[NOT stopped]
    	[LVARS[[my_x my_y] = sim_coords(sim_myself)]]
    	[LVARS[up_x=my_x+1][up_y=my_y]]
    	[NOT square_value ?up_x ?up_y ==]
    	[target_location = ?target_x ?target_y]
    	[LVARS[score=calc_square_value(sim_myself,up_x,up_y,
				       target_x,target_y)]] ;;; give it a value
    	[LVARS [num = sim_cycle_number]]
    	==>
    	[square_value ?up_x ?up_y ?score ?num]
    	
    RULE get_below_square_value
    	[NOT stopped]
    	[LVARS[[my_x my_y] = sim_coords(sim_myself)]]
    	[LVARS[up_x=my_x][up_y=my_y-1]]
    	[NOT square_value ?up_x ?up_y ==]
    	[target_location = ?target_x ?target_y]
    	[LVARS[score=calc_square_value(sim_myself,up_x,up_y,
				       target_x,target_y)]] ;;; give it a value
    	[LVARS [num = sim_cycle_number]]
    	==>
    	[square_value ?up_x ?up_y ?score ?num]
    
    RULE get_left_square_value
    	[NOT stopped]
    	[LVARS[[my_x my_y] = sim_coords(sim_myself)]]
    	[LVARS[up_x=my_x-1][up_y=my_y]]
    	[NOT square_value ?up_x ?up_y ==]
    	[target_location = ?target_x ?target_y]
    	[LVARS[score=calc_square_value(sim_myself,up_x,up_y,
				       target_x,target_y)]] ;;; give it a value
    	[LVARS [num = sim_cycle_number]]
    	==>
    	[square_value ?up_x ?up_y ?score ?num]
    
    ;;; If a square is blocked then it is given a very high value (50)
    ;;; so the agent won't move there
    RULE blocked_square
    	[NOT stopped]
    	[seen_obstacle ?obst ==]
    	[LVARS[[ob_x ob_y] = sim_coords(obst)]]
    	[LVARS [num = sim_cycle_number]]
    	==>
    	[NOT square_value ?ob_x ?ob_y ==]
    	[square_value ?ob_x ?ob_y 50 ?num]
    
    ;;; Assign a high score to currently occupied square so agent
    ;;; is unlikely to move back to the square again next move (unless blocked)
    RULE set_current_square
    	[NOT stopped]
    	[LVARS[[my_x my_y] = sim_coords(sim_myself)]]
    	[LVARS [num = sim_cycle_number]]
    	==>
    	[NOT square_value ?my_x ?my_y ==]
    	[square_value ?my_x ?my_y 30 ?num]
    
    ;;; There are 4 squares which can be moved to from any location
    ;;; This rule assigns the move_square fact to the square which
    ;;; has the smallest square_value and hence is the best move
    ;;; to perform from the current position.
    RULE pick_random_move
    	[NOT stopped]
    	[LVARS[[my_x my_y] = sim_coords(sim_myself)]]
    	[NOT move_square ==]
    	[square_value ?x ?y ?score ==]
    	[WHERE next_to(my_x,my_y,x,y)]
    	==>
    	[move_square ?x ?y ?score]
    
    RULE get_already_stored_square_values
    	[NOT stopped]
    	[LVARS[[my_x my_y] = sim_coords(sim_myself)]]
    	[square_value ?sx ?sy ?score ==]
    	[WHERE next_to(my_x,my_y,sx,sy)]
    	[move_square ?mx ?my ?score2]
    	[WHERE (score<score2)]
    	==>
    	[NOT move_square ==]
    	[move_square ?sx ?sy ?score]
    
    RULE get_direcition
    	[NOT stopped]
    	[move_square ?newx ?newy ==]
    	[LVARS[[my_x my_y] = sim_coords(sim_myself)]]
    	[LVARS[dir=a_direction(my_x,my_y,newx,newy)]]
    	==>
    	[NOT direction ==]
    	[direction ?dir]
    
    ;;; The following three rules
    ;;; change the graphical trace in some way
    
    RULE move
    	[NOT stopped]
    	[direction ?a_direction]
    	[NOT have_tile ==]
    	==>
    	[NOT move_square ==]
    	[do move_to ?a_direction]
    	
    RULE push
    	[NOT stopped]
    	[have_tile ?tile ==]
    	[direction ?a_direction]
    	==>
    	[NOT move_square ==]
    	[do push_to ?tile ?a_direction]
    
    RULE draw_target  ;;; moves the agent's target indicator
    	[NOT stopped]
    	[target_location = ?x ?y]
    	==>
 	[POP11
    	 if drawgraphics(TW) then
	     ;;; We only have a target to update if the graphics are on.
	     (x, y) -> sim_coords(target(sim_myself));
	     endif;
    	 ]
enddefine;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;    Rulesets for environment
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define :ruleset environment_rules;
    [DLOCAL
     [prb_walk = false]
     [prb_chatty = false]
     [prb_show_conditions = false]
     ];


    ;;; This rule runs every cycle - it removes
    ;;; items whose life cycle has come to an end
    ;;; it also creates new items
    RULE life_rules
    	[NOT stopped]
    	==>
    	;;; Decrease the life of all objects in environment
    	[do decrease_life]
    	;;; Remove dead items
    	[do remove_dead_items]
    	;;; Create new items
    	[do create_new_objects]

enddefine;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;   Rulesystems
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define :rulesystem agent_rulesystem;
    include: agent_repercept_rules
    include: agent_tile_percept_rules
    include: agent_hole_percept_rules
    include: agent_obstacle_percept_rules
    include: agent_analyse_percept_rules
    include: agent_move_rules
    include: agent_moving_rules
enddefine;

define :rulesystem environment_rulesystem;
    include: environment_rules
enddefine;



;;; The following functions are redefined from the standard ones to prevent
;;; all info being printed out during exp.

define vars procedure sim_scheduler_pausing_trace(objects, cycle);
    lvars objects, cycle,input,agentnum;

    ;;; code for dumping a snapshot every team_dump_images cycles
    if isinteger(team_dump_images) and cycle mod team_dump_images == 0 then
    	sysobey('import -window "---- Tileworld ----" -silent ' >< 
		gensym(team_dump_file)><'.yuv');
    endif;
    
     [^((hd(objects)))]=>
    datalength(sim_data(hd(objects)))=>
    [^((hd(tl(objects))))]=>
    datalength(sim_data(hd(tl(objects))))=>
    [^((hd(tl(tl(objects)))))]=>
    datalength(sim_data(hd(tl(tl(objects)))))=>
 
    systime()->newtime;
    newtime-oldtime->timedifference;
    timedifference
=>
    newtime->oldtime;
    ;;; prb_print_table(sim_data(hd(tl(objects))));
    pr('\n==================== end of cycle ' >< cycle >< ' ===============\n');
    pr('Type help for a list of interaction commands\n');
    if(cycle_pause=true) then
	printf('Created %p tiles %p holes %p agents in total\n', 
	       [% TW.num_tiles, TW.num_holes, TW.num_agents %]);
    	while input /= [] do
    	    readline()->input;
    	    if(input=[stop]) then
        	'BYE'=>
        	exitto(sim_scheduler);
    	    elseif(input=[print]) then
        	'press number of agent for its data: '=>
        	print_agents(objects);
        	readline()->agentnum;
        	print_agent_data(hd(agentnum),objects);
    	    elseif(input=[help]) then
        	'type stop to end simulation'=>
        	'type print to get agent data'=>
        	'type nothing and press return to continue'=>
    	    endif;
    	endwhile;
    endif;
enddefine;

define vars procedure sim_scheduler_finished(objects, cycle);
    lvars objects, cycle;
    ;;; user definable
    pr('\n============== Finished. Cycle ' >< cycle >< ' ============\n');
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



;;; Set up the environment.
define setup(tx, ty, ntiles, nholes, nobstacles, nagents, draw_tileworld);
    lvars tx, ty, ntiles, nholes, nobstacles, nagents, draw_tileworld, 
	  i, tile, hole, obstacle, agent;

    if isinteger(team_dump_images) then
        1 -> gensym(team_dump_file);
    endif;
    
    ;;; Create and (optionally) draw the environment.
    make_tileworld(tx, ty, draw_tileworld) -> TW;
    display(TW);

    ;;; Make the border obstacles.
    for i from -1 to (height(TW)) do
	make_border_obstacle(-1, i, TW)-> obstacle;
        obstacle::startup_objects(TW)->startup_objects(TW);
	make_border_obstacle(width(TW), i, TW) -> obstacle;
	obstacle::startup_objects(TW)->startup_objects(TW);
    endfor;

    for i from 0 to (width(TW) - 1) do
	make_border_obstacle(i, -1, TW) -> obstacle;
	obstacle::startup_objects(TW)->startup_objects(TW);
	make_border_obstacle(i, height(TW), TW) -> obstacle;
	obstacle::startup_objects(TW)->startup_objects(TW);
    endfor;

    ;;; Make some tiles.
    for i from 1 to ntiles do
    	make_tile('green', 'T' >< i, TW) -> tile;
	tile::startup_objects(TW)->startup_objects(TW);
    	endfor;

    ;;; Make some holes
    for i from 1 to nholes do
    	make_hole('brown', 'H' >< i, TW) -> hole;
	hole::startup_objects(TW)->startup_objects(TW);
	endfor;
    	
    ;;; Make some obstacles
    for i from 1 to nobstacles do
	make_obstacle('black', TW)-> obstacle;
	obstacle::startup_objects(TW)->startup_objects(TW);
    endfor;

    ;;; Add the agents and setup initial db of agent
    for i from 1 to nagents do
	make_tile_agent('blue', 'A1', TW)->agent;
        agent::startup_objects(TW)->startup_objects(TW);
    	;;; uncomment for debuging
    	;;;1->printlevel(agt1);
    	sim_add_data([environment ^TW], sim_data(agent));
    	sim_add_data([score 0], sim_data(agent));
    endfor;
    
    ;;; Add the environment to the list of objects (its an agent whose rule
    ;;; system must be run) Doing this after the agent has been created
    ;;; means the environments rule system runs before the agent
    ;;;TW::objects(TW)->objects(TW);
    TW::startup_objects(TW)->startup_objects(TW);

enddefine;

;;; Run the tileworld for <cycles> cycles.  If specified, <config> should be
;;; a four element list containing the number of tiles, holes, obstacles and
;;; agents for this tileworld.
define go(cycles /* &optional config, drawgraphics */);
    lvars cycles,
	  tx = tw_xsize,
	  ty = tw_ysize,
	  nt = tw_ntiles,
	  nh = tw_nholes,
	  no = tw_nobstacles,
	  na = tw_nagents,
	  config = false,
	  drawgraphics = tw_drawgraphics;
    ARGS cycles, &OPTIONAL config:islist, drawgraphics:isboolean,;

    if config then
	dl(config) -> (tx, ty, nt, nh, no, na);
	endif;
    	setup(tx, ty, nt, nh, no, na, drawgraphics);
	systime()->oldtime;
    sim_scheduler(startup_objects(TW), cycles);
enddefine;

define continue(cycles);
    sim_scheduler(sim_objects, cycles);
enddefine;

'**********************************************'=>
'TO PAUSE AT THE END OF EACH CYCLE AND ENABLE INTERACTIONS,'=>
''=>
'true->cycle_pause;'=>
''=>
'TO RUN, EXECUTE (MOVE TO AND PRESS ESC-D) THE FOLLOWING LINE,'=>
'CHANGE THE 100 TO ANY NUMBER OF CYCLES'=>
''=>
'go(100);'=>

/* -------- TESTING CODE ---------------
setup();
sim_scheduler(objects(TW),100);
---------------------------------------*/


/* --- Revision History ---------------------------------------------------

Tue Nov 19 11:59:11 2002  Brian Logan  <bsl@cs.nott.ac.uk>

*  go : revised the go and setup procedures to allow configuration of the
   tileworld.  Also propagated the optional draw_graphics flag through to
   the tile, hole and obstacle constructors.

*/
