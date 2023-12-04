#ifndef FedTime_h
#define FedTime_h


#if defined(_WIN32)
#if defined(BUILDING_FEDTIME)
// define the proper qualifiers to import/export symbols from/to DLL
#define RTI_EXPORT_FEDTIME __declspec(dllexport)
#else // !BUILDING_RTI
#define RTI_EXPORT_FEDTIME __declspec(dllimport)
#endif // BUILDING_FEDTIME
#else // !WIN32
// no special qualfifers are necessary on non-WIN32 platforms
#define RTI_EXPORT
#define RTI_EXPORT_FEDTIME
#endif



#include <RTI.hh>
#include <sys/types.h>
#include <string.h>

class RTI_EXPORT_FEDTIME MyFedTime : public RTI::FedTime {
//-----------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------
public:
  MyFedTime();
  MyFedTime(const long&);
  MyFedTime(const RTI::FedTime&);
  MyFedTime(const MyFedTime&);
  virtual ~MyFedTime();

//-----------------------------------------------------------------
// Overloaded functions from RTI::FedTime
//-----------------------------------------------------------------
public:
  virtual void                setZero();
  virtual RTI::Boolean        isZero();
  virtual void                setEpsilon();
  virtual void                setPositiveInfinity();
  virtual RTI::Boolean        isPositiveInfinity();
  virtual int                 encodedLength() const;
  virtual void                encode(char *buff) const;
  virtual int                 getPrintableLength() const;
  virtual void                getPrintableString(char*);

//-----------------------------------------------------------------
// Overloaded operators from RTI::FedTime
//-----------------------------------------------------------------
public:
  virtual RTI::FedTime& operator+= (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator-= (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);
  
  virtual RTI::Boolean operator<= (const RTI::FedTime&) const
    throw (RTI::InvalidFederationTime);

  virtual RTI::Boolean operator< (const RTI::FedTime&) const
    throw (RTI::InvalidFederationTime);

   virtual RTI::Boolean operator>= (const RTI::FedTime&) const
    throw (RTI::InvalidFederationTime);
  
  virtual RTI::Boolean operator> (const RTI::FedTime&) const
    throw (RTI::InvalidFederationTime);

  virtual RTI::Boolean operator== (const RTI::FedTime&) const
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator= (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);

//-----------------------------------------------------------------
// Implementation functions
//-----------------------------------------------------------------
public:
  virtual long           getTime() const;

//-----------------------------------------------------------------
// Implementation operators
//-----------------------------------------------------------------
  virtual RTI::Boolean operator== (const long&) const
    throw (RTI::InvalidFederationTime);

  virtual RTI::Boolean operator!= (const RTI::FedTime&) const
    throw (RTI::InvalidFederationTime);
  
  virtual RTI::Boolean operator!= (const long&) const
    throw (RTI::InvalidFederationTime);
  
  virtual RTI::FedTime& operator= (const MyFedTime&)
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator= (const long&)
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator*= (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator/= (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator+= (const long&)
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator-= (const long&)
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator*= (const long&)
    throw (RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator/= (const long&)
    throw (RTI::InvalidFederationTime);

  virtual MyFedTime operator+ (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);

  virtual MyFedTime operator+ (const long&)
    throw (RTI::InvalidFederationTime);

  virtual MyFedTime operator- (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);

  virtual MyFedTime operator- (const long&)
    throw (RTI::InvalidFederationTime);

  virtual MyFedTime operator* (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);

  virtual MyFedTime operator* (const long&)
    throw (RTI::InvalidFederationTime);

  virtual MyFedTime operator/ (const RTI::FedTime&)
    throw (RTI::InvalidFederationTime);

  virtual MyFedTime operator/ (const long&)
    throw (RTI::InvalidFederationTime);
  
//-----------------------------------------------------------------
// Implementation friends
//-----------------------------------------------------------------
public:
  friend ostream & operator<< (ostream&, RTI::FedTime const &);

//-----------------------------------------------------------------
// Implementation member variables
//-----------------------------------------------------------------
private:
  long                 _fedTime;
  long                 _zero;
  long                 _epsilon;
  long                 _positiveInfinity;
};

//-----------------------------------------------------------------
// Global operators
//-----------------------------------------------------------------

/*MyFedTime operator+ (const long&, const RTI::FedTime&);
MyFedTime operator- (const long&, const RTI::FedTime&);
MyFedTime operator* (const long&, const RTI::FedTime&);
MyFedTime operator/ (const long&, const RTI::FedTime&);
*/
#endif
