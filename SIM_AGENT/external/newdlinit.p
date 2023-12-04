uses newexternal

external declare foo in c;
    void dlinit()
    {}

    void createFederationExecution(executionName,Fedfile)
     char *executionName;
     char *Fedfile;
    {}

endexternal;

external load foo;
    '/home/mhl/SIM_AGENT/external/dlinit.so'
endexternal;

[calling dlinit] =>
dlinit();
[dlopen complete] =>


