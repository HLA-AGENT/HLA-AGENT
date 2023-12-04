#ifndef Country_HEADER
#define Country_HEADER

#if defined(_MSC_VER)
#define RTI_USES_STD_FSTREAM
#endif // defined(_MSC_VER)
#include <RTI.hh>
#if defined(_MSC_VER)
using std::ostream;
#endif // defined(_MSC_VER)
#include <fedtime.hh>

//-----------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------
class Tile;

//-----------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------
typedef Tile* TilePtr;

//-----------------------------------------------------------------
//
// CLASS:
//     Country
//
// PURPOSE:
//     The purpose of instances of class Country is to demonstrate
//     the proper usage of the RTI C++ API.  Instances can update
//     their state on the local machine (actually simulate the 
//     population) or from reflections provided by the RTI
//     (ghosting).
//
//     Note: Neither the interface nor the implementation of
//           Country is intended to be a tutorial on
//           Object-Oriented Analysis & Design.
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
class Tile
{
public:
  Tile();
  Tile( const char* name,int& x, int& y);
  Tile( const char* name, const int& obj );
  Tile( const char* name, const double& population );
  Tile( RTI::ObjectHandle  id);
  virtual ~Tile();
  
  //-----------------------------------------------------------------
  // Methods acting on the RTI
  //-----------------------------------------------------------------
  static Tile*  Find( RTI::ObjectHandle       objectId );
  static void      Init( RTI::RTIambassador* rtiAmb );
  void             PublishAndSubscribe();
  void             Register();
  void             Update( RTI::FedTime& newTime );
  void             Update( const RTI::AttributeHandleValuePairSet& theAttributes );
  static void      Update( RTI::InteractionClassHandle theInteraction,
			   const RTI::ParameterHandleValuePairSet& theParameters );
  
  //-----------------------------------------------------------------
  // Friends of Tile
  //-----------------------------------------------------------------
  friend ostream& operator << ( ostream &s, Tile &v );
  friend ostream& operator << ( ostream &s, Tile *v );
  
  //-----------------------------------------------------------------
  // Accessor Methods
  //-----------------------------------------------------------------
  RTI::FedTime const & GetLastTime()              { return m_lastTime; };
  
  RTI::FedTime const & GetLastTimePlusLookahead() { m_TimePlusLookahead = m_lastTime;
  m_TimePlusLookahead+=ms_lookahead;
  return m_TimePlusLookahead;};
  
  int&                 GetObject() {return m_object;};
  
  const char*          GetName()                  { return m_name; };
  
  int&                 GetXpos()                  { return m_xpos; };
  
  int&                 GetYpos() { return m_ypos;};

double&                             GetPopulation()
                                          { return m_population; };
    
  RTI::ObjectHandle&   GetInstanceId()            { return m_instanceId; };
  
  // Static Accessor Methods
  static RTI::ObjectClassHandle       GetTileRtiId()
  { return ms_TileTypeId; };
  static RTI::AttributeHandle         GetNameRtiId() 
  { return ms_nameTypeId; };
  static RTI::AttributeHandle         GetPopulationRtiId()
  { return ms_popTypeId; };
  static RTI::AttributeHandle         GetXposRtiId()
  { return ms_xposTypeId; };
  static RTI::AttributeHandle         GetObjectRtiId()
  { return ms_objectTypeId; };
  static RTI::AttributeHandle         GetYposRtiId()
  { return ms_yposTypeId; };
  static RTI::InteractionClassHandle  GetCommRtiId()
  { return ms_commTypeId; };
  static RTI::ParameterHandle         GetMessageRtiId()
  { return ms_commMsgTypeId; };
  static RTI::FedTime const &         GetLookahead()
  { return ms_lookahead;};

   //-----------------------------------------------------------------
   // Mutator Methods
   //-----------------------------------------------------------------
  void                                SetLastTime( RTI::FedTime const & time ) { m_lastTime = time;};
  void                                SetName( const char* );
  void                                SetXpos( const int& );
  void                                SetYpos( const int& );
  void                                SetObject( const int& );
  void                                SetPopulation( const double& );
  void                                Move(const int&, const int&);
  static void SetLookahead( RTI::FedTime& time )  {ms_lookahead = time;};

   //-----------------------------------------------------------------
   // Static Members
   //-----------------------------------------------------------------
   static const double                 ms_growthRate; 

   // Extent data memebers
   static Tile*                   ms_countryExtent[ MAX_FEDERATE + 1 ];
   static unsigned int                 ms_extentCardinality;    

protected:
   RTI::AttributeHandleValuePairSet*   CreateNVPSet();

private:
  char*                               m_name;
  int                                 m_xpos;
  int                                 m_ypos;
  int                                 m_object;
  double                              m_population;
  RTI::ObjectHandle                   m_instanceId;
  RTI::FedTime &                      m_lastTime;
  
  RTI::FedTime &                      m_TimePlusLookahead;
  static RTI::RTIambassador*          ms_rtiAmb;
  
   // Change flags for attribute values (dirty bits)
   RTI::Boolean                        hasNameChanged;
   RTI::Boolean                        hasPopulationChanged;
   RTI::Boolean                        hasYposChanged;
   RTI::Boolean                        hasXposChanged;
   RTI::Boolean                        hasObjectChanged;

   //-----------------------------------------------------------------
   // Static Member Data
   //-----------------------------------------------------------------


   // Run-Time Type Identification data
  static RTI::ObjectClassHandle       ms_TileTypeId;
  static RTI::AttributeHandle         ms_nameTypeId;
  static RTI::AttributeHandle         ms_xposTypeId;
  static RTI::AttributeHandle         ms_yposTypeId;
  static RTI::AttributeHandle         ms_objectTypeId;
  static RTI::AttributeHandle         ms_popTypeId;
  static RTI::InteractionClassHandle  ms_commTypeId;
  static RTI::ParameterHandle         ms_commMsgTypeId;

  // Names for querying RTTI values
  static char* const                  ms_TileTypeStr;       
  static char* const                  ms_nameTypeStr;       
  static char* const                  ms_xposTypeStr;
  static char* const                  ms_yposTypeStr;
  static char* const                  ms_objectTypeStr;
  static char* const                  ms_popTypeStr;       
  static char* const                  ms_commTypeStr;       
  static char* const                  ms_commMsgTypeStr;
  
  // Lookahead Time value
  static RTIfedTime                    ms_lookahead;
  
};

#endif
