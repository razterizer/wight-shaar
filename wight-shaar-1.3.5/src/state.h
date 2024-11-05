// -*-c++-*-
#ifndef STATE_H
#define STATE_H

#include <iostream>

#include "geometry.h"
#include "transform.h"


class State {
  Vector x, v, L;
  Quaternion q;
public:
  State() {}
  State(const Vector& x0,     //should be Vertex instead
	const Vector& v0,
	const Vector& L0,
	const Quaternion& q0)
    : x(x0), v(v0), L(L0), q(q0) {}
  friend ostream& operator<< (ostream& o, const State& s);
  const State& operator+= (const State& s);
  const State& operator*= (float h);
  State operator+ (const State& s) const;
  State operator* (float h) const;
  friend State operator* (float h, const State& s);
  Vector& pos(void);
  void pos(const Vector& p);
  Vector& vel(void);
  void vel(const Vector& v);
  Vector& ang_mom(void);
  void ang_mom(const Vector& l);
  Quaternion& rot(void);
  void rot(const Quaternion& r);
};





#endif
