#include "tileWorld.hh"
#include "TwFederateAmbassador.hh"

#if !defined(WIN32) && !defined(VXWORKS) && !defined(__KCC) && (__SUNPRO_CC_COMPAT != 5)
#  include <stream.h>
#endif

#include <string.h>
#include <stdlib.h>

#if !defined(WIN32) && !defined(VXWORKS)
#  include <unistd.h>
#endif

#include <RTI.hh>

#if defined(WIN32)
#  include <windows.h>   // for "Sleep"
#  include <sys/timeb.h> // for "struct _timeb"
#  include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#elif defined(VXWORKS)
#  include <ace/OS.h>
#  include <iostream>
#  include <timers.h>
using std::cout;
using std::flush;
using std::endl;
using std::cerr;
#else
#  include <sys/time.h>
#endif

#include <fedtime.hh>

RTI::Boolean        timeAdvGrant = RTI::RTI_FALSE;
RTI::Boolean        TimeRegulation = RTI::RTI_FALSE;
RTI::Boolean        TimeConstrained = RTI::RTI_FALSE;
RTI::FedTime &          grantTime=(*(RTI::FedTimeFactory::makeZero()));

void printUsage( const char* pExeName )
{
   cout << "FED_HW: usage: "
        << pExeName
        << " <Country Name> <Initial Population> [<Number of Ticks>]"
        << endl;
}

int hw_main(int argc, char *argv[])
{
   const char* exeName = argv[0];         // Name of executable process
   char* const fedExecName = "HelloWorld"; // Name of the Federation Execution
   Tile*    myTile = NULL;          // Pointer to Federate's Tile
   Tile*    mySecondTile = NULL;    //Pointer to another tile
   int         numberOfTicks( 100 );

   int loopCounter = 0; 

   //------------------------------------------------------
   // Make sure executable is provided with correct number
   // of arguments.
   //------------------------------------------------------
   if (argc < 4)
   {
      printUsage( exeName );
      return -1;
   }
   else if ( !argv[1] && !argv[2] && (argc == 3 || !argv[3]) )
   {
      printUsage( exeName );
      return -1;
   }
   else
   {
      myTile = new Tile( argv[1], atoi(argv[4]));
      mySecondTile = new Tile (argv[1], atoi(argv[4]));

      if ( argc > 3 )
      {
         numberOfTicks = atoi( argv[3] );
      }
   }

   try
   {
      //------------------------------------------------------
      // Create RTI objects
      //
      // The federate communicates to the RTI through the RTIambassador
      // object and the RTI communicates back to the federate through
      // the FederateAmbassador object.
      //------------------------------------------------------
      RTI::RTIambassador       rtiAmb;         // libRTI provided
      TwFederateAmbassador     fedAmb;         // User-defined

      // Named value placeholder for the federates handle -
      // we don't really use this in HelloWorld but some
      // services and MOM might need it - if we were to use them.
      RTI::FederateHandle      federateId;

      //------------------------------------------------------
      // Create federation execution.
      //
      // The RTI_CONFIG environment variable must be set in the 
      // shell's environment to the directory that contains
      // the RTI.rid file and the HelloWorld.fed
      //      
      // In RTI 1.3, when a federate creates the federation 
      // execution the $RTI_HOME/bin/$RTI_ARCH/fedex.sh process
      // is executed on the localhost. Therefore, the RTI_HOME 
      // environment variable must be set to the root of the 
      // RTI 1.0 distribution tree in the federate environment.
      //------------------------------------------------------
      try
      {
         //------------------------------------------------------
         // A successful createFederationExecution will cause
         // the fedex process to be executed on this machine.
         //
         // A "HelloWorld.fed" file must exist in the current
         // directory. This file specifies the FOM
         // object and interaction class structures as well as 
         // default/initial transport and ordering information for
         // object attributes and interaction classes.
         //------------------------------------------------------
         cout << "FED_HW: CREATING FEDERATION EXECUTION" << endl;
         rtiAmb.createFederationExecution( fedExecName, "HelloWorld.fed" ); 
         cout << "FED_HW: SUCCESSFUL CREATE FEDERATION EXECUTION" << endl;
      }
      catch ( RTI::FederationExecutionAlreadyExists& e )
      {
         cerr << "FED_HW: Note: Federation execution already exists." <<
          &e << endl;
      }
      catch ( RTI::Exception& e )
      {
         cerr << "FED_HW: ERROR:" << &e << endl;
         return -1;
      }

#if defined(WIN32)
      struct _timeb tb;
      _ftime(&tb);
      
      cout << myTile->GetName() << " " << tb.time << " " << 
       tb.millitm * 1000 << " START" << endl;
#elif defined(VXWORKS)
      struct timespec tp;
      clock_gettime(CLOCK_REALTIME, &tp);
      
      cout << myTile->GetName() << " " << tp.tv_sec << " " <<
       tp.tv_nsec / 1000L << " START" << endl;
#else
      static struct timeval tp;
      gettimeofday(&tp, NULL);

      cout << myTile->GetName() << " "
           << tp.tv_sec << " "
           << tp.tv_usec << " "
           << "START\n";
#endif

      RTI::Boolean Joined    = RTI::RTI_FALSE;
      int          numTries  = 0;

      //------------------------------------------------------
      // Here we loop around the joinFederationExecution call
      // until we try to many times or the Join is successful.
      // 
      // The federate that successfully CREATES the federation
      // execution will get to the join call before the 
      // FedExec is initializes and will be unsuccessful at
      // JOIN call.  In this loop we catch the
      // FederationExecutionDoesNotExist exception to
      // determine that the FedExec is not initialized and to
      // keep trying. If the JOIN call does not throw an
      // exception then we set Joined to TRUE and that will
      // cause us to exit the loop anf proceed in the execution.
      //------------------------------------------------------
      while( !Joined && (numTries++ < 20) )
      {

         //------------------------------------------------------
         // Join the named federation execution as the named
         // federate type.  Federate types (2nd argument to
         // joinFederationExecution) does not have to be unique
         // in a federation execution; however, the save/restore
         // services use this information but we are not doing
         // save/restore in HelloWorld so we won't worry about it
         // here (best to make the names unique if you do
         // save/restore unless you understand how save/restore
         // will use the information
         //
         //------------------------------------------------------
         try
         {
            cout << "FED_HW: JOINING FEDERATION EXECUTION: " << exeName << endl;

            federateId = rtiAmb.joinFederationExecution( myTile->GetName(),
                                                         fedExecName, 
                                                         &fedAmb);
            Joined = RTI::RTI_TRUE; 
         }
         catch (RTI::FederateAlreadyExecutionMember& e)
         {
            cerr << "FED_HW: ERROR: " << myTile->GetName()
                 << " already exists in the Federation Execution "
                 << fedExecName << "." << endl;
            cerr << &e << endl;
            return -1;
         }
         catch (RTI::FederationExecutionDoesNotExist&)
         {
            cerr << "FED_HW: ERROR: " << fedExecName << " Federation Execution "
                 << "does not exists."<< endl;
	    rtiAmb.tick(2.0, 2.0);
         }
         catch ( RTI::Exception& e )
         {
            cerr << "FED_HW: ERROR:" << &e << endl;
            return -1;
         }
      } // end of while

      cout << "FED_HW: JOINED SUCCESSFULLY: " << exeName 
           << ": Federate Handle = " << federateId << endl;

      //------------------------------------------------------
      // The Country class needs to determine what the RTI is
      // going to call its class type and its attribute's types.
      //
      // This is stored globally even though it is
      // theoretically possible for a federate to join more
      // than one Federation Execution and thus
      // possibly have more than one Run-Time mapping.
      //
      // Note: This has not been tested and may not work.
      //       This would require having an RTIambassador for
      //       each FederationExecution.
      //------------------------------------------------------
      Tile::Init( &rtiAmb );


      //------------------------------------------------------
      // Publication/Subscription
      //
      // Declare my interests to the RTI for the object and
      // interaction data types I want to receive.  Also tell
      // the RTI the types of data I can produce.
      //
      // Note: In publication I am telling the RTI the type
      // of data I CAN produce not that I necessarily will.
      // In this program we will create all data types that
      // are published however in more advance applications
      // this convention allows migration of object &
      // attributes to other simulations as neccessary
      // through the Ownership Management services.
      //
      // NOTE: Each time an object or interaction class is
      //       subscribed or published it replaces the previous
      //       subscription/publication for that class.
      //------------------------------------------------------
      myTile->PublishAndSubscribe();
      mySecondTile->PublishAndSubscribe();

      //------------------------------------------------------
      // Register my object with the federation execution.
      // This requires invoking the registerObject service 
      // which returns an HLA object handle. 
      //------------------------------------------------------
      myTile->Register();
      mySecondTile->Register();

      // Set time step to 10
      const RTIfedTime timeStep(10.0);

      timeAdvGrant = RTI::RTI_FALSE;

      try
      {
         cout << "FED_HW: ENABLING ASYNCHRONOUS DELIVERY" << endl;
         //------------------------------------------------------
         // Turn on asynchronous delivery of receive ordered
         // messages. This will allow us to receive messages that
         // are not TimeStamp Ordered outside of a time
         // advancement.
         //------------------------------------------------------
         rtiAmb.enableAsynchronousDelivery();
      }
      catch ( RTI::Exception& e )
      {
         cerr << "FED_HW: ERROR:" << &e << endl;
      }

      //------------------------------------------------------
      // Set the Time Management parameters
      //
      // This version of HelloWorld operates as a time-stepped
      // simulation.  This means that it should be constrained
      // and regulating.
      //
      // In the 1.0 version of HelloWorld this section of code
      // was before the publication and subscription.  In 1.3
      // enableTimeConstrained and enableTimeRegulation 
      //------------------------------------------------------
      try
      {
         cout << "FED_HW: ENABLING TIME CONTRAINT" << endl;
         //------------------------------------------------------
         // Turn on constrained status so that regulating
         // federates will control our advancement in time.
         //
         // If we are constrained and sending fderates specify
         // the Country attributes and Communication interaction
         // with timestamp in the HelloWorld.fed file we will
         // receive TimeStamp Ordered messages.
         //------------------------------------------------------
         rtiAmb.enableTimeConstrained();
         timeAdvGrant = RTI::RTI_FALSE;
	 TimeConstrained = RTI::RTI_FALSE;

         //------------------------------------------------------
         // Tick the RTI until we gwt the timeConstrainedEnabled
         // callback with my current time.
         //------------------------------------------------------
         while ( !TimeConstrained )
         {
	   rtiAmb.tick(1.0, 1.1);
         }
      }
      catch ( RTI::Exception& e )
      {
         cerr << "FED_HW: ERROR:" << &e << endl;
      }

      try
      {
         cout << "FED_HW: ENABLING TIME REGULATION WITH LOOKAHEAD = "
              << Tile::GetLookahead() << endl;
         //------------------------------------------------------
         // Turn on regulating status so that constrained
         // federates will be controlled by our time.
         //
         // If we are regulating and our Country attributes and
         // Communication interaction are specified with timestamp
         // in the HelloWorld.fed file we will send TimeStamp
         // Ordered messages.
         //------------------------------------------------------
         rtiAmb.enableTimeRegulation( grantTime, Tile::GetLookahead());

         //------------------------------------------------------
         // enableTimeRegulation is an implicit timeAdvanceRequest
         // so set timeAdvGrant to TRUE since we will get a
         // timeRegulationEnabled which is an implicit 
         // timeAdvanceGrant
         //------------------------------------------------------
         timeAdvGrant = RTI::RTI_FALSE;
	 TimeRegulation = RTI::RTI_FALSE;

         //------------------------------------------------------
         // Tick the RTI until we gwt the timeRegulationEnabled
         // callback with my current time.
         //------------------------------------------------------
         while ( !TimeRegulation )
         {
	   rtiAmb.tick(1.0, 1.1);
         }
      }
      catch ( RTI::Exception& e )
      {
         cerr << "FED_HW: ERROR:" << &e << endl;
      }

      //------------------------------------------------------
      // Event Loop
      // ----------
      // 
      // 1.) Calculate current state and update to RTI.
      // 2.) Ask for a time advance.
      // 3.) Tick the RTI waiting for the grant and process all
      //     RTI initiated services (especially reflections).
      // 4.) Repeat.
      //------------------------------------------------------
      int counter = 0;

      while ( counter++ < numberOfTicks-1 )
      {
         cout << "FED_HW: " << endl;
         cout << "FED_HW: HelloWorld Event Loop Iteration #: " << counter << endl; 

         //------------------------------------------------------
         // 1.) - Update current state
         //------------------------------------------------------
         myTile->Update( grantTime );
	 mySecondTile->Update( grantTime );

         // Print state of all countries
         Tile* pTile = NULL;
         for ( unsigned int i = 0; i < Tile::ms_extentCardinality; i++ )
         {
            pTile = Tile::ms_countryExtent[ i ];

            if ( pTile )
            {
               cout << "FED_HW: Tile[" << i << "] " << pTile << endl;
            }
         }

         //------------------------------------------------------
         // 2.) - Ask for a time advance.
         //------------------------------------------------------
	 if(RTI::RTI_FALSE)
	   {
	     try
	       {
		 RTIfedTime requestTime(timeStep.getTime());
		 requestTime += grantTime;
		 timeAdvGrant = RTI::RTI_FALSE;
		 loopCounter++;
		 rtiAmb.timeAdvanceRequest( requestTime );
	       }
	     catch ( RTI::Exception& e )
	       {
		 cerr << "FED_HW: ERROR: " << &e << endl;
	       }
	   }
	 else
	   {
	     //------------------------------------------------------
	     // 2.) - Ask for a next event request advance.
	     //------------------------------------------------------
	     try
	       {
		 RTIfedTime requestTime(timeStep.getTime());
		 requestTime += grantTime;
		 timeAdvGrant = RTI::RTI_FALSE;
		 loopCounter = 0;
		 rtiAmb.nextEventRequest( requestTime );
	       }
	     catch ( RTI::Exception& e )
	       {
		 cerr << "FED_HW: ERROR: " << &e << endl;
	       }
	   }

         //------------------------------------------------------
         // 3.) Tick the RTI waiting for the grant and process all
         //     RTI initiated services (especially reflections).
         //------------------------------------------------------
         while( timeAdvGrant != RTI::RTI_TRUE )
         {
            //------------------------------------------------------
            // Tick will turn control over to the RTI so that it can
            // process an event.  This will cause an invocation of one
            // of the federateAmbassadorServices methods.
            //
            // Be sure not to invoke the RTIambassadorServices from the
            // federateAmbassadorServices; otherwise, a ConcurrentAccess
            // exception will be thrown.
            //------------------------------------------------------
	   
	   rtiAmb.tick(1.0, 1.1);
         }
      } // 4.) - Repeat

      if ( myTile )
      {
         // Perform last update - this is necessary to give the
         // Country instance the current granted time otherwise
         // the deleteObjectInstance call that happens in the
         // destructor will have an invalid time since it is in
         // the past. This is a problem with HelloWorld not RTI.
         myTile->Update( grantTime );
	 mySecondTile->Update( grantTime );
         delete myTile;
	 delete mySecondTile;
      }

      try{
	rtiAmb.disableTimeConstrained();
	TimeConstrained = RTI::RTI_FALSE;
      }
      catch(RTI::Exception& e)
      {
	  cerr << "FED_HW:ERR:" << &e << endl;
      }

      try{
	rtiAmb.disableTimeRegulation();
	TimeRegulation = RTI::RTI_FALSE;
      }
      catch(RTI::Exception& e)
      {
	  cerr << "FED_HW: ERROR:" << &e << endl;
      }


      //------------------------------------------------------
      // Resign from the federation execution to remove this
      // federate from participation.  The flag provided
      // will instruct the RTI to call deleteObjectInstance
      // for all objects this federate has privilegeToDelete
      // for (which by default is all objects that this federate
      // registered) and to release ownership of any attributes
      // that this federate owns but does not own the
      // privilefeToDelete for.
      //------------------------------------------------------
      try
      {
         cout << "FED_HW: RESIGN FEDERATION EXECUTION CALLED" << endl;
         
         rtiAmb.resignFederationExecution(
                   RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES ); 
         cout << "FED_HW: SUCCESSFUL RESIGN FEDERATION EXECUTION CALLED" << endl;
      }
      catch ( RTI::Exception& e )
      {
         cerr << "FED_HW: ERROR:" << &e << endl;
         return -1;
      }

      //------------------------------------------------------
      // Destroy the federation execution in case we are the
      // last federate. This will not do anything bad if there
      // other federates joined.  The RTI will throw us an
      // exception telling us that other federates are joined
      // and we can just ignore that.
      //------------------------------------------------------
      try
      {
         cout << "FED_HW: DESTROY FEDERATION EXECUTION CALLED" << endl;
         rtiAmb.destroyFederationExecution( fedExecName ); 
         cout << "FED_HW: SUCCESSFUL DESTROY FEDERATION EXECUTION CALLED" << endl;
      }
      catch ( RTI::FederatesCurrentlyJoined& /* e */ )
      {
	cerr << "FED_HW: FederatesCurrentlyJoined" << endl;
	return 0;
      }
      catch ( RTI::FederationExecutionDoesNotExist& /* e */)
      {
	cerr << "FED_HW: FederationExecutionDoesNotExist" << endl;
	return 0;
      }
      catch ( RTI::Exception& e )
      {
         cerr << "FED_HW: ERROR:" << &e << endl;
         return -1;
      }

   }
   catch (RTI::ConcurrentAccessAttempted& e)
   {
      cerr << "FED_HW: ERROR: Concurrent access to the RTI was attemted.\n"
           << "       Exception caught in main() - PROGRAM EXITING.\n"
           << "\n"
           << "Note:  Concurrent access will result from invoking\n"
           << "       RTIambassadorServices within the scope of\n"
           << "       federateAmbassadorService invocations.\n"
           << "\n"
           << "       e.g. RTI calls provideAttributeValueUpdate() and\n"
           << "       within that method you invoke updateAttributeValues\n"
           << endl;
      cerr << &e << endl;
      return -1;
   }
   catch ( RTI::Exception& e )
   {
      cerr << "FED_HW: ERROR:" << &e << endl;
      return -1;
   }

#if defined(WIN32)
   struct _timeb tb;
   _ftime(&tb);
z   
   cout << argv[2] << " " << tb.time << " " << tb.millitm * 1000 << 
           " END" << endl;
#elif defined(VXWORKS)
   struct timespec tp;
   clock_gettime(CLOCK_REALTIME, &tp);
   
   cout << argv[2] << " " << tp.tv_sec << " " << tp.tv_nsec / 1000L 
	<< " START" << endl;
#else
   static struct timeval tp;
   gettimeofday(&tp, NULL);

   cout << argv[2] << " "
        << tp.tv_sec << " "
        << tp.tv_usec << " "
        << "END\n";
#endif

   cout << "FED_HW: Exiting " << exeName << "." << endl;

   return 0;
}


#if defined(VXWORKS)
extern "C" 
{
int
hwmain(char* arg1, char* arg2, char* arg3)
{
  ACE_MAIN_OBJECT_MANAGER;

  char* argv[] = { "hw_main", arg1, arg2, arg3 };
  int argc = 1;
  
  if ( argv[0] )
  {
    argc++;
    if ( argv[1] )
    {
      argc++;
      if ( argv[2] )
      {
	argc++;
      }
    }
  }

  return hw_main(argc, argv);
}
}
#else
int
main(int argc, char** argv)
{
  return hw_main(argc, argv);
}
#endif
