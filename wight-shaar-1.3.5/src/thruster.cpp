#include "thruster.h"



Thruster :: Thruster()
  : x(Vector()), F(Vector()), on(false),
    qx(Quaternion(1,Vector(0,0,0))),
    qy(Quaternion(1,Vector(0,0,0))),
    gimbal_pos(Vector()),
    rot_node(NULL)
{
  max_angles(90,90);
}

Thruster :: Thruster(const Vector& x, const Vector& F,
		     bool on, float max_ang_x, float max_ang_y)
  : x(x), F(F), on(on),
    qx(Quaternion(1,Vector(0,0,0))),
    qy(Quaternion(1,Vector(0,0,0))),
    gimbal_pos(Vector()),
    rot_node(NULL)
{
  max_angles(max_ang_x, max_ang_y);
}

bool Thruster :: enabled(void) const {
  return on;
}

void Thruster :: enabled(bool flag) {
  on = flag;
  if (rot_node!=NULL)
    on &= rot_node->is_active();
}

Vector Thruster :: force(void) const {
  return on? qx*qy*F:Vector();
}

void Thruster :: force(const Vector& f) {
  F=f;
}

Vector Thruster :: pos(void) const {
  return on? x:Vector();
}

void Thruster :: pos(const Vector& p) {
  x=p;
}


Vector Thruster :: torque(void) const {
  return on? (x^(qx*qy*F)):Vector();
}

void Thruster :: gimbal(float dx, float dy) {
  dx*=d2r;
  dy*=d2r;

  float ang_x=qx.angle(),
    ang_y=qy.angle();

  Vector ax_x=qx.axis(),
    ax_y=qy.axis();

  ang_x *= SIGN(ax_x[X]);
  ang_y *= SIGN(ax_y[Y]);

  if (-max_x<=(ang_x+dx) and (ang_x+dx)<=max_x)
    qx.rotate(Vector(1,0,0), ang_x+dx);

  if (-max_y<=(ang_y+dy) and (ang_y+dy)<=max_y)
    qy.rotate(Vector(0,1,0), ang_y+dy);

  rot_node->rotate(qx*qy);
}

void Thruster :: gimbal_reset(void) {
  qx(1,Vector(0,0,0));
  qy(1,Vector(0,0,0));
  rot_node->rotate(Quaternion());
}

void Thruster :: max_angles(float max_x, float max_y) {
  this->max_x=max_x*d2r;
  this->max_y=max_y*d2r;
}

float Thruster :: angle_x(void) const {
  Vector ax=qx.axis();
  return qx.angle()*r2d * ((float)SIGN(ax[X]));
}

float Thruster :: angle_y(void) const {
  Vector ax=qy.axis();
  return qy.angle()*r2d * ((float)SIGN(ax[Y]));
}

void Thruster :: bind(GBase *n, const Vector& piv) {
  rot_node=n;

  gimbal_pos = Vector(piv);            //rotational pivot position
  rot_node->rotate(gimbal_pos, qx*qy);
}
