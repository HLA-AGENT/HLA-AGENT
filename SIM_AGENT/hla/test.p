 uses objectclass;

vars makefoos = true,
     doupdates = true;

define update_wrapper1(o, upd_p); 
    printf('Updating the slot:\n', []);
    enddefine;

define update_wrapper2(o, upd_p); 
    printf('Updating the slot: %p of %p \n', [^upd_p ^o]);
    enddefine;

define update_wrapper3(o, upd_p); 
    printf('Updating the slot: %p of %p \n', [^wrapper_invoker ^o]);
    enddefine;

define update_wrapper4(p); 
    lvars value, o;
    -> o; -> value;

    ;;; Is it safe to assume that objectclass slot updaters will always take
    ;;; two arguments?
    printf('wrapper pdnargs: %p pdprops: %p wrapped procedure pdnargs: %p pdprops %p\n',
	   [^(pdnargs(p)) ^ (pdprops(p)) 
	    ^(pdnargs(wrapper_invoker)) ^(pdprops(wrapper_invoker))]);
    printf('Check equality: %p %p\n', [^(valof("colour") = wrapper_invoker)
				       ^("colour" = pdprops(wrapper_invoker))]);
    if doupdates then
    	printf('Updating the slot: %p of %p with value %p\n', 
	       [^wrapper_invoker ^o ^value]);
	;;; value -> p(o);
	value; o; p();
	printf('Slot value now %p\n', [^(wrapper_invoker(o))]);
    else
	printf('Ignoring the update: %p of %p with value %p\n', 
	       [^wrapper_invoker ^o ^value]);
	printf('Slot value remains: %p\n', [^(wrapper_invoker(o))]);
	endif;
    enddefine;

define update_wrapper5(value, o, p); 

    ;;; Is it safe to assume that objectclass slot updaters will always take
    ;;; two arguments?
    printf('wrapper pdnargs: %p pdprops: %p wrapped procedure pdnargs: %p pdprops %p\n',
	   [^(pdnargs(p)) ^ (pdprops(p)) 
	    ^(pdnargs(wrapper_invoker)) ^(pdprops(wrapper_invoker))]);
    printf('Check equality: %p %p\n', [^(valof("colour") = wrapper_invoker)
				       ^("colour" = pdprops(wrapper_invoker))]);
    if doupdates then
    	printf('Updating the slot: %p of %p with value %p\n', 
	       [^wrapper_invoker ^o ^value]);
	;;; value -> p(o);
	value; o; p();
	printf('Slot value now %p\n', [^(wrapper_invoker(o))]);
    else
	printf('Ignoring the update: %p of %p with value %p\n', 
	       [^wrapper_invoker ^o ^value]);
	printf('Slot value remains: %p\n', [^(wrapper_invoker(o))]);
	endif;
    enddefine;

define cons_wrapper(p);
    unless makefoos then
	repeat pdnargs(p) times
	    erase(_) ->;
	    endrepeat;
    	else
	    p();
	    endunless;
    enddefine;

define :class foo;
    slot colour;
    
    ;;; on cons do cons_wrapper;
    ;;; on update(o, upd_p) do update_wrapper3(o, upd_p);
    on update do update_wrapper5;
    enddefine;


vars x = newfoo();
x =>
"red" -> colour(x);
x =>
false -> doupdates;
"blue" -> colour(x);

define :class bar;
    slot position = 1;
    enddefine;

define :wrapper position(b:bar, acc_p);
    printf('Accessing the slot: %p %p\n', [% acc_p, pdprops(acc_p) %]);
    enddefine;

define :wrapper updaterof position(value, b:bar, upd_p); 
    printf('Updating the slot: %p %p with %p\n', [% upd_p, pdprops(upd_p), value%]);
    enddefine;

vars y = newbar() =>
position(y) =>
10 -> position(y);

100 -> wrapper_deref(position)(y);

define :class test1 is HLA_object;
    enddefine;

'test1' :: HLA_FOM_classes -> HLA_FOM_classes;
test1_key -> HLA_class_keys('test1');
101 -> HLA_class_handles(test1_key);

;;; Creation should not trigger the update wrappers
vars x = newtest1();
x =>
HLA_instanceId(x) =>
HLA_proxy_object(x) =>
HLA_federate_objects(HLA_federate) =>

define :class test2 is sim_object;
    slot position;
    slot colour;
    enddefine;

'test2' :: HLA_FOM_classes -> HLA_FOM_classes;
test2_key -> HLA_class_keys('test2');
102 -> HLA_class_handles(test2_key);

;;; Creation should not trigger the update wrappers
vars y = newtest2();
y =>
HLA_instanceId(y) =>
HLA_proxy_object(y) =>
HLA_federate_objects(HLA_federate) =>
;;; this should trigger the update wrappers
10 -> position(y);
position(y) =>
"blue" -> colour(y);
colour(y) =>

define :class student;
    slot student_course;
    ;;; print a message for each new student
    on new(student) do
        printf('One more student to teach!\n');
    on update(o, upd_p) do 
        printf('Updating the slot\n');
    enddefine;

define :class mature_student is student;
    enddefine;

define :class overseas_student is student;
    slot fees;
    enddefine;

;;; Example of updater pdprops and pdnargs
define pairvalues(p) -> (x, y);
    explode(p) -> (x, y);
enddefine;

vars u = conspair(1, 2);
u =>
pairvalues(u) =>
define updaterof pairvalues(a, b, p);
    a -> front(p);
    b -> back(p);
enddefine;
(3, 4) -> pairvalues(u);
u =>
pdprops(pairvalues) =>
pdprops(updater(pairvalues)) =>
pdnargs(pairvalues) =>
pdnargs(updater(pairvalues)) =>

;;; using apply
apply(explode([1 2 ^conspair])) =>
vars procedure makepair = apply(% conspair %);
makepair(3, 4) =>