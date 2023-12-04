// fedtimeInt.hh

#ifndef FEDTIME_HH
#define FEDTIME_HH

#include "RTI.hh"
#include <sys/types.h>
#include <string.h>

class RTIfedTime : public RTI::FedTime {

public:
  RTIfedTime(RTI::Long theTime);  //constructor

  //RTIfedTime(const RTIfedTime &toCopy);

  virtual ~RTIfedTime();

  virtual void setZero();

  virtual RTI::Boolean isZero();
  
  virtual void setEpsilon();

  virtual void setPositiveInfinity();

  virtual RTI::Boolean isPositiveInfinity();
  
  virtual RTI::FedTime& operator+= (const RTI::FedTime& theTime)
    throw (
      RTI::InvalidFederationTime);

  virtual RTI::FedTime& operator-= (const RTI::FedTime&)
    throw (
      RTI::InvalidFederationTime);
  
  virtual RTI::Boolean operator<= (const RTI::FedTime&) const
    throw (
      RTI::InvalidFederationTime);

  virtual RTI::Boolean operator< (const RTI::FedTime&) const
    throw (
      RTI::InvalidFederationTime);

   virtual RTI::Boolean operator>= (const RTI::FedTime&) const
    throw (
      RTI::InvalidFederationTime);
  
  virtual RTI::Boolean operator> (const RTI::FedTime&) const
    throw (
      RTI::InvalidFederationTime);

  virtual RTI::Boolean operator== (const RTI::FedTime&) const
    throw (
      RTI::InvalidFederationTime);
  
  virtual RTI::FedTime& operator= (const RTI::FedTime&)
    throw (
      RTI::InvalidFederationTime);
  
  //return bytes needed to encode
  virtual int encodedLength() const;

  //encode into suppled buffer
  virtual void encode(char *buff) const;
  
  // stream operators
  //friend ostream& operator << (ostream&, RTI::FedTime &);
  virtual int getPrintableLength() const;
  virtual void getPrintableString(char*);

  //
  // Implementation specific services
  //
  virtual RTI::Long getTime() const;
  
private:

  RTI::Long _fedTime;
  RTI::Long _zero;
  RTI::Long _epsilon;
  RTI::Long _positiveInfinity;
};


#endif








