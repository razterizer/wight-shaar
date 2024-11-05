// -*-c++-*-
#ifndef THRUSTER_H
#define THRUSTER_H

#include <cmath>

#include "geometry.h"
#include "transform.h"
#include "gbase.h"
#include "stuff.h"




/********************************************
 * Everything is in body-fixed coordinates! *
 ********************************************/

class Thruster {
  Vector x, F;
  bool on;
  float max_x, max_y;   //RAD
  Quaternion qx,qy;
  Vector gimbal_pos;
  GBase *rot_node;      //pointer to rotating part
public:
  Thruster();
  Thruster(const Vector& x, const Vector& F,
	   bool on, float max_ang_x, float max_ang_y);
  bool enabled(void) const;
  void enabled(bool flag);
  Vector force(void) const;
  void force(const Vector& f);
  Vector pos(void) const;
  void pos(const Vector& p);
  Vector torque(void) const;
  void gimbal(float dx, float dy);            //DEG
  void gimbal_reset(void);
  void max_angles(float max_x, float max_y);  //DEG
  float angle_x(void) const;                  //DEG
  float angle_y(void) const;                  //DEG
  void bind(GBase *n, const Vector& piv);  //bind thruster to node
};


#endif
