load sim_process_boids.p;

;;; run_experiment(num_runs, num_cycles, [options]) 
;;;
;;; options 
;;; arg1 - num boids
;;; arg2 - window x size 
;;; arg3 - window y size 
;;; arg4 - print to file?  
;;; arg5 - sample length 
;;; arg6 - sample gap
sysobey('mkdir boids_results');
'boids_results'->current_directory;
[2 4 8 16 32 64 128 256]->agent_num_list;

for a in agent_num_list do
    sysobey('mkdir '><a><'agents');
    a><'agents'->current_directory;
    ;;;sysobey('cd '><a><'agents');
    for dimension from 100 by 100 to 1000 do
	sysobey('mkdir '><dimension><'x'><dimension);
	dimension><'x'><dimension -> current_directory;
	;;;	sysobey('cd '><dimension><'x'><dimension);
	run_experiment(5, 500,[^a ^dimension ^dimension true 5 5]);
	'..'->current_directory;
    endfor;
    '..'->current_directory;
endfor;


;;;---2 agents---
;;;sysobey('mkdir 2agents');
;;;sysobey('cd 2agents');
;;;sysobey('mkdir 100x100');
;;;sysobey('cd 100x100');
;;;run_experiment(5,500,[2 100 100 true 5 5]);
;;;sysobey('mkdir 200x200');
;;;sysobey('cd 200x200');
;;;run_experiment(5,500,[2 200 200 true 5 5]);
;;;sysobey('mkdir 300x300');
;;;sysobey('cd 300x300');
;;;run_experiment(5,500,[2 300 300 true 5 5]);
;;;sysobey('mkdir 400x400');
;;;sysobey('cd 400x400');
;;;run_experiment(5,500,[2 400 400 true 5 5]);
;;;sysobey('mkdir 500x500');
;;;sysobey('cd 500x500');
;;;run_experiment(5,500,[2 500 500 true 5 5]);
;;;sysobey('mkdir 600x600');
;;;sysobey('cd 600x600');
;;;run_experiment(5,500,[2 600 600 true 5 5]);
;;;sysobey('mkdir 700x700');
;;;sysobey('cd 700x700');
;;;run_experiment(5,500,[2 700 700 true 5 5]);
;;;sysobey('mkdir 800x800');
;;;sysobey('cd 800x800');
;;;run_experiment(5,500,[2 800 800 true 5 5]);
;;;sysobey('mkdir 900x900');
;;;sysobey('cd 900x900');
;;;run_experiment(5,500,[2 900 900 true 5 5]);
;;;sysobey('mkdir 1000x1000');
;;;sysobey('cd 1000x1000');
;;;run_experiment(5,500,[2 1000 1000 true 5 5]);
