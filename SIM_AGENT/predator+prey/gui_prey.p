;;; Panel Variables
vars prey_create_panel_fields,
     prey_create_panel;

;;; Slider Variables
vars s7,s8,s9,s10,s11,s12,s13,s14,s15,s16;

[
        ;;; Text panel
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap 0}
            {margin 5}
            {align centre} :
            'Add Prey']

        ;;; Text panel
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Start Energy']

        ;;; Slider for prey start energy
        [SLIDERS
            {fg 'black'}
            {label energy}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 0} :
            [s7 {0 1000 200 10} round
                [[{-5 13 '0'}] [{-20 13 '1000'}]]
            ]]

        ;;; Text panel
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Energy Thresholds']

        ;;; Sliders for prey energy thresholds
        [SLIDERS
            {step 10}
            {fg 'black'}
            {label limits}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 0} :
            [s8 {0 1000 350} round
                [[{-5 13 'FULL UP'}][]]
            ]
            [s9 {0 1000 150} round
                [[{-5 13 'HUNGRY'}] []]
            ]
            [s10 {0 1000 50} round
                [[{-5 13 'STARVING'}] []]
            ]]

        ;;; Text panel
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Escaping Properties']

        ;;; Sliders to represent prey escape variables
        [SLIDERS
            {fg 'black'}
            {label escape}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 0} :
            [s11 {0 1500 100 10} round
                [[{-5 13 '0     RANGE'}]
                [{-20 13 '1500'}]]
            ]
            [s12 {0 5 3} round
            [[{-5 13 '0     SPEED'}] [{-10 13 '5'}]]
            ]]

        ;;; Text panel
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Grazing Properties']

        ;;; Sliders to represent prey grazing variables
        [SLIDERS
            {fg 'black'}
            {label feed}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 0} :
            [s13 {0 1500 200 10} round
                [[{-5 13 '0     RANGE'}]
                [{-20 13 '1500'}]]
            ]
            [s14 {0 5 3} round
            [[{-5 13 '0     SPEED'}] [{-10 13 '5'}]]
            ]]

        ;;; Text panel
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap 10}
            {margin 5}
            {align centre} :
            'Flocking Properties']

        ;;; Sliders to represent prey flocking variables
        [SLIDERS
            {fg 'black'}
            {label flock}
            {width 250}
            {height 30}
            {radius 8}
            {barcol 'white'}
            {blobcol 'blue'}
            {framecol 'black'}
            {framewidth 3}
            {spacing 10}
            {gap 10} :
            [s15 {0 1500 200 10} round
                [[{-5 13 '0     RANGE'}]
                [{-20 13 '1500'}]]
            ]
            [s16 {0 5 3} round
            [[{-5 13 '0     SPEED'}] [{-10 13 '5'}]]
            ]]

        ;;; Button to reset the sliders to default values
        [ACTIONS
            {margin 5}
            {offset 50}
            {width 90}
            {align centre} {gap 0} :
            [' DEFAULT' [POP11
            prey_create_panel -> rc_current_window_object;
            200 -> slider_value_of_name(prey_create_panel, "energy", 1);
            350 -> slider_value_of_name(prey_create_panel, "limits", 1);
            150 -> slider_value_of_name(prey_create_panel, "limits", 2);
            50 -> slider_value_of_name(prey_create_panel, "limits", 3);
            100 -> slider_value_of_name(prey_create_panel, "escape", 1);
            3 -> slider_value_of_name(prey_create_panel, "escape", 2);
            200 -> slider_value_of_name(prey_create_panel, "feed", 1);
            3 -> slider_value_of_name(prey_create_panel, "feed", 2);
            200 -> slider_value_of_name(prey_create_panel, "flock", 1);
            3 -> slider_value_of_name(prey_create_panel, "flock", 2);
            ]]]

        ;;; Text panel
        [TEXT {bg 'black'}
            {fg 'brown'}
            {gap 0}
            {margin 5}
            {align centre} :
            'Add Number']

        ;;; Panel to enter the number of prey to be added
        [NUMBERIN
            {label number_of_prey}
            {align centre}
            {gap 0}
            {margin 10}
            {offset 10}
            {width 90}
            {height 10}
            {font '10x20'}
            {bg 'blue'}:
            1]

        ;;; Buttons to add prey
        [ACTIONS
            {margin 15}
            {offset 50}
            {width 90}
            {align centre}
            {gap 0} :
            [' MANUAL' [POP11
            if world_create = false then return() endif;
            add_prey(
            rc_field_item_of_name(prey_create_panel,"number_of_prey",1),1,
            slider_value_of_name(prey_create_panel, "energy", 1),
            slider_value_of_name(prey_create_panel, "limits", 1),
            slider_value_of_name(prey_create_panel, "limits", 2),
            slider_value_of_name(prey_create_panel, "limits", 3),
            slider_value_of_name(prey_create_panel, "escape", 1),
            slider_value_of_name(prey_create_panel, "escape", 2),
            slider_value_of_name(prey_create_panel, "feed", 1),
            slider_value_of_name(prey_create_panel, "feed", 2),
            slider_value_of_name(prey_create_panel, "flock", 1),
            slider_value_of_name(prey_create_panel, "flock", 2));
            ]]
            [' RANDOM' [POP11
            if world_create = false then return() endif;
            add_prey(
            rc_field_item_of_name(prey_create_panel,"number_of_prey",1),2,
            slider_value_of_name(prey_create_panel, "energy", 1),
            slider_value_of_name(prey_create_panel, "limits", 1),
            slider_value_of_name(prey_create_panel, "limits", 2),
            slider_value_of_name(prey_create_panel, "limits", 3),
            slider_value_of_name(prey_create_panel, "escape", 1),
            slider_value_of_name(prey_create_panel, "escape", 2),
            slider_value_of_name(prey_create_panel, "feed", 1),
            slider_value_of_name(prey_create_panel, "feed", 2),
            slider_value_of_name(prey_create_panel, "flock", 1),
            slider_value_of_name(prey_create_panel, "flock", 2));
            ]]
            ]

        ;;; Button to close the panel
        [ACTIONS
            {bg 'black'}
            {margin 10}
            {offset 50} ;;; Horizontal displacement if right or left
            ;;; button width
            {width 90}
            {align centre} {gap 0} :
            ['   CLOSE'[POP11 delete(prey_create_panel,panel_list)->panel_list;
                            rc_kill_menu(); 0 -> prey_flag;]]
        ]

    ]  -> prey_create_panel_fields;
