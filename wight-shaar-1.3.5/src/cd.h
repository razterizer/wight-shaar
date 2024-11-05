// -*-c++-*-
#ifndef CD_H
#define CD_H

#include <cmath>
#include <stdexcept>

#include "geometry.h"
#include "gbase.h"
#include "bb.h"
#include "stuff.h"

class CD {
  void (*action_aabb)(GBase *coll1, GBase *coll2);
  void (*action_ray)(GBase *coll, const Ray& r);
  bool overlap(const BB& b1, const BB& b2);
  bool intersect(const BB& b, Ray& r);
public:
  CD()
    : action_aabb(NULL), action_ray(NULL) {}
  CD(void (*f_aabb)(GBase *c1, GBase *c2),
     void (*f_ray)(GBase *c, const Ray& r))
    : action_aabb(f_aabb), action_ray(f_ray) {}
  ~CD() {}
  bool detect(GBase *tA, GBase *tB);
  bool detect(GBase *t, const Ray& r);
};





#endif
