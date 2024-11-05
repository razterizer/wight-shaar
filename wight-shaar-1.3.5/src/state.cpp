#include "state.h"


ostream& operator<< (ostream& o, const State& s){
  o << "x: " << s.x << endl
    << "v: " << s.v << endl
    << "L: " << s.L << endl
    << "R: " << s.q;
  return o;
}

const State& State :: operator+= (const State& s){
  x += s.x;
  v += s.v;
  L += s.L;
  q += s.q;
  return *this;
}

const State& State :: operator*= (float h){
  x *= h;
  v *= h;
  L *= h;
  q *= h;
  return *this;
}

State State :: operator+ (const State& s) const {
  State tmp(*this);
  tmp += s;
  return tmp;
}

State State :: operator* (float h) const {
  State tmp(*this);
  tmp *= h;
  return tmp;
}

State operator* (float h, const State& s) {
  return s*h;
}

Vector& State :: pos(void) {
  return x;
}

void State :: pos(const Vector& p) {
  x=p;
}

Vector& State :: vel(void) {
  return v;
}

void State :: vel(const Vector& v_) {
  v=v_;
}

Vector& State :: ang_mom(void) {
  return L;
}

void State :: ang_mom(const Vector& l) {
  L = l;
}

Quaternion& State :: rot(void) {
  return q;
}

void State :: rot(const Quaternion& r) {
  q = r;
}
