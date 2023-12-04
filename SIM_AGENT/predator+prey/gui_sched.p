;;; Panel Variables
vars sched_panel_fields,
     sched_panel;

[
        ;;; Text panel
        [TEXT {bg 'black'} {fg 'brown'}
            {gap 0}
            {margin 5}
            {align centre} :
            'Run Agents']

        ;;; Text panel
        [TEXT {bg 'black'} {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Cycles']

        ;;; Panel to enter the number of cycles to run the simulation for
        [NUMBERIN
            {label cycles}
            {align centre}
            {gap 0}
            {margin 10}
            {offset 10}
            {width 90}
            {height 10}
            {font '10x20'}
            {bg 'blue'}:
            500]

        ;;; Button to run the simulation
        [ACTIONS
            {margin 15}
            {offset 50}
            {width 90}
            {align centre}
            {gap 0} :
            ['RUN' [POP11
            if world_create = false then return() endif;
            run_agents(rc_field_item_of_name(sched_panel, "cycles", 1));]
            ]]

        ;;; Button to close the panel
        [ACTIONS
            {bg 'black'}
            {margin 10}
            {offset 50}
            {width 90}
            {align centre}
            {gap 0} :
            ['   CLOSE' [POP11 delete(sched_panel,panel_list)->panel_list;
                            rc_kill_menu(); 0 -> run_flag;]]
        ]
]  -> sched_panel_fields;
