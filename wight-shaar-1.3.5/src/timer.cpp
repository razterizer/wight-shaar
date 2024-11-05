#include "timer.h"

#ifdef WIN32

long double micro_time(void) {
  return clock()*1.0e6/CLOCKS_PER_SEC;
}

#else

long double micro_time(void) {
  timeval t;
  gettimeofday(&t, NULL);
  long double us = t.tv_usec + t.tv_sec*1.0e6;
  return us;
}

#endif

/***********************************************
 *                    TIMER                    *
 ***********************************************/


Timer :: Timer() {
  time = micro_time();
}

Timer :: Timer(long double t) {
  time = t;
}

ostream& operator<< (ostream& o, const Timer& t) {
  if (1e6 <= t.time)
    o << t.time*1e-6 << " [s]";
  else if (1e3 <= t.time)
    o << t.time*1e-3 << " [ms]";
  else if (1.0 <= t.time)
    o << t.time << " [us]";
  return o;
}

const Timer& Timer :: operator+= (const Timer& t) {
  time += t.time;
  return *this;
}

const Timer& Timer :: operator-= (const Timer& t) {
  time -= t.time;
  return *this;
}

Timer Timer :: operator+ (const Timer& t) const {
  Timer tmp(*this);
  tmp.time += t.time;
  return tmp;
}

Timer Timer :: operator- (const Timer& t) const {
  Timer tmp(*this);
  tmp.time -= t.time;
  return tmp;
}

long double Timer :: operator* (long double a) const {
  return time*1e-6*a;  //seconds
}

long double operator* (long double a, const Timer& t) {
  return a*t.time*1e-6;  //seconds
}

long double Timer :: operator/ (long double a) const {
  return time*1e-6/a;
}

long double operator/ (long double a, const Timer& t) {
  return a/t.time*1e6;
}

const Timer& Timer :: operator= (const Timer& t) {
  time = t.time;
  return *this;
}

bool Timer :: operator== (const Timer& t) const {
  return time==t.time;
}

bool Timer :: operator!= (const Timer& t) const {
  return time!=t.time;
}

bool Timer :: operator< (const Timer& t) const {
  return time<t.time;
}

bool Timer :: operator> (const Timer& t) const {
  return time>t.time;
}

bool Timer :: operator<= (const Timer& t) const {
  return time<=t.time;
}

bool Timer :: operator>= (const Timer& t) const {
  return time>=t.time;
}

long double Timer :: get(void) const {
  return time*1e-6;
}

void Timer :: update(void) {
  time = micro_time();
}

//void Timer :: set(long double us) {
//  time = us;
//}


void _time::tic(void) {
  _time::tic_time0.update();
  _time::tic_time = _time::tic_time0;
}

Timer _time::toc(void) {
  _time::tic_time.update();
  return _time::tic_time - _time::tic_time0;
}

void _time::tic(Timer& t0) {
  t0.update();
}

Timer _time::toc(const Timer& t0) {
  Timer t;
  t-=t0;
  return t;
}
