// -*-c++-*-
#ifndef ODE_H
#define ODE_H

#include <stdexcept>
#include <vector>

#include "vessel.h"


typedef State (*DiffFun)(float,
			 const State&,
			 Vessel *,
			 const vector<float>&);



State diff_rigid(float t,
		 const State& s,
		 Vessel *craft,
		 const vector<float>& params);

State diff_spring(float t,
		  const State& s,
		  Vessel *craft,
		  const vector<float>& params)
  throw(invalid_argument);



class ODE {
protected:
  const DiffFun diff_state;
  vector<float> params;
  Vessel *craft;
  float t;
  float dt;
public:
  ODE(DiffFun ds, Vessel *v)
    : diff_state(ds), craft(v), t(0), dt(.01) {}
  ODE(DiffFun ds, vector<float> p, Vessel *v)
    : diff_state(ds), params(p), craft(v), t(0), dt(.01) {}
  ODE(DiffFun ds, Vessel *v, float dt)
    : diff_state(ds), craft(v), t(0), dt(dt) {}
  ODE(DiffFun ds, vector<float> p, Vessel *v, float dt)
    : diff_state(ds), params(p), craft(v), t(0), dt(dt) {}
  virtual ~ODE() {}
  virtual State& step(void) = 0;
  virtual State& step(float dt);
  virtual State& step(vector<float> p);
  virtual State& step(vector<float> p, float dt);
  State& current(void) const;
  float time(void) const;
  float timestep(void) const;
};


class Euler : public ODE {
public:
  Euler(DiffFun ds, Vessel *v)
    : ODE(ds, v) {}
  Euler(DiffFun ds, vector<float> p, Vessel *v)
    : ODE(ds, p, v) {}
  Euler(DiffFun ds, Vessel *v, float dt)
    : ODE(ds, v, dt) {}
  Euler(DiffFun ds, vector<float> p, Vessel *v, float dt)
    : ODE(ds, p, v, dt) {}
  State& step(void);
};



class RungeKutta : public ODE {
  State k[4], s;
public:
  RungeKutta(DiffFun ds, Vessel *v)
    : ODE(ds, v) {}
  RungeKutta(DiffFun ds, vector<float> p, Vessel *v)
    : ODE(ds, p, v) {}
  RungeKutta(DiffFun ds, Vessel *v, float dt)
    : ODE(ds, v, dt) {}
  RungeKutta(DiffFun ds, vector<float> p, Vessel *v, float dt)
    : ODE(ds, p, v, dt) {}
  State& step(void);
};



#endif
