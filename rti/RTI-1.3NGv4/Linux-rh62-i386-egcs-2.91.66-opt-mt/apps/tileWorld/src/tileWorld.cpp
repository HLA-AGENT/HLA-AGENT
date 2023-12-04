#include "tileWorld.hh"

#ifndef _MSC_VER
#include <stdlib.h>
#include <iostream.h>
#include <strings.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#endif

// the following are for ntohl and cvt_ftof
#if defined(_X86_) && !defined(WIN32)
#include <arpa/inet.h>
#endif 

#if defined(__alpha)
#include <arpa/inet.h>
#include <cvt.h>
#endif  // __alpha


/////////////////////////////////////////////////
// What's in this file...
/////////////////////////////////////////////////
// Constructors and Deconstructors
//----------------------------------------------
// Tile::Tile( const char* name, const int& obj )
// Tile::Tile( const char* name, const double& population )
// Tile::Tile(RTI::ObjectHandle id)
// Tile::Tile()
// Tile::~Tile()
//
// Other Methods
//-----------------------------------------------
// Tile* Tile::Find( RTI::ObjectHandle objectId ) - Gets the object corresponding to objectId
// Tile::Init( const RTI::RTIambassador* rtiAmb ) - Get RTI names for attributes
// void Tile::PublishAndSubscribe()               - self explanatory
// void Tile::Register()                          - Creates a new instance
//
// Update Methods
//----------------------------------------------
// void Tile::Update( RTIfedTime& newTime )       
//      - Update and send new values (also send periodic interaction)
// void Tile::Update( const AttributeHandleValuePairSet& theAttributes )
//      - Received new data update local info
// void Tile::Update( RTI::InteractionClassHandle theInteraction,
//                    const RTI::ParameterHandleValuePairSet& theParameters )
//      - Received an interaction (process it)
//
// Set Methods
//----------------------------------------------
// void Tile::Move( const char* )                - changes x,y attributes
// void Tile::SetName( const char* )             
// void Tile::SetPopulation( const double& population)
//
// Creating method
//----------------------------------------------
// RTI::AttributeHandleValuePairSet* Tile::CreateNVPSet() - Creates a Name Value Pair
//                                                          set of attributes that
//                                                          have changed.
//
// Operator overloads
//-----------------------------------------------
// ostream &operator << ( ostream &s, Country &v )
// ostream &operator << ( ostream &s, Country *v )

//-----------------------------------------------------------------
// Static variable definition
//-----------------------------------------------------------------
RTI::RTIambassador*          Tile::ms_rtiAmb        = NULL;
RTI::ObjectClassHandle       Tile::ms_TileTypeId = 0;
RTI::AttributeHandle         Tile::ms_nameTypeId    = 0;
RTI::AttributeHandle         Tile::ms_popTypeId     = 0;
RTI::AttributeHandle         Tile::ms_xposTypeId     = 0;
RTI::AttributeHandle         Tile::ms_yposTypeId     = 0;
RTI::AttributeHandle         Tile::ms_objectTypeId = 0;
RTI::InteractionClassHandle  Tile::ms_commTypeId    = 0;
RTI::ParameterHandle         Tile::ms_commMsgTypeId = 0;

RTIfedTime                Tile::ms_lookahead(1.0);

char* const  Tile::ms_TileTypeStr = "Tile";       
char* const  Tile::ms_nameTypeStr = "Name" ;
char* const  Tile::ms_popTypeStr = "Population" ;       
char* const  Tile::ms_xposTypeStr = "Xpos" ; 
char* const  Tile::ms_yposTypeStr = "Ypos" ; 
char* const  Tile::ms_objectTypeStr = "Objtype" ;
char* const  Tile::ms_commTypeStr = "Communication";
char* const  Tile::ms_commMsgTypeStr = "Message";

const double Tile::ms_growthRate(0.001); 

// This is bad form - The size of the array is based on a compile
// directive but the initialization of the array is hard coded for
// a specific number of elements.  This will do - it's not worth
// going through the hassle of creating a dynamic array of pointers.
Tile*   Tile::ms_countryExtent[ MAX_FEDERATE + 1 ] = 
{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
  
unsigned int Tile::ms_extentCardinality = 0;

// Non-Class Constant
const double countryDefaultPopulation = 100;
const int defaultx = 1;
const int defaulty = 1;
const int defaultobject = 1;  //1=tile, 2=hole, 3=obstacle


//-----------------------------------------------------------------
//
// METHOD:
//     Tile::Tile( const char* name, const int& obj )
//
// PURPOSE:
//     Constructor.  The constructor initializes the member data
//     with the values passed in and adds this Tile instance to
//     the Country extenet (collection of all elements of a type).
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------

Tile::Tile( const char* name, const int& obj )
   : m_name(NULL),
     m_lastTime(*(RTI::FedTimeFactory::makeZero())),
     m_TimePlusLookahead(*(RTI::FedTimeFactory::makeZero()))
{
   //-----------------------------------------------------------------
   // Add this new object to the extent (collection) of all
   // Country instances.
   //-----------------------------------------------------------------
   Tile::ms_countryExtent[ Tile::ms_extentCardinality++ ] = this;

   this->SetName( name );
   this->SetPopulation( countryDefaultPopulation );
   this->SetXpos( defaultx );
   this->SetYpos( defaulty );
   this->SetObject( obj );
}

//-----------------------------------------------------------------
//
// METHOD:
//     Tile::Tile( const char* name, const double& population )
//
// PURPOSE:
//     Constructor.  The constructor initializes the member data
//     with the values passed in and adds this Tile instance to
//     the Country extenet (collection of all elements of a type).
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------

Tile::Tile( const char* name, const double& population )
   : m_name(NULL),
     m_lastTime(*(RTI::FedTimeFactory::makeZero())),
     m_TimePlusLookahead(*(RTI::FedTimeFactory::makeZero()))
{
   //-----------------------------------------------------------------
   // Add this new object to the extent (collection) of all
   // Country instances.
   //-----------------------------------------------------------------
   Tile::ms_countryExtent[ Tile::ms_extentCardinality++ ] = this;

   this->SetName( name );
   this->SetPopulation( population );
   this->SetXpos( defaultx );
   this->SetYpos( defaulty );
   this->SetObject( defaultobject );
}

//-----------------------------------------------------------------
//
// METHOD:
//     Tile::Tile(RTI::ObjectHandle id)
//
// PURPOSE:
//     Constructor.  The constructor initializes the member data
//     with the default values and adds this Tile instance to
//     the Tile extent (collection of all elements of a type).
//
//     This constructor is used when a remote object is discovered.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
Tile::Tile( RTI::ObjectHandle  id)
   : m_name(NULL),
     m_instanceId(id),
     m_lastTime(*(RTI::FedTimeFactory::makeZero())),
     m_TimePlusLookahead(*(RTI::FedTimeFactory::makeZero()))
{
   //-----------------------------------------------------------------
   // Add this new object to the extent (collection) of all
   // Tile instances.
   //-----------------------------------------------------------------
   Tile::ms_countryExtent[ Tile::ms_extentCardinality++ ] = this;

   this->SetName( NULL );
   this->SetXpos( defaultx );
   this->SetYpos( defaulty );
   this->SetPopulation( countryDefaultPopulation ); // default population
   this->SetObject( defaultobject );
}

//-----------------------------------------------------------------
//
// METHOD:
//     Tile::Tile()
//
// PURPOSE:
//     Constructor.  The constructor initializes the member data
//     with the default values and adds this Tile instance to
//     the Tile extenet (collection of all elements of a type).
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
Tile::Tile()
   : m_name(NULL),
     m_lastTime(*(RTI::FedTimeFactory::makeZero())),
     m_TimePlusLookahead(*(RTI::FedTimeFactory::makeZero()))
{
   //-----------------------------------------------------------------
   // Add this new object to the extent (collection) of all
   // Tile instances.
   //-----------------------------------------------------------------
   Tile::ms_countryExtent[ Tile::ms_extentCardinality++ ] = this;

   this->SetName( NULL );
   this->SetXpos( defaultx );
   this->SetYpos( defaulty );
   this->SetPopulation( countryDefaultPopulation ); // default population
   this->SetObject( defaultobject );
}

//-----------------------------------------------------------------
//
// METHOD:
//     Tile::~Tile()
//
// PURPOSE:
//     Virtual destructor. Frees memory allocated for m_name and
//     removes this instance from the extent.
//
//     Note: Removing an element from the extent causes the array
//           to be collapsed so as to not have any gaps.  Some
//           elements will have new indices within the extent
//           after this occurs.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
Tile::~Tile()
{
   Tile *pTile = NULL;
   unsigned int ndx = 0;

   //-----------------------------------------------------------------
   // Find the position in the extent for this instance.  The
   // zero'th position always hold the local instance.
   //-----------------------------------------------------------------
   for ( ndx = 0; ndx < Tile::ms_extentCardinality; ndx++ )
   {
      pTile = Tile::ms_countryExtent[ ndx ];

      if ( pTile && pTile->GetInstanceId() == this->GetInstanceId() )
      {
         break;
      }
   }

   if ( pTile )
   {
      //-----------------------------------------------------------------
      // First thing to do is move the rest of the Tile objects
      // one position up in the collection and then reduce the
      // cardinality by one.
      //-----------------------------------------------------------------
      for ( unsigned int i=ndx; (i < Tile::ms_extentCardinality)
                       && (Tile::ms_countryExtent[ i ] != NULL); i++ )
      {
         Tile::ms_countryExtent[ i ] = Tile::ms_countryExtent[ i+1 ];
      }

      Tile::ms_extentCardinality = Tile::ms_extentCardinality - 1;

      //-----------------------------------------------------------------
      // If the instance was found in the 0th position then this is the
      // local Tile instance so we should delete it from the federation
      // execution.
      //-----------------------------------------------------------------
      if ( ms_rtiAmb && ndx==0 )
      {
         //-----------------------------------------------------------------
         // this call returns an event retraction handle but we don't
         // support event retraction so no need to store it.
         //-----------------------------------------------------------------
         (void) ms_rtiAmb->deleteObjectInstance( this->GetInstanceId(),
                                                 this->GetLastTimePlusLookahead(),
                                                 NULL );
      }
      else
      {
         //-----------------------------------------------------------------
         // Otherwise, this is a remote object that removeObject was called
         // on.
         //-----------------------------------------------------------------
         //We don't need to do anything here 
      }
   }
 
   delete [] m_name;
}

//-----------------------------------------------------------------
//
// METHOD:
//     Tile* Tile::Find( RTI::ObjectHandle objectId )
//
// PURPOSE:
//     Looks through the extent to find the Country instance
//     with the passed in object Id.
//
// RETURN VALUES:
//     Pointer to country object that has the specified
//     ObjectHandle. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
Tile* Tile::Find( RTI::ObjectHandle objectId )
{
   Tile *pTile = NULL;

   for ( unsigned int i = 0; i < Tile::ms_extentCardinality; i++ )
   {
      pTile = Tile::ms_countryExtent[ i ];

      if ( pTile && pTile->GetInstanceId() == objectId ) {
	break;
      } else {
	pTile = NULL;
      }
   }

   return pTile;
}

//-----------------------------------------------------------------
//
// METHOD:
//     Tile::Init( const RTI::RTIambassador* rtiAmb )
//
// PURPOSE:
//     Sets the member data containing the RTI run-time type
//     ids for Tile's class and attributes.  Stores the
//     rtiAmb pointer for updating state etc.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::Init( RTI::RTIambassador* rtiAmb )
{
   ms_rtiAmb = rtiAmb;

   if ( ms_rtiAmb )
   {
      //------------------------------------------------------
      // Get the RTTI (Meta-Object Protocol MOP) handles
      //
      // Since the 1.0 RTI does not know anything about your data
      // and thus uses Run-Time Type Identification we must ask the
      // RTI what to call each of our data elements.
      //
      //------------------------------------------------------
      ms_TileTypeId  = ms_rtiAmb->getObjectClassHandle(ms_TileTypeStr);
      ms_nameTypeId     = ms_rtiAmb->getAttributeHandle( ms_nameTypeStr,
                                                         ms_TileTypeId);
      ms_popTypeId      = ms_rtiAmb->getAttributeHandle( ms_popTypeStr,
                                                         ms_TileTypeId);
      ms_xposTypeId      = ms_rtiAmb->getAttributeHandle( ms_xposTypeStr,
                                                         ms_TileTypeId);
      ms_yposTypeId      = ms_rtiAmb->getAttributeHandle( ms_yposTypeStr,
                                                         ms_TileTypeId);
      ms_objectTypeId    = ms_rtiAmb->getAttributeHandle( ms_objectTypeStr,
                                                         ms_TileTypeId);
   }
}

//-----------------------------------------------------------------
//
// METHOD:
//     void Tile::PublishAndSubscribe()
//
// PURPOSE:
//     Publishes and Subscribes to Object & Interaction classes
//     and their member data.
//
//     Note: In most reasonable applications this would be broken
//           up into 2 different methods.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::PublishAndSubscribe()
{
   if ( ms_rtiAmb )
   {
      //------------------------------------------------------
      // To actually subscribe and publish we need to build
      // an AttributeHandleSet that contains a list of
      // attribute type ids (AttributeHandle).
      //------------------------------------------------------
      RTI::AttributeHandleSet *tileAttributes;
      tileAttributes = RTI::AttributeHandleSetFactory::create(5); //create(x) x=number of attributes

      tileAttributes->add( ms_nameTypeId );
      tileAttributes->add( ms_popTypeId );
      tileAttributes->add( ms_xposTypeId );
      tileAttributes->add( ms_yposTypeId );
      tileAttributes->add( ms_objectTypeId );

      ms_rtiAmb->subscribeObjectClassAttributes( ms_TileTypeId,
                                                *tileAttributes );
      ms_rtiAmb->publishObjectClass( ms_TileTypeId,
                                     *tileAttributes);

      tileAttributes->empty();

      delete tileAttributes;   // Deallocate the memory

      //------------------------------------------------------
      // Same as above for interactions
      //------------------------------------------------------

      // Get RTTI info
      ms_commTypeId    = ms_rtiAmb->getInteractionClassHandle( ms_commTypeStr );

      ms_commMsgTypeId = ms_rtiAmb->getParameterHandle( ms_commMsgTypeStr,
                                                        ms_commTypeId);

      // Declare my Interaction interests
      ms_rtiAmb->subscribeInteractionClass( ms_commTypeId );
      ms_rtiAmb->publishInteractionClass( ms_commTypeId );
   }
}

//-----------------------------------------------------------------
//
// METHOD:
//     void Tile::Register()
//
// PURPOSE:
//     Creates an HLA object instance and registers it with the RTI.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::Register()
{
   if ( ms_rtiAmb )
   {
      //---------------------------------------------------------
      // Register my tile object with the RTI.  Registering
      // an object with the RTI allows the object to be 
      // discovered by other federates in the federation
      // execution.
      //
      // Note: Discovery happens after an object is registered
      //       not after the initial update like in the 1.0 RTI.
      //---------------------------------------------------------
      m_instanceId =
         ms_rtiAmb->registerObjectInstance( this->GetTileRtiId() );
   }
}

//-----------------------------------------------------------------
//
// METHOD:
//     void Tile::Update( RTIfedTime& newTime )
//
// PURPOSE:
//     Update the state of the Tile's population based on
//     the new time value.  The deltaTime is calculated based
//     on the last time the Tile object was updated and
//     the newTime passed in.  The deltaTime is multiplied by
//     the growth rate and current population to determine the
//     number of births in the deltaTime.  The population is
//     increased by the number of births.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::Update( RTI::FedTime& newTime )
{
   //------------------------------------------------------
   // we have advanced in time so calculate my next state.
   //------------------------------------------------------

   RTI::FedTime *pTime = RTI::FedTimeFactory::makeZero();
   (*pTime) = newTime;
   (*pTime) -= this->GetLastTime();
   // Set last time to new time
   this->SetLastTime( newTime );

   if ( !(pTime->isZero()))
   {
      SetPopulation( GetPopulation() + 
                     (GetPopulation()*ms_growthRate) );
      Move(GetXpos()+1,GetYpos()+1);
      //      SetXpos(GetXpos()+1);
      //      SetYpos(GetYpos()+1);
   }

   if ( ms_rtiAmb )
   {
      //------------------------------------------------------
      // Update state of country
      //------------------------------------------------------
      try
      {
         //------------------------------------------------------
         // In order to send the values of our attributes, we must
         // construct an AttributeHandleValuePairSet (AHVPS) which
         // is a set comprised of attribute handles, values, and
         // the size of the values. CreateNVPSet() is a method 
         // defined on the Tile class - it is not part of the RTI.
         // Look inside the method to see how to construct an AHVPS
         //------------------------------------------------------
         RTI::AttributeHandleValuePairSet* pNvpSet = this->CreateNVPSet();
         
         //------------------------------------------------------
         // Send the AHVPS to the federation.
         //
         // this call returns an event retraction handle but we 
         // don't support event retraction so no need to store it.
         //------------------------------------------------------
         (void) ms_rtiAmb->updateAttributeValues( this->GetInstanceId(),
                                                  *pNvpSet,
                                                  this->GetLastTimePlusLookahead(), 
                                                  NULL );
         // Must free the memory
         pNvpSet->empty();
         delete pNvpSet;
      }
      catch ( RTI::Exception& e )
      {
         cerr << "FED_HW: Error:" << &e << endl;
      }
      
      // Periodically send an interaction to tell everyone Hello
      static int periodicMessage = 0;

      if ( (periodicMessage++%100) == 0 )
      {
         RTI::ParameterHandleValuePairSet* pParams = NULL;

                 //------------------------------------------------------
                 // Periodically stimulate an update of the "Name" 
                 // attribute for the benefit of late-arriving federates.
                 // It would be more correct to use 
                 // "requestClassAttributeValueUpdate" and 
                 // "provideAttributeValueUpdate", but let's keep things
                 // simple.
                 //------------------------------------------------------
         hasNameChanged = RTI::RTI_TRUE;

         //------------------------------------------------------
         // Set up the data structure required to push this
         // objects's state to the RTI.  The
         // ParameterHandleValuePairSet is similar to the AHVPS
         // except it contains ParameterHandles instead of
         // AttributeHandles.
         //------------------------------------------------------
         pParams = RTI::ParameterSetFactory::create( 1 );

         char *pMessage = "Hello World!";
         
         pParams->add( this->GetMessageRtiId(),
                       (char*) pMessage,
                       ((strlen(pMessage)+1)*sizeof(char)) );
         try
         {
            //------------------------------------------------------
            // this call returns an event retraction handle but we 
            // don't support event retraction so no need to store it.
            //------------------------------------------------------
            (void) ms_rtiAmb->sendInteraction( GetCommRtiId(), *pParams,
                                               this->GetLastTimePlusLookahead(),
                                               NULL );
         }
         catch ( RTI::Exception& e )
         {
            cerr << "FED_HW: Error:" << &e << endl;
         }

         //------------------------------------------------------
         // Must free the memory:
         //    ParameterSetFactory::create() allocates memory on
         //    the heap.
         //------------------------------------------------------
         delete pParams;
      }
   }
   delete pTime;
}

//-----------------------------------------------------------------
//
// METHOD:
//     void Tile::Update( const AttributeHandleValuePairSet& theAttributes )
//
// PURPOSE:
//     Update the new attribute values.  If this is being called
//     then this object is either a remote object that was
//     discovered or it has some attributes that are owned by
//     another federate.
//
//     Note: This version does not implement any ownership
//           transfer - the above was a generic statement.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::Update( const RTI::AttributeHandleValuePairSet& theAttributes )
{
   RTI::AttributeHandle attrHandle;
   RTI::ULong           valueLength;

   // We need to iterate through the AttributeHandleValuePairSet
   // to extract each AttributeHandleValuePair.  Based on the type
   // specified ( the value returned by getHandle() ) we need to
   // extract the data from the buffer that is returned by 
   // getValue().
   for ( unsigned int i = 0; i < theAttributes.size(); i++ )
   {
      attrHandle = theAttributes.getHandle( i );

      if ( attrHandle == Tile::GetPopulationRtiId() )
      {
         // When we run this over multiple platforms we will have
         // a problem with different endian-ness of platforms. Either
         // we need to encode the data using something like XDR or
         // provide another mechanism.
         double population;
         theAttributes.getValue( i, (char*)&population, valueLength );

#if defined(_X86_) || defined(i386)
         long x = ntohl(*(long*)&population);
         *(long*)&population = ntohl(* (((long*)&population) + 1));
         *(((long*)&population)+1) = x;
#elif defined(__alpha)
         double x;
         cvt_ftof(&population, CVT_IEEE_T, &x, CVT_BIG_ENDIAN_IEEE_T, 0);
         population = x;
#endif  // __alpha

         SetPopulation( (double)population );
      }
      else if ( attrHandle == Tile::GetNameRtiId() )
      {
        // Same as above goes here...
         char name[ 1024 ];
         theAttributes.getValue( i, (char*)name, valueLength );
         SetName( (const char*)name );
      }
      else if ( attrHandle == Tile::GetXposRtiId() )
      {
	int xpos;
	theAttributes.getValue(i,(char*)&xpos, valueLength);
	SetXpos( (int)xpos);
      }
      else if ( attrHandle == Tile::GetYposRtiId() )
      {
	int ypos;
	theAttributes.getValue(i,(char*)&ypos, valueLength);
	SetYpos( (int)ypos);
      }
      else if ( attrHandle == Tile::GetObjectRtiId() )
      {
	int obj;
	theAttributes.getValue(i,(char*)&obj, valueLength);
	SetObject( (int)obj);
      }
   }
}

//-----------------------------------------------------------------
//
// METHOD:
//     void Tile::Update( RTI::InteractionClassHandle theInteraction,
//                    const RTI::ParameterHandleValuePairSet& theParameters )
//
// PURPOSE:
//     Process an interaction.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::Update( RTI::InteractionClassHandle theInteraction,
             const RTI::ParameterHandleValuePairSet& theParameters )
{
   if ( theInteraction == Tile::GetCommRtiId() )
   {
      RTI::ParameterHandle paramHandle;
      RTI::ULong           valueLength;

      // We need to iterate through the AttributeHandleValuePairSet
      // to extract each AttributeHandleValuePair.  Based on the type
      // specified ( the value returned by getHandle() ) we need to
      // extract the data frlom the buffer that is returned by 
      // getValue().
      for ( unsigned int i = 0; i < theParameters.size(); i++ )
      {
         paramHandle = theParameters.getHandle( i );
         if ( paramHandle == Tile::GetMessageRtiId() )
         {
            // When we run this over multiple platforms we will have
            // a problem with different endian-ness of platforms. Either
            // we need to encode the data using something like XDR or
            // provide another mechanism.
            char msg[ 1024 ];
            theParameters.getValue( i, (char*)msg, valueLength );
            cout << "FED_HW: Interaction Received: " << msg << endl;
         }
         else
         {
            // There must be an error since there should only be
            // one parameter to Communication.
            cerr << "FED_HW: Error: I seem to have received a parameter for "
                 << "interaction class Communication that I don't "
                 << "know about." << endl;
         }
      }
   }
   else
   {
      cerr << "FED_HW: Recieved an interaction class I don't know about." << endl;   
   }
}
 //-----------------------------------------------------------------
//
// METHOD:
//     void Tile::Move( const char* )
//
// PURPOSE:
//     Moves the tile to a new location
//     
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::Move( const int& x, const int& y)
{
  SetXpos(x);
  SetYpos(y);
}            
 
//-----------------------------------------------------------------
//
// METHOD:
//     void Tile::SetName( const char* )
//
// PURPOSE:
//     Sets the new value of the name attribute and sets the
//     changed flag to true.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::SetName( const char* name)
{
   if (m_name != name)
   {
     // Delete the existing memory
     delete [] m_name;
     
     // Allocate appropriate size string and copy data
     if ( name && strlen( name ) > 0 )
     {
       m_name = new char[ strlen(name) + 1 ];
       strcpy( m_name, name );
     }
     else
     {
       m_name = NULL;
     }
   }

   // Set flag so that when Update( FederateTime ) is called
   // we send this new value to the RTI.
   hasNameChanged = RTI::RTI_TRUE;
}

//-----------------------------------------------------------------
//
// METHOD:
//     void Country::SetPopulation( const double& population)
//
// PURPOSE:
//     Sets the new value of the population attribute and sets the
//     changed flag to true.
//
// RETURN VALUES:
//     None. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
void Tile::SetPopulation( const double& population )
{
   m_population = population;

   // Set flag so that when Update( FederateTime ) is called
   // we send this new value to the RTI.
   hasPopulationChanged = RTI::RTI_TRUE;
}
void Tile::SetXpos( const int& xpos )
{
   m_xpos = xpos;

   // Set flag so that when Update( FederateTime ) is called
   // we send this new value to the RTI.
   hasXposChanged = RTI::RTI_TRUE;
}
void Tile::SetYpos( const int& ypos )
{
   m_ypos = ypos;

   // Set flag so that when Update( FederateTime ) is called
   // we send this new value to the RTI.
   hasYposChanged = RTI::RTI_TRUE;
}

void Tile::SetObject( const int& obj )
{
   m_object = obj;

   // Set flag so that when Update( FederateTime ) is called
   // we send this new value to the RTI.
   hasObjectChanged = RTI::RTI_TRUE;
}

//-----------------------------------------------------------------
//
// METHOD:
//     RTI::AttributeHandleValuePairSet* Tile::CreateNVPSet()
//
// PURPOSE:
//     Create a name value pair set (aka handle value pair) for
//     the changed attributes of this country object.
//
// RETURN VALUES:
//     RTI::AttributeHandleValuePairSet* containing the
//     attributes that have changed in the instance of Country. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
RTI::AttributeHandleValuePairSet* Tile::CreateNVPSet()
{
   RTI::AttributeHandleValuePairSet* pTileAttributes = NULL;

   // Make sure the RTI Ambassador is set.
   if ( ms_rtiAmb )
   {
      //------------------------------------------------------
      // Set up the data structure required to push this
      // object's state to the RTI.
      //------------------------------------------------------
     pTileAttributes = RTI::AttributeSetFactory::create(5); //changed

      if ( hasNameChanged == RTI::RTI_TRUE )
      {
         // We don't do any encoding here since the name type
         // is a string.
         pTileAttributes->add( this->GetNameRtiId(),
                                  (char*) this->GetName(),
                                  ((strlen(this->GetName())+1)*sizeof(char)) );
      }
      
      if ( hasPopulationChanged == RTI::RTI_TRUE )
      {
         // Here we are encoding the double so that it is in a
         // common format so that federates on other platforms
         // know how to read it.
#if defined(_X86_) || defined(i386)
         double tmp;
         *((long*)&tmp) = htonl(*(((long*)&this->GetPopulation()) + 1));
         *(((long*)&tmp) + 1) = htonl(*((long*)&this->GetPopulation()));
         pTileAttributes->add( this->GetPopulationRtiId(),
                                  (char*)&tmp,
                                  sizeof(double) );
#elif defined(__alpha)
         double x;
         double pop = this->GetPopulation();
         cvt_ftof(&pop, CVT_BIG_ENDIAN_IEEE_T, &x, CVT_IEEE_T, 0);
         pTileAttributes->add( this->GetPopulationRtiId(), 
                                  (char*)&x, sizeof(double));
#else
         pTileAttributes->add( this->GetPopulationRtiId(),
                                  (char*) &this->GetPopulation(),
                                  sizeof(double) );
#endif
      }

       if ( hasXposChanged == RTI::RTI_TRUE )
      {
         pTileAttributes->add( this->GetXposRtiId(),
                                  (char*) &this->GetXpos(),
                                  sizeof(int) );

      }

       if ( hasYposChanged == RTI::RTI_TRUE )
      {
         pTileAttributes->add( this->GetYposRtiId(),
                                  (char*) &this->GetYpos(),
                                  sizeof(int) );
      }
       if ( hasObjectChanged == RTI::RTI_TRUE )
      {
         pTileAttributes->add( this->GetObjectRtiId(),
                                  (char*) &this->GetObject(),
                                  sizeof(int) );
      }
       
   }

   // pCountryAttributes is allocated on the heap and must be
   // deallocated by the federate.
   return pTileAttributes;
}

//-----------------------------------------------------------------
//
// METHOD:
//     ostream &operator << ( ostream &s, Country &v )
//
// PURPOSE:
//     Overloaded stream operator for outputing objects contents in
//     a readable format.
//
// RETURN VALUES:
//     Returns the stream. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
ostream &operator << ( ostream &s, Tile &v )
{
   const char* name = v.GetName();
   if (name == 0)
           name = "(unknown)";

   s << "Name: " << name
     << " Population: " << v.GetPopulation()<<endl
     << "Type: "<<v.GetObject()
     << " X pos: " <<v.GetXpos()
     << " Y pos: " <<v.GetYpos()
     << " Time: " << v.GetLastTime();

   return s;
}

//-----------------------------------------------------------------
//
// METHOD:
//     ostream &operator << ( ostream &s, Country *v )
//
// PURPOSE:
//     Overloaded stream operator for outputing objects contents in
//     a readable format.
//
// RETURN VALUES:
//     Returns the stream. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
ostream &operator << ( ostream &s, Tile *v ) 
{
   if ( !v )
      return s;
   else
   {
      s << *v;
   }

   return s;
}
