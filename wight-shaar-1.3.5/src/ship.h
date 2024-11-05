// -*-c++-*-
#ifndef SHIP_H
#define SHIP_H

#include <stdexcept>

#include "vessel.h"
#include "torpedo.h"


class Ship : public Vessel {
protected:
  Torpedo *w_torp;
private:
  Ship();
public:
  Ship(const string& n);
  Ship(const Matrix& I, State s0, unsigned Nt, float m,
       const string& n);
  Ship(const Matrix& I, State s0, unsigned Nt, float m,
       const string& path, const string& wfobj,
       const string& n);
  Ship(const Ship& sh);
  ~Ship();
public:
  GTYPE type(void) const;
  void set_torpedo(Torpedo *t);
  void activate_torpedo(void);
};


#endif
