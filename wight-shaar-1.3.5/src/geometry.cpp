#include "geometry.h"


/*********************************************
 *                  Texel                    *
 *********************************************/

void Texel :: set(float x, float y) {
  this->x=x;
  this->y=y;
}

ostream& operator<< (ostream& o, const Texel& t) {
  o << '('
    << t.x << ", "
    << t.y << ')';
  return o;
}

bool Texel :: operator== (const Texel& t) const {
  return x==t.x and y==t.y;
}

bool Texel :: operator!= (const Texel& t) const {
  return not (*this==t);
}

const Texel& Texel :: operator+= (const Texel& t) {
  x+=t.x;
  y+=t.y;
  return *this;
}

const Texel& Texel :: operator+= (float d) {
  x+=d;
  y+=d;
  return *this;
}

const Texel& Texel :: operator-= (const Texel& t) {
  x-=t.x;
  y-=t.y;
  return *this;
}

const Texel& Texel :: operator-= (float d) {
  x-=d;
  y-=d;
  return *this;
}

const Texel& Texel :: operator*= (float d) {
  x*=d;
  y*=d;
  return *this;
}

const Texel& Texel :: operator/= (float d) throw(overflow_error) {
  if (!d) throw overflow_error("Texel::operator/=");
  float a=1.0f/d;
  x*=a;
  y*=a;
  return *this;
}

Texel Texel :: operator+ (const Texel& t) const {
  Texel tmp(*this);
  tmp += t;
  return tmp;
}

Texel Texel :: operator+ (float d) const {
  Texel tmp(*this);
  tmp += d;
  return tmp;
}

Texel operator+ (float d, const Texel& t) {
  Texel tmp(t);
  tmp += d;
  return tmp;
}

Texel Texel :: operator- (const Texel& t) const {
  Texel tmp(*this);
  tmp -= t;
  return tmp;
}

Texel Texel :: operator- (float d) const {
  Texel tmp(*this);
  tmp -= d;
  return tmp;
}

Texel operator- (float d, const Texel& t) {
  Texel tmp(t);
  tmp -= d;
  return tmp;
}

Texel Texel :: operator* (float d) const {
  Texel tmp(*this);
  tmp *= d;
  return tmp;
}

Texel operator* (float d, const Texel& t) {
  Texel tmp(t);
  tmp *= d;
  return tmp;
}

Texel Texel :: operator/ (float d) const {
  Texel tmp(*this);
  tmp /= d;
  return tmp;
}

Texel Texel :: operator+ () const {
  return *this;
}

Texel Texel :: operator- () const {
  Texel tmp(*this);
  tmp.x = -tmp.x;
  tmp.y = -tmp.y;
  return tmp;
}

float Texel :: operator[] (int i) const throw(out_of_range) {
  switch (i){
  case 0:
    return x;
    break;
  case 1:
    return y;
    break;
  default:
    throw out_of_range("Texel::operator[](int)");
  }
}

float& Texel :: operator[] (int i) throw(out_of_range) {
  switch (i){
  case 0:
    return x;
    break;
  case 1:
    return y;
    break;
  default:
    throw out_of_range("Texel::operator[](int)");
  }
}

void Texel :: render(void) const {
  glTexCoord2f(x,y);
}

/*********************************************
 *                  Vertex                   *
 *********************************************/

void Vertex :: set(float x, float y, float z) {
  this->x=x;
  this->y=y;
  this->z=z;
}

void Vertex :: set(float x, float y, float z, float w) {
  this->x=x;
  this->y=y;
  this->z=z;
  this->w=w;
}

ostream& operator<< (ostream& o, const Vertex& v) {
  o << '('
    << v.x << ", "
    << v.y << ", "
    << v.z << ", "
    << v.w << ')';
  return o;
}

bool Vertex :: operator== (const Vertex& v) const {
  return x==v.x and y==v.y and z==v.z;
}

bool Vertex :: operator!= (const Vertex& v) const {
  return not (*this==v);
}

const Vertex& Vertex :: operator+= (const Vertex& v) {
  x+=v.x;
  y+=v.y;
  z+=v.z;
  return *this;
}

const Vertex& Vertex :: operator+= (float d) {
  x+=d;
  y+=d;
  z+=d;
  return *this;
}

const Vertex& Vertex :: operator-= (const Vertex& v) {
  x-=v.x;
  y-=v.y;
  z-=v.z;
  return *this;
}

const Vertex& Vertex :: operator-= (float d) {
  x-=d;
  y-=d;
  z-=d;
  return *this;
}

const Vertex& Vertex :: operator^= (const Vertex& v) {
  *this = *this ^ v;
  return *this;
}

const Vertex& Vertex :: operator*= (float d) {
  x*=d;
  y*=d;
  z*=d;
  return *this;
}

const Vertex& Vertex :: operator/= (float d) throw(overflow_error) {
  if (!d) throw overflow_error("Vertex::operator/=");
  float a=1.0f/d;
  x*=a;
  y*=a;
  z*=a;
  return *this;
}

Vertex Vertex :: operator^ (const Vertex& v) const {
  Vertex tmp(y*v.z-v.y*z, z*v.x-v.z*x, x*v.y-v.x*y);
  return tmp;
}

float Vertex :: operator* (const Vertex& v) const {
  return x*v.x + y*v.y + z*v.z + w*v.w;
}

Vertex Vertex :: operator* (float d) const {
  Vertex tmp(*this);
  tmp *= d;
  return tmp;
}

Vertex operator* (float d, const Vertex& v) {
  Vertex tmp(v);
  tmp *= d;
  return tmp;
}

Vertex Vertex :: operator/ (float d) const {
  Vertex tmp(*this);
  tmp /= d;
  return tmp;
}

Vertex Vertex :: operator+ (const Vertex& v) const {
  Vertex tmp(*this);
  tmp += v;
  return tmp;
}

Vertex Vertex :: operator+ (float d) const {
  Vertex tmp(*this);
  tmp += d;
  return tmp;
}

Vertex operator+ (float d, const Vertex& v) {
  Vertex tmp(v);
  tmp += d;
  return tmp;
}

Vertex Vertex :: operator- (const Vertex& v) const {
  Vertex tmp(*this);
  tmp -= v;
  return tmp;
}

Vertex Vertex :: operator- (float d) const {
  Vertex tmp(*this);
  tmp -= d;
  return tmp;
}

Vertex operator- (float d, const Vertex& v) {
  Vertex tmp(v);
  tmp.x = d-tmp.x;
  tmp.y = d-tmp.y;
  tmp.z = d-tmp.z;
  return tmp;
}

Vertex Vertex :: operator+ () const {
  return *this;
}

Vertex Vertex :: operator- () const {
  Vertex tmp(*this);
  tmp.x = -tmp.x;
  tmp.y = -tmp.y;
  tmp.z = -tmp.z;
  return tmp;
}

float Vertex :: operator[] (int i) const throw(out_of_range) {
  switch (i){
  case X:
    return x;
    break;
  case Y:
    return y;
    break;
  case Z:
    return z;
    break;
  case W:
    return w;
    break;
  default:
    throw out_of_range("Vertex::operator[](int)");
  }
}

float& Vertex :: operator[] (int i) throw(out_of_range) {
  switch (i){
  case X:
    return x;
    break;
  case Y:
    return y;
    break;
  case Z:
    return z;
    break;
  case W:
    return w;
    break;
  default:
    throw out_of_range("Vertex::operator[](int)");
  }
}

void Vertex :: render(void) const {
  glVertex3f(x,y,z);
}

/*********************************************
 *                  Vector                   *
 *********************************************/

float Vector :: length(void) const {
  return sqrt(x*x+y*y+z*z);  // + w*w
}

const Vector& Vector :: normalize(void) {
  *this = *this/this->length();
  return *this;
}

Vector Vector :: operator! () const {
  Vector tmp(*this);
  return tmp.normalize();
}

const Vector& Vector :: operator+= (const Vector& v) {
  return *this=Vertex::operator+=(v);
}

const Vector& Vector :: operator+= (float d) {
  return *this=Vertex::operator+=(d);
}

const Vector& Vector :: operator-= (const Vector& v) {
  return *this=Vertex::operator-=(v);
}

const Vector& Vector :: operator-= (float d) {
  return *this=Vertex::operator-=(d);
}

const Vector& Vector :: operator^= (const Vector& v) {
  return *this=Vertex::operator^=(v);
}

const Vector& Vector :: operator*= (float d) {
  return *this=Vertex::operator*=(d);
}

const Vector& Vector :: operator/= (float d) {
  return *this=Vertex::operator/=(d);
}

bool Vector :: operator< (const Vector& v) const {
  Vector v1(*this), v2(v);
  return v1.length()<v2.length();
}

bool Vector :: operator> (const Vector& v) const {
  Vector v1(*this), v2(v);
  return v1.length()>v2.length();
}

bool Vector :: operator>= (const Vector& v) const {
  return not (*this<v);
}

bool Vector :: operator<= (const Vector& v) const {
  return not (*this>v);
}

Vector Vector :: operator^ (const Vector& v) const {
  return Vertex::operator^(v);
}

float Vector :: operator* (const Vector& v) const {
  return Vertex::operator*(v);
}

Vector Vector :: operator* (float d) const {
  return Vertex::operator*(d);
}

Vector operator* (float d, const Vector& v) {
  Vector tmp(v);
  tmp *= d;
  return tmp;
}

Vector Vector :: operator/ (float d) const {
  return Vertex::operator/(d);
}

Vector Vector :: operator+ (const Vector& v) const {
  return Vertex::operator+(v);
}

Vector Vector :: operator+ (float d) const {
  return Vertex::operator+(d);
}

Vector operator+ (float d, const Vector& v) {
  Vector tmp(v);
  tmp += d;
  return tmp;
}

Vector Vector :: operator- (const Vector& v) const {
  return Vertex::operator-(v);
}

Vector Vector :: operator- (float d) const {
  return Vertex::operator-(d);
}

Vector operator- (float d, const Vector& v) {
  Vector tmp(v);
  tmp.x = d-tmp.x;
  tmp.y = d-tmp.y;
  tmp.z = d-tmp.z;
  return tmp;
}

Vector Vector :: operator+ () const {
  return *this;
}

Vector Vector :: operator- () const {
  return Vertex::operator-();
}

float Vector :: operator[] (int i) const {
  return Vertex::operator[](i);
}

float& Vector :: operator[] (int i) {
  return Vertex::operator[](i);
}

void Vector :: render(void) const {
  glVertex4f(x,y,z,w);
}

void Vector :: draw(const Vertex& v) const {
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  glColor3f(1,1,0);
  glVertex3f(v[X], v[Y], v[Z]);
  glVertex3f(v[X]+x, v[Y]+y, v[Z]+z);
  glEnd();
  glEnable(GL_LIGHTING);
}

void Vector :: begin(void) const {
  glPushMatrix();
  glTranslatef(x,y,z);
}

void Vector :: end(void) const {
  glPopMatrix();
}


/*********************************************
 *                  Color                    *
 *********************************************/

void Color :: set(float r, float g, float b, float a) {
  x=r;
  y=g;
  z=b;
  w=a;
}

void Color :: set(const GLfloat *rgba) {
  x=rgba[R];
  y=rgba[G];
  z=rgba[B];
  w=rgba[A];
}

const Color& Color :: normalize(void) {
  float max=MAX4(x,y,z,w);
  if (max>1){
    float a=1.0f/max;
    x*=a;
    y*=a;
    z*=a;
    w*=a;
  }
  return *this;
}

float& Color :: red(void) { return x;}

float& Color :: green(void) { return y;}

float& Color :: blue(void) { return z;}

float& Color :: alpha(void) { return w;}

Vector Color :: hsva(void) const {
  float r=x, g=y, b=z, a=w;
  float v,xx,f,h,s;
  int i;

  xx = MIN3(r,g,b);
  v = MAX3(r,g,b);
  if (v==xx){
    Vector hsva(NAN, 0, v, a);
    return hsva;
  }
  f = (r==xx)? g-b : ((g==xx)? b-r : r-g);
  i = (r==xx)? 3 : ((g==xx)? 5:1);
  h = i-f/(v-xx);
  h /= 6.0f;     // h=0..6 -> h=0..1
  s = (v-xx)/v;
  Vector hsva(h, s, v, a);
  return hsva;
}

void Color :: hsva(const Vector& hsva) {
  hsv(hsva);
  w=hsva[A];
}

void Color :: hsv(const Vector& hsv_) {
  Vector hsv(hsv_);
  this->hsv(hsv[0], hsv[1], hsv[2]);
}

void Color :: hsva(const float *hsva) {
  hsv(hsva);
  w=hsva[A];
}

void Color :: hsv(const float *hsv) {
  this->hsv(hsv[0], hsv[1], hsv[2]);
}

void Color :: hsva(float h, float s, float v, float a) {
  hsv(h,s,v);
  w=a;
}

void Color :: hsv(float h, float s, float v) {
  float m,n,f;
  int i;
  
  h*=6.0f;   // h=0..1 -> h=0..6
  if (h==NAN) x=y=z=v;
  i=(int)h;
  f=h-i;
  if (!(i&1)) f=1-f;
  m=v*(1-s);
  n=v*(1-s*f);
  switch (i){
  case 6:
  case 0: x=v; y=n; z=m; break;
  case 1: x=n; y=v; z=m; break;
  case 2: x=m; y=v; z=n; break;
  case 3: x=m; y=n; z=v; break;
  case 4: x=n; y=m; z=v; break;
  case 5: x=v; y=m; z=n;
  }
}

float Color :: hue(void) const {
  Vector hsv_=hsva();
  return hsv_[0];
}

float Color :: saturation(void) const {
  Vector hsv_=hsva();
  return hsv_[1];
}

float Color :: value(void) const {
  Vector hsv_=hsva();
  return hsv_[2];
}

GLenum Color :: render(GLenum type) const throw(invalid_argument) {
  GLfloat c[4];
  c[R]=x;
  c[G]=y;
  c[B]=z;
  c[A]=w;
  if (type==GL_AMBIENT or type==GL_DIFFUSE or
      type==GL_SPECULAR or type==GL_EMISSION){
    glMaterialfv(GL_FRONT, type, c);
  } else if (type==GL_COLOR)
    glColor4f(x,y,z,w);
  else
    throw invalid_argument("Color::render(GLenum type)");
  return type;
}


/*********************************************
 *                    Poly                   *
 *********************************************/

Poly :: Poly(const Poly& p)
  : fn(p.fn), A_f(p.A_f), smooth(p.smooth),
    store_vertex_normals(p.store_vertex_normals),
    store_texels(p.store_texels)
{
  v=vector<Vertex>(p.v);
  t=vector<Texel>(p.t);
  vn=vector<Vector>(p.vn);
}

// PRIVATE
void Poly :: eval_normal(void) {
  fn = Vector();
  if (size()){
    for (unsigned i=0;i<size();i++){
      fn += v[i] ^ v[(i+1)%size()];
    }
    fn /= 2.0f;
  }
  A_f=fn.length();  //surface area;
  try {
    fn.normalize();
  }
  catch (overflow_error) {}
}

void Poly :: add(const Vertex& v_)
  throw(logic_error)
{
  if (not store_vertex_normals and not store_texels){
    v.push_back(v_);
    eval_normal();
    vn.push_back(Vector());
    t.push_back(Texel());
  }
  else
    throw logic_error("Poly::add(const Vertex&)");
}

void Poly :: add(const Vertex& v_, const Vector& vn_)
  throw(logic_error)
{
  if (!size())
    store_vertex_normals = true;

  if (store_vertex_normals and not store_texels){
    v.push_back(v_);
    eval_normal();
    vn.push_back(vn_);
    t.push_back(Texel());
  }
  else
    throw logic_error("Poly::add(const Vertex&, const Vector&)");
}

void Poly :: add(const Vertex& v_, const Texel& t_)
  throw(logic_error)
{
  if (!size())
    store_texels = true;

  if (not store_vertex_normals and store_texels){
    v.push_back(v_);
    eval_normal();
    vn.push_back(Vector());
    t.push_back(t_);
  }
  else
    throw logic_error("Poly::add(const Vertex&, const Texel&)");
}

void Poly :: add(const Vertex& v_, const Vector& vn_, const Texel& t_)
  throw(logic_error)
{
  if (!size()){
    store_vertex_normals = true;
    store_texels = true;
  }

  if (store_vertex_normals and store_texels){
    v.push_back(v_);
    eval_normal();
    vn.push_back(vn_);
    t.push_back(t_);
  }
  else
    throw logic_error("Poly::add(Vertex&, Vector&, Texel&)");
}

void Poly :: remove(void) {
  v.pop_back();
  eval_normal();
  vn.pop_back();
  t.pop_back();
}

void Poly :: remove(int i) {
  v.erase(v.begin()+i);
  eval_normal();
  vn.erase(vn.begin()+i);
  t.erase(t.begin()+i);
}

unsigned Poly :: size(void) const { return v.size();}

const Vector& Poly :: get_normal(void) const { return fn;}

void Poly :: set_normal(const Vector& v_){
  fn=v_;
}

GLenum Poly :: shading(GLenum mode) throw(invalid_argument) {
  switch (mode){
  case GL_FLAT:
    smooth=false;
    break;
  case GL_SMOOTH:
    smooth=true;
    break;
  default:
    throw invalid_argument("Poly::shading(GLenum)");
  }
  return mode;
}

GLenum Poly :: shading(void) const {
  if (smooth)
    return GL_SMOOTH;
  return GL_FLAT;
}

void Poly :: render(void) {
  if (size()==3)
    glBegin(GL_TRIANGLES);
  else if (size()==4)
    glBegin(GL_QUADS);
  else
    glBegin(GL_POLYGON);

  if (not smooth and fn.length()>.8)
    glNormal3f(fn[X], fn[Y], fn[Z]);
  for (unsigned i=0;i<size();i++){
    if (smooth and store_vertex_normals)
      glNormal3f(vn[i][X], vn[i][Y], vn[i][Z]);
    if (store_texels)
      t[i].render();
    v[i].render();
  }

  glEnd();
}

Vertex Poly :: centre(void) const {
  Vertex tmp;
  for (unsigned i=0;i<size();i++)
    tmp += v[i];
  tmp /= size();
  return tmp;
}

float Poly :: area(void) const { return A_f;}

Vertex& Poly :: operator[] (int i) throw(out_of_range) {
  if (i<0 or i>=(int)size())
    throw out_of_range("Poly::operator[](int)");
  return v[i];
}

void Poly :: invert(void) {
  Vertex tmp;
  for (unsigned i=0;i<size()/2;i++){
    tmp=v[(size()-1)-i];
    v[(size()-1)-i]=v[i];
    v[i]=tmp;
  }
  eval_normal();
}

ostream& operator<< (ostream& o, const Poly& p){
  Poly tmp(p);
  for (unsigned i=0;i<tmp.size();i++)
    o << "v[" << i << "] = " << tmp[i] << endl;
  return o;
}



/*********************************************
 *                    Ray                    *
 *********************************************/

ostream& operator<< (ostream& o, const Ray& r) {
  o << r.o << " -> " << r.d << "*(t=" << r.t << ')';
  return o;
}

Vertex Ray :: origin(const Vertex& orig) {
  return o = orig;
}

Vertex Ray :: origin(void) const {
  return o;
}

Vector Ray :: direction(const Vector& dir) {
  return d = dir;
}

Vector Ray :: direction(void) const {
  return d;
}

float Ray :: time(float t_) {
  return t = t_;
}

float Ray :: time(void) const {
  return t;
}
