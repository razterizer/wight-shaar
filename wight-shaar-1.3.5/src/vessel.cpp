#include "ship.h"

float inertia_determinant(const Matrix& m_) {
  Matrix m(m_);
  float det;
  det = m[X][X]*(m[Y][Y]*m[Z][Z] - m[Z][Y]*m[Y][Z])
    - m[X][Y]*(m[Y][X]*m[Z][Z] - m[Z][X]*m[Y][Z])
    + m[X][Z]*(m[Y][X]*m[Z][Y] - m[Z][X]*m[Y][Y]);  
  return det;
}

Matrix inertia_inverse(const Matrix& I_) throw(overflow_error) {
  Matrix I(I_);
  Matrix I_inv;
  float eps=0.0005;

  if (I(X,Y)==0 and I(Y,X)==0 and
      I(X,Z)==0 and I(Z,X)==0 and
      I(Y,Z)==0 and I(Z,Y)==0) {

    if (I(X,X)<eps or I(Y,Y)<eps or I(Z,Z)<eps)
      throw overflow_error("inertia_inverse(const Matrix&)");

    I_inv(X,X) = 1/I(X,X);
    I_inv(Y,Y) = 1/I(Y,Y);
    I_inv(Z,Z) = 1/I(Z,Z);
  }
  else {

    float det = inertia_determinant(I);

    if (det<eps)
      throw overflow_error("inertia_inverse(const Matrix&)");

    I_inv(X,X) =  (I(Y,Y)*I(Z,Z) - I(Y,Z)*I(Z,Y))/det;
    I_inv(X,Y) = -(I(X,Y)*I(Z,Z) - I(Z,Y)*I(X,Z))/det;
    I_inv(X,Z) =  (I(X,Y)*I(Y,Z) - I(Y,Y)*I(X,Z))/det;
    
    I_inv(Y,X) = -(I(Y,X)*I(Z,Z) - I(Y,Z)*I(Z,X))/det;
    I_inv(Y,Y) =  (I(X,X)*I(Z,Z) - I(Z,X)*I(X,Z))/det;
    I_inv(Y,Z) = -(I(X,X)*I(Y,Z) - I(Y,X)*I(X,Z))/det;
    
    I_inv(Z,X) =  (I(Y,X)*I(Z,Y) - I(Z,X)*I(Y,Y))/det;
    I_inv(Z,Y) = -(I(X,X)*I(Z,Y) - I(Z,X)*I(X,Y))/det;
    I_inv(Z,Z) =  (I(X,X)*I(Y,Y) - I(X,Y)*I(Y,X))/det;
  }
  
  return I_inv;
}





Vessel :: Vessel(const string& n)
  : GBase(n), thruster_list(new Thruster[0]), Nt(0), m(0)
{
  bind_rotation(&s.rot());
  bind_translation(&s.pos());
  dynamic = true;
}

Vessel :: Vessel(const Matrix& I, State s0, unsigned Nt, float m,
	     const string& n)
  : GBase(n), I(I), s(s0),
    thruster_list(new Thruster[Nt]), Nt(Nt),
    m(m)
{
  I_inv=inertia_inverse(I);
  bind_rotation(&s.rot());
  bind_translation(&s.pos());
  dynamic = true;
}

Vessel :: Vessel(const Matrix& I, State s0, unsigned Nt, float m,
	     const string& path, const string& wfobj,
	     const string& n)
  : GBase(n), I(I), s(s0),
    thruster_list(new Thruster[Nt]), Nt(Nt),
    m(m) 
{
  I_inv=inertia_inverse(I);
  bind_rotation(&s.rot());
  bind_translation(&s.pos());
  dynamic = true;
  import(path, wfobj);
}

Vessel :: Vessel(const Vessel& sh)   //Does NOT perform deep copy. Use the add-method
  : GBase(sh),
    I(sh.I), I_inv(sh.I_inv), s(sh.s), 
    Nt(sh.Nt),  //thruster references are preserved, since geometry data are shallow copied.
    m(m)
{
  I_inv=inertia_inverse(I);
  bind_rotation(&s.rot());      //renew references
  bind_translation(&s.pos());
  dynamic = true;
  thruster_list = new Thruster[Nt];
  for (unsigned i=0; i<Nt; i++)
    thruster_list[i] = sh.thruster_list[i];
}

Vessel :: ~Vessel() {
  delete[] thruster_list;
}



GTYPE Vessel :: type(void) const {
  return VESSEL;
}

Matrix& Vessel :: inertia(const Matrix& m) {
  I=m;
  I_inv=inertia_inverse(m);
  return I;
}

const Matrix& Vessel :: inertia(void) const {
  return I;
}

const Matrix& Vessel :: inertia_inv(void) const {
  return I_inv;
}

Thruster& Vessel :: thruster(unsigned i) throw(out_of_range) {
  if (i>=Nt)
    throw out_of_range("Thruster::thruster(unsigned)");
  return thruster_list[i];
}

int Vessel :: num_thrusters(int n) {
  delete[] thruster_list;
  thruster_list=new Thruster[Nt=n];

  return Nt;
}

int Vessel :: num_thrusters(void) const {
  return Nt;
}

void Vessel :: bind_thruster(unsigned i, GBase& n, const Vector& piv) throw(out_of_range) {
  if (i>=Nt)
    throw out_of_range("Vessel::bind_thruster(unsigned, const string&)");

  thruster_list[i].bind(&n, piv);
}

float Vessel :: mass(float m) {
  this->m=m;
  return m;
}

float Vessel :: mass(void) const {
  return m;
}

State& Vessel :: state(const State& s) {
  this->s=s;
  return this->s;
  dynamic = true;
  updated = true;
}

State& Vessel :: state(void) {
  return s;
}

void Vessel :: render(void) {
  GBase::render();    //translation/rotation is bound to state-variable
                      //therefore we can invoke the parent's renderer

  //Matrix mat; mat.translate(s.pos());
  //(mat*s.rot()*obj).render();           //SLOOOOOOOOOOOW!!!!!
}


Ray Vessel :: ray(void) {
  return Ray(s.pos(), s.rot()*Vector(0,0,-1), 0);
}
