#include "cd.h"



/********************
 *     PRIVATE      *
 ********************/

bool CD :: overlap(const BB& b1, const BB& b2) {
  if (b1.xmax < b2.xmin or
      b2.xmax < b1.xmin)
    return false;
  if (b1.ymax < b2.ymin or
      b2.ymax < b1.ymin)
    return false;
  if (b1.zmax < b2.zmin or
      b2.zmax < b1.zmin)
    return false;

  return true;
}

//~7 us
bool CD :: intersect(const BB& b, Ray& r) {
  Vector d=r.direction();
  Vertex o=r.origin();
  if (d[X]==0.0f and (o[X]<b.xmin or b.xmax<o[X]))
    return false;
  if (d[Y]==0.0f and (o[Y]<b.ymin or b.ymax<o[Y]))
    return false;
  if (d[Y]==0.0f and (o[Y]<b.ymin or b.ymax<o[Y]))
    return false;

  float
    t1, t2, fubar,
    idx=1.0f/d[X], idy=1.0f/d[Y], idz=1.0f/d[Z],
    t_near, t_far;
  t_near=-INF; t_far=INF;
  r.time(t_far);

  //X
  t1=(b.xmin-o[X])*idx;
  t2=(b.xmax-o[X])*idx;
  if (t1>t2){
    fubar=t1;
    t1=t2;
    t2=fubar;
  }
  if (t1>t_near)
    t_near=t1;
  if (t2<t_far)
    t_far=t2;
  //Y
  t1=(b.ymin-o[Y])*idy;
  t2=(b.ymax-o[Y])*idy;
  if (t1>t2){
    fubar=t1;
    t1=t2;
    t2=fubar;
  }
  if (t1>t_near)
    t_near=t1;
  if (t2<t_far)
    t_far=t2;
  //Z
  t1=(b.zmin-o[Z])*idz;
  t2=(b.zmax-o[Z])*idz;
  if (t1>t2){
    fubar=t1;
    t1=t2;
    t2=fubar;
  }
  if (t1>t_near)
    t_near=t1;
  if (t2<t_far)
    t_far=t2;

  //HIT OR MISS THE BOX?
  if (t_near > t_far or t_far < 0)
    return false;
  r.time(t_near);
  return true;
}


/********************
 *     PUBLIC       *
 ********************/

bool CD :: detect(GBase *tA, GBase *tB) {
  if (not tA->is_active() or not tB->is_active())
    return false;
  if (not overlap(tA->get_aabb(), tB->get_aabb()))
    return false;
  if (tA->is_leaf())
    if (tB->is_leaf()) {
      if (tA!=tB and overlap(tA->get_aabb(), tB->get_aabb())) {
	if (action_aabb != NULL)
	  (*action_aabb)(tA,tB);
	return true;
      }
    }
    else {
      for (unsigned i=0; i<tB->size(); i++)
	if (detect(tA, &(*tB)[i]))
	  return true;
    }
  else
    for (unsigned i=0; i<tA->size(); i++)
      if (detect(&(*tA)[i], tB))
	return true;

  return false;
}

bool CD :: detect(GBase *tr, const Ray& r) {
  Ray ray(r);
  if (not tr->is_active())
    return false;
  if (not intersect(tr->get_aabb(), ray))
    return false;
  if (tr->is_leaf()) {
    if (intersect(tr->get_aabb(), ray)) {
      if (action_ray != NULL)
	(*action_ray)(tr, ray);
      return true;
    }
  }
  else
    for (unsigned i=0; i<tr->size(); i++)
      detect(&(*tr)[i], r);
      //if (detect(&(*tr)[i], r))
	//return true;
  
  return false;
}

