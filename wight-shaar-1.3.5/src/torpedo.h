// -*-c++-*-
#ifndef TORPEDO_H
#define TORPEDO_H

#include "vessel.h"


class Torpedo : public Vessel {
protected:
  float m_0, m_fuel0, m_fuel, m_dot, I_sp, t_b, F;
private:
  Torpedo();
public:
  Torpedo(const Matrix& I, State s0,
	  float m_0, float m_f, float I_sp, float t_b);
  Torpedo(const Torpedo& t);
  GTYPE type(void) const;
  void consume_fuel(float dt);
  void activate(void);
};






#endif
