;;; Panel Variables
vars startup_panel_fields,
     predator_create_panel_fields,
     prey_create_panel_fields;
vars startup_panel,
     predator_create_panel,
     prey_create_panel;

;;; Field definitions

[
        {width 400}

        ;;; Text field
        [TEXT {bg 'black'}
                {fg 'brown'}
                {gap 0}
                {margin 10}
                {align centre} :
                'POP Predation']

        ;;; Text field
        [TEXT {bg 'black'}
                {fg 'brown'}
                {gap 5}
                {margin 10}
                {align centre} :
                'A Predator/Prey Simulator']

        ;;; Text field
        [TEXT {bg 'black'}
                {fg 'brown'}
                {gap 15}
                {margin 10}
                {align left} :
                '1. Create World']
        ;;; Button to open world create panel
        [ACTIONS
            {margin 20}
            {offset 10}
            {width 90}
            {align right}
            {cols 1}
            {gap -57}:
            ['      GO' [POP11
            if world_flag = 0 then
                rc_control_panel(500,150,world_create_panel_fields,
                    'World Create') -> world_create_panel;
                1 -> world_flag;
                world_create_panel::panel_list->panel_list;
            endif;
            ]]
        ]

        ;;; Text field
        [TEXT {bg 'black'}
                {fg 'brown'}
                {gap 10}
                {margin 10}
                {align left} :
                 '2. Add Predators']

        ;;; Button to open the add predators panel
        [ACTIONS
            {margin 20}
            {offset 10}
            {width 90}
            {align right}
            {cols 1}
            {gap -57}:
            ['      GO'[POP11
                    if pred_flag = 0 then
                        rc_control_panel(500,5,predator_create_panel_fields,
                        'Add Predators') -> predator_create_panel;
                        1 -> pred_flag;
                        predator_create_panel::panel_list->panel_list;
                    endif;
            ]]
        ]

        ;;; Text field
        [TEXT {bg 'black'}
                {fg 'brown'}
                {gap 10}
                {margin 10}
                {align left} :
                '3. Add Prey']

        ;;; Button to open the add prey panel
        [ACTIONS
            {margin 20}
            {offset 10}
            {width 90}
            {align right}
            {cols 1}
            {gap -57}:
            ['      GO' [POP11
                    if prey_flag = 0 then
                        rc_control_panel(500,5,prey_create_panel_fields,
                        'Add Prey') -> prey_create_panel;
                        1 -> prey_flag;
                        prey_create_panel::panel_list->panel_list;
                    endif;
            ]]
        ]

        ;;; Text field
        [TEXT {bg 'black'}
                {fg 'brown'}
                {gap 10}
                {margin 10}
                {align left} :
                '4. Set Global Variables']

        ;;; Button to open the global variables panel
        [ACTIONS
            {margin 20}
            {offset 10}
            {width 90}
            {align right}
            {cols 1}
            {gap -57}:
            ['      GO' [POP11
                    if vars_flag = 0 then
                        rc_control_panel(500,5,vars_panel_fields,
                        'Global Variables') -> vars_panel;
                        1 -> vars_flag;
                        vars_panel::panel_list->panel_list;
                    endif;
            ]]
        ]

        ;;; Text field
        [TEXT {bg 'black'}
                {fg 'brown'}
                {gap 10}
                {margin 10}
                {align left} :
                '5. Run Agents']

        ;;; Button to open the run agents panel
        [ACTIONS
            {margin 20}
            {offset 10}
            {width 90}
            {align right}
            {cols 1}
            {gap -57}:
            ['      GO' [POP11
                    if run_flag = 0 then
                        rc_control_panel(500,200,sched_panel_fields,
                        'Run Simulation') -> sched_panel;
                        1 -> run_flag;
                        sched_panel::panel_list->panel_list;
                    endif;
            ]]
        ]

        ;;; Button to exit the simulation
        [ACTIONS
            {bg 'black'}
            {margin 10}
            {width 90}
            {align centre} {gap 10} :
            ['    EXIT'[POP11  0 -> world_flag;
                                0 -> pred_flag;
                                0 -> prey_flag;
                                0 -> vars_flag;
                                0 -> run_flag;
                                killwindows();
                                killmenus();
                                startup_panel -> rc_current_panel;
                                rc_kill_window_object(rc_current_panel);]]
        ]
    ] ->  startup_panel_fields;




/*
PROCEDURE: go ()
INPUTS   : NONE
OUTPUTS  : NONE
USED IN  : starting the simulation
CREATED  : Mar 1999
PURPOSE  : Starts the simulation
TESTS:
go();
*/

define go();

    [] -> panel_list;
    ;;; Create the startup panel
    rc_control_panel(850,150,startup_panel_fields,'Menu')
                -> startup_panel;
enddefine;
