// -*-c++-*-
#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <ctime>
#include <sys/time.h>

using namespace std;


long double micro_time(void);



class Timer {
  long double time;
public:
  Timer();
  Timer(long double t);
  friend ostream& operator<< (ostream& o, const Timer& t);
  const Timer& operator+= (const Timer& t);
  const Timer& operator-= (const Timer& t);
  Timer operator+ (const Timer& t) const;
  Timer operator- (const Timer& t) const;
  long double operator* (long double a) const;
  friend long double operator* (long double a, const Timer& t);
  long double operator/ (long double a) const;
  friend long double operator/ (long double a, const Timer& t);
  const Timer& operator= (const Timer& t);
  bool operator== (const Timer& t) const;
  bool operator!= (const Timer& t) const;
  bool operator< (const Timer& t) const;
  bool operator> (const Timer& t) const;
  bool operator<= (const Timer& t) const;
  bool operator>= (const Timer& t) const;
  long double get(void) const;            //seconds
  void update(void);
  //void set(long double us);               //u-seconds
};

namespace _time {
  static Timer tic_time, tic_time0;
  void tic(void);
  Timer toc(void);
  void tic(Timer& t0);
  Timer toc(const Timer& t0);
}

#endif
