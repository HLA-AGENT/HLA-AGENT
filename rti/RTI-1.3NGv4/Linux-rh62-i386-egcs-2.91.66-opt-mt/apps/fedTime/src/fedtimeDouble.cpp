/*______________________________________________________________________________

Filename:  RTIfedTime.cpp

Author(s): J. Russell Noseworthy

Summary:

History:
 Feb. 1999  J. Russell Noseworthy    initial release.

                      SAIC/OSC/VTC (All Rights Reserved)
______________________________________________________________________________*/

#include <fedtime.hh>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#ifdef __SUNPRO_CC
#include <math.h>
#define infinity HUGE_VAL;
#elif _MSC_VER
#include <limits>
using std::numeric_limits<double>;
#include <winsock2.h> // for htonl
#endif

static double const
RTI_POSITIVE_INFINITY()
{
#ifdef __SUNPRO_CC
  return infinity;
#elif _MSC_VER
  return numeric_limits<double>::infinity();
#endif
}

RTIfedTime::RTIfedTime(const double & theTime)
  : _fedTime(theTime),
    _zero(0.0),
    _epsilon(0.000000001),
    _positiveInfinity(RTI_POSITIVE_INFINITY())
{
}

RTIfedTime::RTIfedTime(const RTIfedTime & theTime)
  : _fedTime(theTime._fedTime),
    _zero(theTime._zero),
    _epsilon(theTime._epsilon),
    _positiveInfinity(theTime._positiveInfinity)
{
}

RTIfedTime::RTIfedTime(const RTI::FedTime & theTime)
{
  RTIfedTime const & myTime(dynamic_cast<RTIfedTime const &>(theTime));
  _fedTime = myTime._fedTime;
  _zero = myTime._zero;
  _epsilon = myTime._epsilon;
  _positiveInfinity = myTime._positiveInfinity;
}

RTIfedTime::~RTIfedTime()
{
}

void RTIfedTime::setZero()
{
  _fedTime = _zero;
}

RTI::Boolean RTIfedTime::isZero()
{
  if(_fedTime == 0.0)
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
    throw (RTI::InvalidFederationTime)
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
  const RTI::Double& fedTime = ((RTIfedTime&)theTime)._fedTime;
  if (_fedTime == fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

//-----------------------------------------------------------------
// Implementation operators
//-----------------------------------------------------------------

RTI::Boolean 
RTIfedTime::operator== (const RTI::Double& theTime) const
  throw (RTI::InvalidFederationTime)
{
  if (_fedTime == theTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean 
RTIfedTime::operator!= (const RTI::FedTime& theTime) const
  throw (RTI::InvalidFederationTime)
{
  const RTI::Double& fedTime = ((RTIfedTime&)theTime)._fedTime;
  if (_fedTime != fedTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::Boolean 
RTIfedTime::operator!= (const RTI::Double& theTime) const
  throw (RTI::InvalidFederationTime)
{
  if (_fedTime != theTime)
    return RTI::RTI_TRUE;
  else
    return RTI::RTI_FALSE;
}

RTI::FedTime& 
RTIfedTime::operator*= (const RTI::FedTime& theTime)
    throw (RTI::InvalidFederationTime)
{
  const RTI::Double& fedTime = ((RTIfedTime&)theTime)._fedTime;
  _fedTime *= fedTime;
  return *this;
}

RTI::FedTime& 
RTIfedTime::operator/= (const RTI::FedTime& theTime)
    throw (RTI::InvalidFederationTime)
{
  const RTI::Double& fedTime = ((RTIfedTime&)theTime)._fedTime;
  _fedTime /= fedTime;
  return *this;
}

RTI::FedTime& 
RTIfedTime::operator+= (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime += theTime;
  return *this;
}

RTI::FedTime& 
RTIfedTime::operator-= (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime -= theTime;
  return *this;
}

RTI::FedTime& 
RTIfedTime::operator*= (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime *= theTime;
  return *this;
}

RTI::FedTime& 
RTIfedTime::operator/= (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime /= theTime;
  return *this;
}

RTIfedTime
RTIfedTime::operator+ (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const RTI::Double& fedTime = ((RTIfedTime&)theTime)._fedTime;
  RTIfedTime outTime(_fedTime + fedTime);
  return outTime;
}

RTIfedTime
RTIfedTime::operator+ (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  RTIfedTime outTime(_fedTime + theTime);
  return outTime;
}

RTIfedTime
RTIfedTime::operator- (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const RTI::Double& fedTime = ((RTIfedTime&)theTime)._fedTime;
  RTIfedTime outTime(_fedTime - fedTime);
  return outTime;
}

RTIfedTime
RTIfedTime::operator- (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  RTIfedTime outTime(_fedTime - theTime);
  return outTime;
}

RTIfedTime
RTIfedTime::operator* (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const RTI::Double& fedTime = ((RTIfedTime&)theTime)._fedTime;
  RTIfedTime outTime(_fedTime * fedTime);
  return outTime;
}

RTIfedTime
RTIfedTime::operator* (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  RTIfedTime outTime(_fedTime * theTime);
  return outTime;
}

RTIfedTime
RTIfedTime::operator/ (const RTI::FedTime& theTime)
  throw (RTI::InvalidFederationTime)
{
  const RTI::Double& fedTime = ((RTIfedTime&)theTime)._fedTime;
  RTIfedTime outTime(_fedTime / fedTime);
  return outTime;
}

RTIfedTime
RTIfedTime::operator/ (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  RTIfedTime outTime(_fedTime / theTime);
  return outTime;
}

RTI::FedTime & RTIfedTime::operator = (const RTI::FedTime& theTime)
 throw (RTI::InvalidFederationTime)
{
  RTIfedTime const & myTime(dynamic_cast<RTIfedTime const &>(theTime));
  _fedTime = myTime._fedTime;
 return *this;
}

RTI::FedTime & RTIfedTime::operator = (const RTIfedTime& theTime)
 throw (RTI::InvalidFederationTime)
{
  _fedTime = theTime._fedTime;
 return *this;
}


RTI::FedTime& 
RTIfedTime::operator= (const RTI::Double& theTime)
  throw (RTI::InvalidFederationTime)
{
  _fedTime = theTime;
  return *this;
}

  //return bytes needed to encode
int RTIfedTime::encodedLength() const
{
  return sizeof(RTI::Double);
}

//encode into suppled buffer
void 
RTIfedTime::encode(char *buff) const
{
  RTI::Double netnum = _fedTime;
#if defined(_M_IX86) || defined(i386)
  *((int*)buff) = htonl(*(((int*)&netnum)+1));
  *(((int*)buff)+1) = htonl(*((int*)&netnum));
#else
#ifdef __alpha
  RTI::Double x;
  cvt_ftof(&netnum, CVT_IEEE_T, &x, CVT_BIG_ENDIAN_IEEE_T, 0);
  netnum = x;
#endif

  memcpy(buff, (char*)&netnum, sizeof(netnum));
#endif
}
  
// stream operators
//friend ostream& operator << (ostream&, RTIfedTime &);
int RTIfedTime::getPrintableLength() const
{ 
  char buff[512];
  sprintf(buff, "%.2lf", _fedTime);
  return strlen(buff) +1;

}

void RTIfedTime::getPrintableString(char* buf) 
{
  sprintf(buf, "%.2lf", _fedTime);
}

//
// Implementation specific services
//
RTI::Double RTIfedTime::getTime() const
{
  return _fedTime;
}
