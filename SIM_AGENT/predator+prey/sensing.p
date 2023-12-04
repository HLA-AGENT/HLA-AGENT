/*
METHOD   : sim_sense_agent (ag, obs, dist)
INPUTS   : ag, obs, dist
  Where  :
    ag is an agent
    obs is an obstacle
    dist is the distance between them
OUTPUTS  : NONE
USED IN  : Sensing an agents environment
CREATED  : Nov 1998
PURPOSE  : Provide neccessary sense information when a basic_agent senses an
obstacle.
*/

define :method sim_sense_agent(ag:sim_agent,obs:obstacle,dist);

    unless ag == obs or dist > sense_limit then
        ;;; Enter sensor info into the agents database
        [new_sense_data obstacle ^obs ^dist ]
    endunless

enddefine;

/*
METHOD   : sim_sense_agent (prey, fd, dist)
INPUTS   : prey, fd, dist
  Where  :
    prey is a prey agent
    fd is a food object
    dist is the distance between them
OUTPUTS  : NONE
USED IN  : Sensing an agents environment.
CREATED  : Feb 1999
PURPOSE  : Provide neccessary sense information when a prey agent senses a
food object
*/

define :method sim_sense_agent(prey:prey_agent,fd:food,dist);

    unless prey == fd or dist > sense_limit then
        ;;; Enter sensor info into the agents database
        [new_sense_data food ^fd ^dist ]
    endunless

enddefine;

/*
METHOD   : sim_sense_agent (ag, pred, dist)
INPUTS   : ag, pred, dist
  Where  :
    ag is a basic agent
    pred is a predator agent
    dist is the distance between them
OUTPUTS  : NONE
USED IN  : Sensing an agents environment
CREATED  : Nov 1998
PURPOSE  : Provide neccessary sense information when a basic agent senses a
predator agent.
*/

define :method sim_sense_agent(ag:sim_agent,pred:predator_agent,dist);

    unless ag == pred or dist > sense_limit then
        ;;; Enter sensor info into the agents database
        [new_sense_data predator ^pred ^dist ]
    endunless

enddefine;

/*
METHOD   : sim_sense_agent (ag, prey, dist)
INPUTS   : ag, prey, dist
  Where  :
    ag is a basic agent
    prey is a prey agent
    dist is the distance between them
OUTPUTS  : NONE
USED IN  : Sensing an agents environment
CREATED  : Nov 1998
PURPOSE  : Provide neccessary sense information when a basic agent senses
a prey agent.
*/

define :method sim_sense_agent(ag:sim_agent,prey:prey_agent,dist);

    unless ag == prey or dist > sense_limit then
        ;;; Enter sensor info into the agents database
        [new_sense_data prey ^prey ^dist ]
    endunless

enddefine;



/*
METHOD   : sim_sense_agent (w, _, dist)
INPUTS   : w, (prey or fd or obs or pred), dist
  Where  :
    w is a world object
    prey is a prey agent
    fd is a food object
    obs is an obstacle
    pred is a predator agent
    dist is the distance between them
OUTPUTS  : NONE
USED IN  : Sensing the world.
CREATED  : Feb 1999
PURPOSE  : Provides neccessary sense information when the world senses any
object.
*/

define :method sim_sense_agent(w:world,prey:prey_agent,dist);

    unless w == prey or dist > sense_limit then
        ;;; Enter sensor info into the worlds database
        [new_sense_data prey ^prey ^dist ]
    endunless

enddefine;

define :method sim_sense_agent(w:world,fd:food,dist);

    unless w == fd or dist > sense_limit then
        ;;; Enter sensor info into the worlds database
        [new_sense_data food ^fd ^dist ]
    endunless

enddefine;

define :method sim_sense_agent(w:world,obs:obstacle,dist);

    unless w == obs or dist > sense_limit then
        ;;; Enter sensor info into the worlds database
        [new_sense_data obstacle ^obs ^dist ]
    endunless

enddefine;

define :method sim_sense_agent(w:world,pred:predator_agent,dist);

    unless w == pred or dist > sense_limit then
        ;;; Enter sensor info into the worlds database
        [new_sense_data predator ^pred ^dist ]
    endunless

enddefine;

/*
METHOD   : sim_sense_agent (obs, obj, dist)
INPUTS   : obs, obj, dist
  Where  :
    obs is an obstacle
    obj is any possible sim object
    dist is the distance between them
OUTPUTS  : NONE
USED IN  : Provide neccessary sense information when an obstacle sense
anything.
CREATED  : Nov 1998
PURPOSE  : Obstacles do not sense anything.

TESTS:

*/

define :method sim_sense_agent(obs:obstacle,obj:sim_object,dist);

enddefine;


/*
METHOD   : sim_sense_agent (fd, obj, dist)
INPUTS   : fd, obj, dist
  Where  :
    fd is a food object
    obj is a sim_object
    dist is the distance between them
OUTPUTS  : NONE
USED IN  : Sensing the food's environment.
CREATED  : Feb 1999
PURPOSE  : Food object don't sense anything so don't enter anything into the
database
*/

define :method sim_sense_agent(fd:food,obj:sim_object,dist);

enddefine;


/*
METHOD   : sim_sense_agent (ob, w, dist)
INPUTS   : ob, w, dist
  Where  :
    ob is a sim_object
    w is a world object
    dist is a the distance between them
OUTPUTS  : NONE
USED IN  : Sensing an objects environment
CREATED  : Feb 1999
PURPOSE  : Nothing can sense the world object so don't enter anything inot the
database
*/

define :method sim_sense_agent(ob:sim_object,w:world,dist);

enddefine;
