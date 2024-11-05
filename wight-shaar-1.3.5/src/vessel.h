// -*-c++-*-
#ifndef VESSEL_H
#define VESSEL_H

#include <stdexcept>

#include "thruster.h"
#include "state.h"
#include "gbase.h"


float inertia_determinant(const Matrix& m_);

Matrix inertia_inverse(const Matrix& I_) throw(overflow_error);


/*****************************************
 *          ^ x                          *
 *          |                            *
 *          _                            *
 *         / |                           *
 * -z <-- (  =       y = up              *
 *         \_|                           *
 *                                       *
 *****************************************/


class Vessel : public GBase {
protected:
  Matrix I, I_inv;          //assume parts are somewhat static
  State s;
  Thruster *thruster_list;
  unsigned Nt;
  float m;
private:
  Vessel();
public:
  Vessel(const string& n);
  Vessel(const Matrix& I, State s0, unsigned Nt, float m,
       const string& n);
  Vessel(const Matrix& I, State s0, unsigned Nt, float m,
       const string& path, const string& wfobj,
       const string& n);
  Vessel(const Vessel& sh);
  ~Vessel();
public:
  GTYPE type(void) const;
  Matrix& inertia(const Matrix& m);
  const Matrix& inertia(void) const;
  const Matrix& inertia_inv(void) const;
  Thruster& thruster(unsigned i) throw(out_of_range);
  int num_thrusters(int n);           //for creating thrusters otherwise error
  int num_thrusters(void) const;
  void bind_thruster(unsigned i, GBase& n, const Vector& piv) throw(out_of_range); //thruster No i <-> node
  float mass(float m);
  float mass(void) const;
  State& state(const State& s);         //for setting the init state
  State& state(void);
  void render(void);
  Ray ray(void);
};


#endif
