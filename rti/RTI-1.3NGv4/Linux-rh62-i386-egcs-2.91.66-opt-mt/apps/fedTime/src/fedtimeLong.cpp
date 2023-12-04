// fedtimeInt.cc
// Implementation of the abstract time class

#include <RTI.hh>
#include <fedtimeInt.hh>
#include <values.h>
#include <stdio.h>
#include <arpa/inet.h>

RTI::FedTime::~FedTime()
{
}

// Constructor
RTIfedTime::RTIfedTime(RTI::Long theTime)
{
  _fedTime = theTime;
  _zero = 0;
  _epsilon = 1;
  _positiveInfinity = MAXLONG;  
}

RTIfedTime::~RTIfedTime()
{
  ;
}

void RTIfedTime::setZero()
{
  _fedTime = _zero;
}

RTI::Boolean RTIfedTime::isZero()
{
  if(_fedTime == 0)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

void RTIfedTime::setEpsilon()
{
  _fedTime = _epsilon;
}

void RTIfedTime::setPositiveInfinity()
{
  _fedTime = _positiveInfinity;
}

RTI::Boolean RTIfedTime::isPositiveInfinity()
{
  if(_fedTime == _positiveInfinity)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::FedTime& RTIfedTime::operator += (const RTI::FedTime& theTime)
    throw (
      RTI::InvalidFederationTime)
{
  RTIfedTime& myTime = (RTIfedTime&)theTime;
  
  _fedTime = _fedTime + myTime._fedTime;
  return *this;
}

RTI::FedTime& RTIfedTime::operator -= (const RTI::FedTime& theTime)
    throw (
      RTI::InvalidFederationTime)
{
  RTIfedTime& myTime = (RTIfedTime&)theTime;
  _fedTime = _fedTime - myTime._fedTime;
  return *this;
}

RTI::Boolean RTIfedTime::operator <= (const RTI::FedTime& theTime) const
    throw (
      RTI::InvalidFederationTime)
{
  RTIfedTime& myTime = (RTIfedTime&)theTime;
  if (_fedTime <= myTime._fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean RTIfedTime::operator < (const RTI::FedTime& theTime) const
    throw (
      RTI::InvalidFederationTime)
{
  RTIfedTime& myTime = (RTIfedTime&)theTime;
  if (_fedTime < myTime._fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean RTIfedTime::operator >= (const RTI::FedTime& theTime) const
    throw (
      RTI::InvalidFederationTime)
{
  RTIfedTime& myTime = (RTIfedTime&)theTime;
  if (_fedTime >= myTime._fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean RTIfedTime::operator > (const RTI::FedTime& theTime) const
    throw (
      RTI::InvalidFederationTime)
{
  RTIfedTime& myTime = (RTIfedTime&)theTime;
  if (_fedTime > myTime._fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean RTIfedTime::operator == (const RTI::FedTime& theTime) const
    throw (
      RTI::InvalidFederationTime)
{
  RTIfedTime& myTime = (RTIfedTime&)theTime;
  if (_fedTime == myTime._fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::FedTime& RTIfedTime::operator = (const RTI::FedTime& theTime)
    throw (
      RTI::InvalidFederationTime)
{
  RTIfedTime& myTime = (RTIfedTime&)theTime;
  _fedTime = myTime._fedTime;
  return *this;
  
}
  
  //return bytes needed to encode
int RTIfedTime::encodedLength() const
{
  return sizeof(RTI::Long) + sizeof( size_t );
}

  //encode into suppled buffer
void RTIfedTime::encode(char *buff) const
{
  /* marshal the length to support interoperability w/ the Java binding 
     (ideally, the API would be modified to pass the length to
     FedTimeFactory::decode)
     */
  long netnum = _fedTime;

  size_t length = htonl( sizeof( netnum ) );
  memcpy( buff, (char*) &length, sizeof ( length ) );

  buff += sizeof( length ) / sizeof( char );
  memcpy( buff, (char*)&netnum, sizeof( netnum ) );
}
  
// stream operators
//friend ostream& operator << (ostream&, RTIfedTime &);
int RTIfedTime::getPrintableLength() const
{
  char temp[100];
  sprintf(temp, "%d", _fedTime);
  return (strlen(temp)+1);
}

void RTIfedTime::getPrintableString(char *str)
{
  sprintf(str, "%d", _fedTime);
}

//
// Implementation specific services
//
RTI::Long RTIfedTime::getTime() const
{
  return _fedTime;
}

RTI::FedTime *
RTI::FedTimeFactory::makeZero()
  throw(RTI::MemoryExhausted)
{
  return new RTIfedTime(0);
  //the ctor will throw the exception
}

RTI::FedTime *
RTI::FedTimeFactory::decode(const char* buff)
  throw(RTI::MemoryExhausted)
{
  // skip over the first four bytes (length)
  buff += sizeof( size_t ) / sizeof( char );

  long netnum;
  memcpy((char*)&netnum, buff, sizeof(netnum));
  return new RTIfedTime(netnum);
  //the ctor will throw the exception
}







