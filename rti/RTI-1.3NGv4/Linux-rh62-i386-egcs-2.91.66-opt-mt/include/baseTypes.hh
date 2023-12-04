//File baseTypes.hh
//Included in RTI.hh

#ifndef NULL
#define NULL (0)
#endif

typedef unsigned short UShort;
typedef short          Short;
#ifdef __alpha
typedef unsigned int   ULong;
typedef int            Long;
#else
typedef unsigned long  ULong;
typedef long           Long;
#endif
typedef double         Double;
typedef float          Float;

enum Boolean {
    RTI_FALSE = 0,
    RTI_TRUE};

class RTI_EXPORT Exception {
public:
  ULong _serial;
  char *_reason;
  const char *_name;
  Exception (const char *reason);
  Exception (ULong serial, const char *reason=NULL);
  Exception (const Exception &toCopy);
  virtual ~Exception ();
  Exception & operator = (const Exception &);
  //
  // RTI_STD was added for the RTI 1.3NG to allow the use of the Standard C++ 
  // ostream or to use the legacy ostream.  The issue concerns whether ostream
  // is in the global namespace or in namespace std.
  //
  friend RTI_STD::ostream RTI_EXPORT & operator<< (RTI_STD::ostream &, Exception *);
  //
  // Added for RTI 1.3 NG to workaround a Sun 4.2 C++ compiler bug.
  // These are used internally to pass exceptions between threads.
  //
  virtual Exception * cloneSelf() const throw() = 0;
  virtual void        throwSelf() const         = 0;
};

#define RTI_EXCEPT(A)   \
class A : public Exception {       \
public: \
  static RTI_EXPORT const char *_ex;  \
  A (const char *reason) : Exception (reason)  { _name = _ex; }   \
  A (ULong serial, const char *reason=NULL)        \
    : Exception (serial, reason)  { _name = _ex; }        \
  A (A const & toCopy) : Exception(toCopy) { _name = _ex; } \
  Exception * cloneSelf() const throw() { return (new A(_reason)); } \
  void        throwSelf() const         { throw *this; } \
};
