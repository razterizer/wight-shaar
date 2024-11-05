#include "torpedo.h"



Torpedo :: Torpedo(const Matrix& I, State s0,
		   float m_0, float m_f, float I_sp, float t_b)
  : Vessel(I, s0, 1, m_0+m_f, "torpedo"),
    m_0(m_0), m_fuel0(m_f), m_fuel(m_f), m_dot(m_f/t_b),
    I_sp(I_sp), t_b(t_b), F(I_sp*9.81f*m_f/t_b)
{
  active=false;
  thruster(0).force(Vector(0,0,-F));
}

Torpedo :: Torpedo(const Torpedo& t)
  : Vessel(t),
    m_0(t.m_0), m_fuel0(t.m_fuel0), m_fuel(t.m_fuel), m_dot(t.m_dot),
    I_sp(t.I_sp), t_b(t.t_b), F(t.F)
{
  active=false;
}

GTYPE Torpedo :: type(void) const {
  return TORPEDO;
}

void Torpedo :: consume_fuel(float dt) {
  if (active) {
    float dm_f = m_dot*dt;
    m_fuel -= dm_f;   //for reference
    m -= dm_f;        //m = m_0 + m_fuel
    if (m_fuel <= 0){    //self destruct when out of fuel
      active = false;
      thruster(0).enabled(false);
    }
  }
}

void Torpedo :: activate(void) {
  if (not active) {
    m = m_0+m_fuel0;
    m_fuel = m_fuel0;
    thruster(0).enabled(true);
    active = true;
  }
}
