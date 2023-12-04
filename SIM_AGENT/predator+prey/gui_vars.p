;;; Panel variables
vars vars_panel_fields,
     vars_panel;

;;; Slider Variables
vars s17,s18,s19,s20;

/*
PROCEDURE: set_flocking (button)
INPUTS   : button is radio button
OUTPUTS  : NONE
USED IN  : vars_panel
CREATED  : Mar 1999
PURPOSE  : Radio button callback to set the type of flocking to be used
*/

define set_flocking(button);

    lvars label = rc_button_label(button);

    ;;; Put the button label into flock_type
    label -> flock_type;

enddefine;


/*
PROCEDURE: set_attack (button)
INPUTS   : button is a radio button
OUTPUTS  : NONE
USED IN  : vars_panel
CREATED  : Mar 1999
PURPOSE  : Radio button callback to set the type of attack to be used
*/

define set_attack(button);

    lvars label = rc_button_label(button);

    ;;; Put the button label into attack_type
    label -> attack_type;

enddefine;

;;; Definition of panel fields

[
        ;;; Text field
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap 0}
            {margin 5}
            {align centre} :
            'Global Variables']

        ;;; Text field
        [TEXT {bg 'black'} {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Predator Attack Style']

        ;;; Radio buttons to select attack type
        [RADIO {gap 0}
            {label attack_type}
            {width 70}
            {height 20}
            {margin 4}
            {fieldbg 'light gray'}
            {spec {^rc_radio_select_action ^set_attack}}
            {cols 2} :
            'isolate' 'direct'
        ]

        ;;; Text field
        [TEXT {bg 'black'} {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Flocking Style']

        ;;; Radio buttons to select flock type
        [RADIO {gap 0}
            {label flocking_type}
            {width 70}
            {height 20}
            {margin 4}
            {fieldbg 'light gray'}
            {spec {^rc_radio_select_action ^set_flocking}}
            {cols 3} :
            'none' 'point' 'group'
            'herd' 'leader' 'boids'
        ]

        ;;; Text field
        [TEXT {bg 'black'} {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Flock Separation']

        ;;; Slider to represent flock separation
        [SLIDERS
            {fg 'black'}
            {label flocking}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 0} :
            [s17 {10 60 30 5} round
                [[{-5 13 '10'}] [{-12 13 '60'}]]
            ]]

        ;;; Text field
        [TEXT {bg 'black'} {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Local Flocking Radius']

        ;;; Text field
        [TEXT {bg 'black'} {fg 'brown'}
            {gap -5}
            {margin 5}
            {align centre} :
            '(group/herd/boids)']

        ;;; Slider to represent the local flocking radius
        [SLIDERS
            {fg 'black'}
            {label box}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 0} :
            [s18 {10 100 30 5} round
                [[{-5 13 '10'}] [{-12 13 '100'}]]
            ]]

        ;;; Text field
        [TEXT {bg 'black'} {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Local Attack Radius']

        ;;; Text field
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap -5}
            {margin 5}
            {align centre} :
            '(isolate only)']

        ;;; Slider to represent the local attack radius
        [SLIDERS
            {fg 'black'}
            {label a_radius}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 0} :
            [s20 {10 100 40 5} round
                [[{-5 13 '10'}] [{-12 13 '100'}]]
            ]]



        ;;; Text field
        [TEXT {bg 'black'} {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Extra Effort']

        ;;; Text field
        [TEXT {bg 'black'} {fg 'brown'}
            {gap -5}
            {margin 5}  ;;; margin above and below the text
            {align centre} :
            ;;; Now the strings
            '(when starving)']

        ;;; Slider to represent the extra effort made when an agent is
        ;;; starving
        [SLIDERS
            {fg 'black'}
            {label extra}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 0} :
            [s19 {0 3 1 1} round
                [[{-5 13 '0'}] [{-12 13 '3'}]]
            ]]

        ;;; Buttons to set variables, or to return them to default values
        [ACTIONS
            {margin 5}
            {offset 50}
            {width 90}
            {align centre}
            {gap 10} :
            ['     SET' [POP11
            slider_value_of_name(vars_panel, "flocking", 1)
            -> flock_separation;
            slider_value_of_name(vars_panel, "box", 1) -> flocking_radius;
            slider_value_of_name(vars_panel, "extra", 1) -> extra;
            slider_value_of_name(vars_panel, "a_radius", 1) -> attack_radius;
            ]]
            [' DEFAULT' [POP11
            vars_panel -> rc_current_window_object;
            30 -> slider_value_of_name(vars_panel, "flocking", 1);
            30 -> slider_value_of_name(vars_panel, "box", 1);
            1 -> slider_value_of_name(vars_panel, "extra", 1);
            40 -> slider_value_of_name(vars_panel, "a_radius", 1);
            ]]]


        ;;; Button to close the panel
        [ACTIONS
            {bg 'black'}
            {margin 10}
            {offset 50}
            {width 90}
            {align centre} {gap 10} :
            ['   CLOSE'[POP11 delete(vars_panel,panel_list)->panel_list;
                            rc_kill_menu(); 0 -> vars_flag;]]
        ]

]  -> vars_panel_fields;
