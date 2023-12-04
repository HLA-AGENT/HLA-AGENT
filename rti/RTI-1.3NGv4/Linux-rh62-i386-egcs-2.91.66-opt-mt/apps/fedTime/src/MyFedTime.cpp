#include "MyFedTime.h"
#include <stdio.h>
#include <RTI.hh>
//#include <values.h>
#include <limits.h>

#define POSITIVE_INFINITY (LONG_MAX)

//-----------------------------------------------------------------
// RTI::FedTime and RTI::FedTimeFactory functions
//-----------------------------------------------------------------

RTI::FedTime::~FedTime()
{
}

RTI::FedTime *
RTI::FedTimeFactory::makeZero()
  throw(RTI::MemoryExhausted)
{
  return new MyFedTime(0);
}

RTI::FedTime *
RTI::FedTimeFactory::decode(const char* buff)
  throw(RTI::MemoryExhausted)
{
  long netnum;
  memcpy((char*)&netnum, buff, sizeof(netnum));
  return new MyFedTime(netnum);
}

//-----------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------

MyFedTime::MyFedTime() :
  _fedTime(0),
  _zero(0),
  _epsilon(0),
  _positiveInfinity(POSITIVE_INFINITY)
{
}

MyFedTime::MyFedTime(const long& theTime) :
  _fedTime(theTime),
  _zero(0),
  _epsilon(0),
  _positiveInfinity(POSITIVE_INFINITY)
{
}

MyFedTime::MyFedTime(const RTI::FedTime& toCopy) :
  _fedTime(((MyFedTime&)toCopy)._fedTime),
  _zero(((MyFedTime&)toCopy)._zero),
  _epsilon(((MyFedTime&)toCopy)._epsilon),
  _positiveInfinity(((MyFedTime&)toCopy)._positiveInfinity)
{
}

MyFedTime::MyFedTime(const MyFedTime& toCopy) :
  _fedTime(toCopy._fedTime),
  _zero(toCopy._zero),
  _epsilon(toCopy._epsilon),
  _positiveInfinity(toCopy._positiveInfinity)
{
}

MyFedTime::~MyFedTime()
{
}

//-----------------------------------------------------------------
// Overloaded from RTI::FedTime
//-----------------------------------------------------------------
void 
MyFedTime::setZero()
{
  _fedTime = _zero;
}

RTI::Boolean 
MyFedTime::isZero()
{
  if(_fedTime == _zero)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

void 
MyFedTime::setEpsilon()
{
  _fedTime = _epsilon;
}

void 
MyFedTime::setPositiveInfinity()
{
  _fedTime = _positiveInfinity;
}

RTI::Boolean 
MyFedTime::isPositiveInfinity()
{
  if(_fedTime == _positiveInfinity)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

//return bytes needed to encode
int 
MyFedTime::encodedLength() const
{
  return sizeof(long);
}

//encode into suppled buffer
void 
MyFedTime::encode(char *buff) const
{
  long netnum = _fedTime;
  memcpy(buff, (char*)&netnum, sizeof(netnum));
}
  
int 
MyFedTime::getPrintableLength() const
{ 
  char buff[256];
  sprintf(buff, "%ld", _fedTime);
  return strlen(buff);
}

void 
MyFedTime::getPrintableString(char* buff)
{
  sprintf(buff, "%ld", _fedTime);
}

//-----------------------------------------------------------------
// Overloaded operators from RTI::FedTime
//-----------------------------------------------------------------

RTI::FedTime& 
MyFedTime::operator+= (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  _fedTime += fedTime;
  return *this;
}

RTI::FedTime& 
MyFedTime::operator-= (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  _fedTime -= fedTime;
  return *this;
}

RTI::Boolean 
MyFedTime::operator<= (const RTI::FedTime& theTime) const
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  if (_fedTime <= fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean 
MyFedTime::operator< (const RTI::FedTime& theTime) const
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  if (_fedTime < fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean 
MyFedTime::operator>= (const RTI::FedTime& theTime) const
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  if (_fedTime >= fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean 
MyFedTime::operator> (const RTI::FedTime& theTime) const
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  if (_fedTime > fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean 
MyFedTime::operator== (const RTI::FedTime& theTime) const
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  if (_fedTime == fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::FedTime& 
MyFedTime::operator= (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  _fedTime = fedTime;
  return *this;
}

//-----------------------------------------------------------------
// Implementation functions
//-----------------------------------------------------------------

long 
MyFedTime::getTime() const
{
  return _fedTime;
}

//-----------------------------------------------------------------
// Implementation operators
//-----------------------------------------------------------------

RTI::Boolean 
MyFedTime::operator== (const long& theTime) const
  throw (RTI::InvalidFederationTime)
{
  if (_fedTime == theTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean 
MyFedTime::operator!= (const RTI::FedTime& theTime) const
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  if (_fedTime != fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean 
MyFedTime::operator!= (const long& theTime) const
  throw (RTI::InvalidFederationTime)
{
  if (_fedTime != theTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::FedTime& 
MyFedTime::operator= (const MyFedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime = theTime._fedTime;
  return *this;
}

RTI::FedTime& 
MyFedTime::operator= (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime = theTime;
  return *this;
}

RTI::FedTime& 
MyFedTime::operator*= (const RTI::FedTime& theTime)
    throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  _fedTime *= fedTime;
  return *this;
}

RTI::FedTime& 
MyFedTime::operator/= (const RTI::FedTime& theTime)
    throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  _fedTime /= fedTime;
  return *this;
}

RTI::FedTime& 
MyFedTime::operator+= (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime += theTime;
  return *this;
}

RTI::FedTime& 
MyFedTime::operator-= (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime -= theTime;
  return *this;
}

RTI::FedTime& 
MyFedTime::operator*= (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime *= theTime;
  return *this;
}

RTI::FedTime& 
MyFedTime::operator/= (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime /= theTime;
  return *this;
}

MyFedTime
MyFedTime::operator+ (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  MyFedTime outTime(_fedTime + fedTime);
  return outTime;
}

MyFedTime
MyFedTime::operator+ (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  MyFedTime outTime(_fedTime + theTime);
  return outTime;
}

MyFedTime
MyFedTime::operator- (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  MyFedTime outTime(_fedTime - fedTime);
  return outTime;
}

MyFedTime
MyFedTime::operator- (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  MyFedTime outTime(_fedTime - theTime);
  return outTime;
}

MyFedTime
MyFedTime::operator* (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  MyFedTime outTime(_fedTime * fedTime);
  return outTime;
}

MyFedTime
MyFedTime::operator* (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  MyFedTime outTime(_fedTime * theTime);
  return outTime;
}

MyFedTime
MyFedTime::operator/ (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const long& fedTime = ((MyFedTime&)theTime)._fedTime;
  MyFedTime outTime(_fedTime / fedTime);
  return outTime;
}

MyFedTime
MyFedTime::operator/ (const long& theTime)
  throw (RTI::InvalidFederationTime)
{
  MyFedTime outTime(_fedTime / theTime);
  return outTime;
}

//-----------------------------------------------------------------
// Implementation friends
//-----------------------------------------------------------------

ostream& 
operator<< (ostream& s, RTI::FedTime const & theTime)
{
  MyFedTime const & myTime = (MyFedTime const &)theTime;
  s << myTime.getTime();
  return s;
}

//-----------------------------------------------------------------
// Global operators
//-----------------------------------------------------------------

MyFedTime 
operator+ (const long& theTime1, const RTI::FedTime& theTime2)
{
  const long& fedTime2 = ((MyFedTime&)theTime2).getTime();
  MyFedTime outTime(theTime1 + fedTime2);
  return outTime;
}

MyFedTime 
operator- (const long& theTime1, const RTI::FedTime& theTime2)
{
  const long& fedTime2 = ((MyFedTime&)theTime2).getTime();
  MyFedTime outTime(theTime1 - fedTime2);
  return outTime;
}

MyFedTime 
operator* (const long& theTime1, const RTI::FedTime& theTime2)
{
  const long& fedTime2 = ((MyFedTime&)theTime2).getTime();
  MyFedTime outTime(theTime1 * fedTime2);
  return outTime;
}

MyFedTime 
operator/ (const long& theTime1, const RTI::FedTime& theTime2)
{
  const long& fedTime2 = ((MyFedTime&)theTime2).getTime();
  MyFedTime outTime(theTime1 / fedTime2);
  return outTime;
}
