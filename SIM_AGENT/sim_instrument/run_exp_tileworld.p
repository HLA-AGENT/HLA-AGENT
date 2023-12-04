load sim_process_tw.p;

;;; run_experiment(num_runs, num_cycles, [options]) 
;;;
;;; optional arguments (ie., inside square brackets)
;;; arg1 - num agents
;;; arg2 - window x size 
;;; arg3 - window y size 
;;; arg4 - print to file?  
;;; arg5 - sample length 
;;; arg6 - sample gap

run_experiment(1,1000,[10 25 25 true 5 5]);
