"object346"->object;
"sim_x"->slotname;

"object235"->object2;
"sim_y"->slotname2;

vars shared = [];

newanyproperty([], 20, 1, false, syshash, 
	       nonop =, "tmparg", [], false) -> slot_prop;

;;;Create one key,value pair
cons("face",slot_prop(conspair(object,slotname)))->slot_prop(conspair(object,slotname));

cons("pooface",slot_prop(conspair(object,slotname)))->slot_prop(conspair(object,slotname));

;;;Create another key, value pair entry

cons("face",slot_prop(conspair(object2,slotname2)))->slot_prop(conspair(object2,slotname2));


;;; A procedure called by approperty which explodes all the items in
;;; the proprty into a single list. It only adds the item to the
;;; exploded list if it has more than two access/updates in its
;;; value(list) 
define explode_common(key,value);
    if(length(value)>1) then 
	value <> shared->shared;
    endif;
enddefine;



define lconstant printage(key,value);
    pr(key), pr(`\t`), pr(value), pr(newline);
enddefine;

appproperty(slot_prop, printage);
appproperty(slot_prop, remove_small);
shared=>
