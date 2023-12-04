;;; Modified version of datafile that "reads" its input from a string and
;;; "writes" its output to a string.

;;; Permitted datatypes are: words, numbers, lists, vector types, record
;;; types, vector arrays, ordinary properties, booleans


vars
      procedure (datastrread, datastrwrite)
      ;

lvars
      charsonline,
      ;

define HLA_encode_data(data_struct) -> string;
    lvars stringlength = 0;
    dlocal charsonline = 0,
           cucharout = procedure(x);
		           x;
			   1 + charsonline -> charsonline;
			   1 + stringlength -> stringlength;
			   endprocedure;

    datastrwrite(data_struct);
    consstring(stringlength) -> string;
enddefine;


define HLA_decode_data(string) -> data_struct;
    datastrread(incharitem(stringin(string))) -> data_struct;
enddefine;


define datastrwrite(x);
    lvars x, lo;
    if charsonline > 60 then
	nl(1);
	0 -> charsonline
	endif;
    sp(1);
    if isnumber(x) then
	pr(x)
    elseif isword(x) do
	;;; printing the structure takes up more space and is slower than
	;;; just printing the word but it ensures that words with non printing
	;;; characters are stored properly (eg -space-) and datafile control
	;;; words (e.g. zw) are not confused.
	spr("zw"); pr(datalength(x));
	appdata(x, datastrwrite)
    elseif islist(x) then
	spr("zl"); pr(length(x));
	applist(x, datastrwrite)
    elseif isstring(x) then
	spr("zs"); pr(datalength(x));
	appdata(x, datastrwrite)
    elseif isvector(x) then
	spr("zv"); pr(datalength(x));
	appdata(x, datastrwrite)
    elseif isarray(x) then
	arrayvector_bounds(x) -> (, lo);
	pr("za");
	datastrwrite(isarray_by_row(x));
	datastrwrite(lo - 1);
	datastrwrite(boundslist(x));
	datastrwrite(arrayvector(x))
    elseif isref(x) then
	pr("zr"); datastrwrite(cont(x))
    elseif isboolean(x) then
	spr("zb");
	pr(if x then "true" else "false" endif)
    elseif isvectorclass(x) then
	spr("zu"); spr(dataword(x)); pr(datalength(x)); appdata(x, datastrwrite)
    elseif isproperty(x) then
	spr("zh");
	datastrwrite(datalist(x));
	datastrwrite(property_size(x));
	datastrwrite(property_default(x));
	datastrwrite(true)        ;;; Can't tell if permanent or not
	else
	    spr("zc"); pr(dataword(x)); appdata(x, datastrwrite)
	    endif
	enddefine;


define datastrread(rditem) -> x;
    lvars x, y, n, key, by_row = false;
    rditem() -> x;
    if x == "zl" then
	rditem() -> n;
	conslist(repeat n times datastrread(rditem) endrepeat, n) -> x
    elseif x == "zp" then
	conspair(datastrread(rditem), datastrread(rditem)) -> x
    elseif x == "zs" then
	rditem() -> y;
	inits(y) -> x;
	for n from 1 to y do
	    datastrread(rditem) -> fast_subscrs(n, x)
	    endfor
    elseif x == "zv" then
	rditem() -> y;
	initv(y) -> x;
	for n from 1 to y do
	    datastrread(rditem) -> fast_subscrv(n, x)
	    endfor
    elseif x == "za" then
	datastrread(rditem) -> n;
	if isboolean(n) then
	    true -> by_row;
	    datastrread(rditem) -> n;
	    endif;
	if isinteger(n) then
	    ;;; quite new format - this is the offset
	    ;;; bounds & arrayvector follow
	    datastrread(rditem) -> x;
	    datastrread(rditem) -> y;
	    newanyarray(x, y, n, by_row) -> x
	else
	    ;;; old format - this is the boundslist, data follows
	    newarray(n) -> x;
	    arrayvector(x) -> y;
	    for n from 1 to datalength(y) do
		datastrread(rditem) -> fast_subscrv(n, arrayvector(x))
		endfor
	    endif
    elseif x == "zr" then
	consref(datastrread(rditem)) -> x
    elseif x == "zb" then
	valof(datastrread(rditem)) -> x
    elseif x == "zw" then
	rditem() -> n;
	consword(repeat n times datastrread(rditem) endrepeat, n) -> x
    elseif x == "zc" then
	datastrread(rditem) -> y;
	key_of_dataword(y) -> key;
	unless key then
	    mishap('Unknown dataword encountered in datafile\n' sys_><
		   ';;;          (recordclass declaration not loaded?)', [^y]);
	    endunless;
	repeat datalength(key) times datastrread(rditem) endrepeat;
	apply(class_cons(key)) -> x
    elseif x == "zu" then
	datastrread(rditem) -> y;
	key_of_dataword(y) -> key;
	unless key then
	    mishap('Unknown dataword encountered in datafile\n' ><
		   ';;;          (vectorclass declaration not loaded?)', [^y]);
	    endunless;
	repeat (datastrread(rditem) ->> n) times datastrread(rditem) endrepeat;
	apply(n, class_cons(key)) -> x
    elseif x == "zh" then
	newproperty(datastrread(rditem), datastrread(rditem), 
		    datastrread(rditem), datastrread(rditem)) -> x
	endif
    enddefine;

;;; hla_datastring ends here
vars hla_datastring = true;
