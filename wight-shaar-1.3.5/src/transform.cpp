#include "transform.h"


/*********************************************
 *                  Matrix                   *
 *********************************************/

Matrix :: Matrix(float m[ROWS][COLS]) {
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j]=m[i][j];
}

Matrix :: Matrix(GLfloat *glm) {
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j]=glm[j*ROWS+i];
}

Matrix :: Matrix(GLdouble *glm) {
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j]=glm[j*ROWS+i];
}

Matrix Matrix :: identity(void) const {
  float tmp[ROWS][COLS];
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      tmp[i][j] = i==j;
  return Matrix(tmp);
}

Vertex Matrix :: get_row(int i) const {
  Vertex tmp;
  if (i<0 or i>=ROWS)
    throw out_of_range("Matrix::get_row(int)");
  tmp.set(mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
  return tmp;
}

Vertex Matrix :: get_col(int j) const {
  Vertex tmp;
  if (j<0 or j>=COLS)
    throw out_of_range("Matrix::get_col(int)");
  tmp.set(mat[0][j], mat[1][j], mat[2][j], mat[3][j]);
  return tmp;
}

const Matrix& Matrix :: transpose(void) {
  Matrix tmp(*this);
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j] = tmp.mat[j][i];
  return *this;
}

//const Matrix& Matrix :: inverse(void) {
//}

const Matrix& Matrix :: translate(const Vector& v) {
  Vector v_(v);  
  this->translate(v_[X], v_[Y], v_[Z]);
  /*
  Matrix tmp;
  tmp.mat[X][W] = v_[X];
  tmp.mat[Y][W] = v_[Y];
  tmp.mat[Z][W] = v_[Z];
  *this = tmp * (*this);
  */
  return *this;
}

const Matrix& Matrix :: translate(const Vertex& v) {
  Vector v_(v);
  this->translate(v_[X], v_[Y], v_[Z]);  
  /*
  Matrix tmp;
  tmp.mat[X][W] = v_[X];
  tmp.mat[Y][W] = v_[Y];
  tmp.mat[Z][W] = v_[Z];
  *this = tmp * (*this);
  */
  return *this;
}

const Matrix& Matrix :: translate(float x, float y, float z) {
  Matrix tmp;
  tmp.mat[X][W] = x;
  tmp.mat[Y][W] = y;
  tmp.mat[Z][W] = z;
  *this = tmp * (*this);
  return *this;
}

const Matrix& Matrix :: scale(const Vector& v) {
  Vector v_(v);
  Matrix tmp;
  tmp.mat[X][X] = v_[X];
  tmp.mat[Y][Y] = v_[Y];
  tmp.mat[Z][Z] = v_[Z];
  *this = tmp * (*this);
  return *this;
}

const Matrix& Matrix :: scale(float x, float y, float z) {
  Matrix tmp;
  tmp.mat[X][X] = x;
  tmp.mat[Y][Y] = y;
  tmp.mat[Z][Z] = z;
  *this = tmp * (*this);
  return *this;
}

const Matrix& Matrix :: rotate(float pitch, float yaw, float roll) {
  Matrix tmp;
  float angle_x=pitch*d2r;
  float angle_y=yaw*d2r;
  float angle_z=roll*d2r;
  float a = cos(angle_x);
  float b = sin(angle_x);
  float c = cos(angle_y);
  float d = sin(angle_y);
  float e = cos(angle_z);
  float f = sin(angle_z);
  tmp.mat[X][X] = c*e;
  tmp.mat[X][Y] = -c*f;
  tmp.mat[X][Z] = -d;
  tmp.mat[Y][X] = -b*d*e+a*f;
  tmp.mat[Y][Y] = b*d*f+a*e;
  tmp.mat[Y][Z] = -b*c;
  tmp.mat[Z][X] = a*d*e+b*f;
  tmp.mat[Z][Y] = -a*d*f+b*e;
  tmp.mat[Z][Z] = a*c;
  *this = tmp * (*this);
  return *this;
}

const Matrix& Matrix :: rotate(const Vector& v, float ang) {
  Matrix tmp;
  ang*=d2r;
  /*
		  ( xx(1-c)+c	xy(1-c)-zs  xz(1-c)+ys	 0  )
		  |					    |
		  | yx(1-c)+zs	yy(1-c)+c   yz(1-c)-xs	 0  |
		  | xz(1-c)-ys	yz(1-c)+xs  zz(1-c)+c	 0  |
		  |					    |
		  (	 0	     0		 0	 1  )
  */
  float c=cos(ang);
  float s=sin(ang);
  Vector ax(v);
  ax.normalize();
  float c1 = 1-c;
  float x = v[X], y = v[Y], z = v[Z];
  float xx1c = x*x*c1;
  float xy1c = x*y*c1;
  float xz1c = x*z*c1;
  float yy1c = y*y*c1;
  float yz1c = y*z*c1;
  float zz1c = z*z*c1;
  float zs = z*s;
  float ys = y*s;
  float xs = x*s;
  tmp.mat[X][X] = xx1c+c;  tmp.mat[X][Y] = xy1c-zs; tmp.mat[X][Z] = xz1c+ys;
  tmp.mat[Y][X] = xy1c+zs; tmp.mat[Y][Y] = yy1c+c;  tmp.mat[Y][Z] = yz1c-xs;
  tmp.mat[Z][X] = xz1c-ys; tmp.mat[Z][Y] = yz1c+xs; tmp.mat[Z][Z] = zz1c+c;
  *this = tmp * (*this);
  return *this;
}

const Matrix& Matrix :: rotate(float x, float y, float z, float ang) {
  Vector v(x,y,z);
  return rotate(v, ang);
}

ostream& operator<< (ostream& o, const Matrix& m) {
  for (int i=0;i<m.ROWS;i++){
    o << '[';
    for (int j=0;j<m.COLS;j++){
      o << m.mat[i][j];
      if (j<m.COLS-1)
	o << ", ";
    }
    o << ']';
    if (i<m.ROWS-1)
      o << endl;
  }
  return o;
}

bool Matrix :: operator== (const Matrix& m) const {
  bool flag=1;
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      flag &= mat[i][j]==m.mat[i][j];
  return flag;
}

bool Matrix :: operator!= (const Matrix& m) const {
  return not (*this==m);
}

const Matrix& Matrix :: operator+= (const Matrix& m) {
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j] += m.mat[i][j];
  return *this;
}

const Matrix& Matrix :: operator+= (float d) {
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j] += d;
  return *this;
}

const Matrix& Matrix :: operator-= (const Matrix& m) {
  return *this += -m;
}

const Matrix& Matrix :: operator-= (float d) {
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j] -= d;
  return *this;
}

const Matrix& Matrix :: operator*= (const Matrix& m) {
  Matrix m1(*this);
  Matrix m2(m);
  //Optimized(?)
  mat[X][X]=m1.mat[X][X]*m2.mat[X][X]+m1.mat[X][Y]*m2.mat[Y][X]+
    m1.mat[X][Z]*m2.mat[Z][X]+m1.mat[X][W]*m2.mat[W][X];
  mat[X][Y]=m1.mat[X][X]*m2.mat[X][Y]+m1.mat[X][Y]*m2.mat[Y][Y]+
    m1.mat[X][Z]*m2.mat[Z][Y]+m1.mat[X][W]*m2.mat[W][Y];
  mat[X][Z]=m1.mat[X][X]*m2.mat[X][Z]+m1.mat[X][Y]*m2.mat[Y][Z]+
    m1.mat[X][Z]*m2.mat[Z][Z]+m1.mat[X][W]*m2.mat[W][Z];
  mat[X][W]=m1.mat[X][X]*m2.mat[X][W]+m1.mat[X][Y]*m2.mat[Y][W]+
    m1.mat[X][Z]*m2.mat[Z][W]+m1.mat[X][W]*m2.mat[W][W];

  mat[Y][X]=m1.mat[Y][X]*m2.mat[X][X]+m1.mat[Y][Y]*m2.mat[Y][X]+
    m1.mat[Y][Z]*m2.mat[Z][X]+m1.mat[Y][W]*m2.mat[W][X];
  mat[Y][Y]=m1.mat[Y][X]*m2.mat[X][Y]+m1.mat[Y][Y]*m2.mat[Y][Y]+
    m1.mat[Y][Z]*m2.mat[Z][Y]+m1.mat[Y][W]*m2.mat[W][Y];
  mat[Y][Z]=m1.mat[Y][X]*m2.mat[X][Z]+m1.mat[Y][Y]*m2.mat[Y][Z]+
    m1.mat[Y][Z]*m2.mat[Z][Z]+m1.mat[Y][W]*m2.mat[W][Z];
  mat[Y][W]=m1.mat[Y][X]*m2.mat[X][W]+m1.mat[Y][Y]*m2.mat[Y][W]+
    m1.mat[Y][Z]*m2.mat[Z][W]+m1.mat[Y][W]*m2.mat[W][W];

  mat[Z][X]=m1.mat[Z][X]*m2.mat[X][X]+m1.mat[Z][Y]*m2.mat[Y][X]+
    m1.mat[Z][Z]*m2.mat[Z][X]+m1.mat[Z][W]*m2.mat[W][X];
  mat[Z][Y]=m1.mat[Z][X]*m2.mat[X][Y]+m1.mat[Z][Y]*m2.mat[Y][Y]+
    m1.mat[Z][Z]*m2.mat[Z][Y]+m1.mat[Z][W]*m2.mat[W][Y];
  mat[Z][Z]=m1.mat[Z][X]*m2.mat[X][Z]+m1.mat[Z][Y]*m2.mat[Y][Z]+
    m1.mat[Z][Z]*m2.mat[Z][Z]+m1.mat[Z][W]*m2.mat[W][Z];
  mat[Z][W]=m1.mat[Z][X]*m2.mat[X][W]+m1.mat[Z][Y]*m2.mat[Y][W]+
    m1.mat[Z][Z]*m2.mat[Z][W]+m1.mat[Z][W]*m2.mat[W][W];

  mat[W][X]=m1.mat[W][X]*m2.mat[X][X]+m1.mat[W][Y]*m2.mat[Y][X]+
    m1.mat[W][Z]*m2.mat[Z][X]+m1.mat[W][W]*m2.mat[W][X];
  mat[W][Y]=m1.mat[W][X]*m2.mat[X][Y]+m1.mat[W][Y]*m2.mat[Y][Y]+
    m1.mat[W][Z]*m2.mat[Z][Y]+m1.mat[W][W]*m2.mat[W][Y];
  mat[W][Z]=m1.mat[W][X]*m2.mat[X][Z]+m1.mat[W][Y]*m2.mat[Y][Z]+
    m1.mat[W][Z]*m2.mat[Z][Z]+m1.mat[W][W]*m2.mat[W][Z];
  mat[W][W]=m1.mat[W][X]*m2.mat[X][W]+m1.mat[W][Y]*m2.mat[Y][W]+
    m1.mat[W][Z]*m2.mat[Z][W]+m1.mat[W][W]*m2.mat[W][W];
  /*
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j] = m1.get_row(i)*m2.get_col(j);
  */
  return *this;
}

const Matrix& Matrix :: operator*= (float d) {
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j] *= d;
  return *this;
}

const Matrix& Matrix :: operator/= (float d) {
  float a=1.0f/d;
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      mat[i][j] *= a;
  return *this;
}

Matrix Matrix :: operator+ (const Matrix& m) const {
  Matrix tmp(*this);
  return tmp += m;
}

Matrix Matrix :: operator+ (float d) const {
  Matrix tmp(*this);
  return tmp += d;
}

Matrix operator+ (float d, const Matrix& m) {
  Matrix tmp(m);
  return tmp += d;
}

Matrix Matrix :: operator- (const Matrix& m) const {
  Matrix tmp(*this);
  return tmp -= m;
}

Matrix Matrix :: operator- (float d) const {
  Matrix tmp(*this);
  return tmp -= d;
}

Matrix operator- (float d, const Matrix& m) {
  Matrix tmp(m);
  return tmp += d;
}

Matrix Matrix :: operator* (const Matrix& m) const {
  Matrix tmp(*this);
  return tmp *= m;
}

Matrix Matrix :: operator* (float d) const {
  Matrix tmp(*this);
  return tmp *= d;
}

Matrix operator* (float d, const Matrix& m) {
  Matrix tmp(m);
  return tmp *= d;
}

Vertex Matrix :: operator* (const Vertex& v) const {
  Vertex tmp;
  for (int i=0;i<ROWS;i++)
    tmp[i] = get_row(i)*v;
  return tmp;
}

Vector Matrix :: operator* (const Vector& v) const {
  return Vector(*this * Vertex(v));
}

Poly Matrix :: operator* (const Poly& p) const {
  Poly tmp(p);
  for (unsigned i=0;i<tmp.size();i++)
    tmp[i] = Vertex((*this)*Vector(tmp[i]));
  return tmp;
}

BB Matrix :: operator* (const BB& b) const {
  float
    xx1=mat[X][X]*b.xmin,
    xx2=mat[X][X]*b.xmax,
    yx1=mat[Y][X]*b.xmin,
    yx2=mat[Y][X]*b.xmax,
    zx1=mat[Z][X]*b.xmin,
    zx2=mat[Z][X]*b.xmax,
    x=mat[X][W];
  float
    xy1=mat[X][Y]*b.ymin,
    xy2=mat[X][Y]*b.ymax,
    yy1=mat[Y][Y]*b.ymin,
    yy2=mat[Y][Y]*b.ymax,
    zy1=mat[Z][Y]*b.ymin,
    zy2=mat[Z][Y]*b.ymax,
    y=mat[Y][W];
  float
    xz1=mat[X][Z]*b.zmin,
    xz2=mat[X][Z]*b.zmax,
    yz1=mat[Y][Z]*b.zmin,
    yz2=mat[Y][Z]*b.zmax,
    zz1=mat[Z][Z]*b.zmin,
    zz2=mat[Z][Z]*b.zmax,
    z=mat[Z][W];

  float xx[8] = {
    xx1 + xy1 + xz1,
    xx2 + xy1 + xz1,
    xx1 + xy2 + xz1,
    xx2 + xy2 + xz1,
    xx1 + xy1 + xz2,
    xx2 + xy1 + xz2,
    xx1 + xy2 + xz2,
    xx2 + xy2 + xz2
  };
  float yy[8] = {
    yx1 + yy1 + yz1,
    yx2 + yy1 + yz1,
    yx1 + yy2 + yz1,
    yx2 + yy2 + yz1,
    yx1 + yy1 + yz2,
    yx2 + yy1 + yz2,
    yx1 + yy2 + yz2,
    yx2 + yy2 + yz2
  };
  float zz[8] = {
    zx1 + zy1 + zz1,
    zx2 + zy1 + zz1,
    zx1 + zy2 + zz1,
    zx2 + zy2 + zz1,
    zx1 + zy1 + zz2,
    zx2 + zy1 + zz2,
    zx1 + zy2 + zz2,
    zx2 + zy2 + zz2
  };

  BB out;
  out.xmin=out.ymin=out.zmin=INF;
  out.xmax=out.ymax=out.zmax=-INF;

  for (unsigned i=0; i<8; i++){
    out.xmin=MIN2(out.xmin, xx[i]);
    out.xmax=MAX2(out.xmax, xx[i]);
    out.ymin=MIN2(out.ymin, yy[i]);
    out.ymax=MAX2(out.ymax, yy[i]);
    out.zmin=MIN2(out.zmin, zz[i]);
    out.zmax=MAX2(out.zmax, zz[i]);
  }
  out.xmin+=x; out.xmax+=x;
  out.ymin+=y; out.ymax+=y;
  out.zmin+=z; out.zmax+=z;

  return out;
}

Matrix Matrix :: operator/ (float d) const {
  Matrix tmp(*this);
  tmp /= d;
  return tmp;
}

Matrix Matrix :: operator+ () const {
  return *this;
}

Matrix Matrix :: operator- () const {
  Matrix tmp(*this);
  for (int i=0;i<ROWS;i++)
    for (int j=0;j<COLS;j++)
      tmp.mat[i][j] = -tmp.mat[i][j];
  return tmp;
}

Vertex Matrix :: operator[] (int i) {
  return get_row(i);
}

float& Matrix :: operator() (int i, int j) {
  if (i<0 or i>ROWS-1 or j<0 or j>COLS-1)
    throw out_of_range("Matrix::operator()(int,int)");
  return mat[i][j];
}

void Matrix :: begin(void) const {
  glPushMatrix();
  if (mat[X][Y] or mat[Y][Z] or mat[Z][X]){
    float glMat[ROWS*COLS];
    for (int i=0;i<ROWS;i++)
      for (int j=0;j<COLS;j++)
	glMat[j*ROWS+i]=mat[i][j];
    glMultMatrixf(glMat);
  }
  else {
    glTranslatef(mat[X][W], mat[Y][W], mat[Z][W]);
    glScalef(mat[X][X], mat[Y][Y], mat[Z][Z]);
  }
}

void Matrix :: end(void) const {
  glPopMatrix();
}


/*********************************************
 *                  Quaternion               *
 *********************************************/

Quaternion :: Quaternion(float s, const Vector& v_) {
  Vector v(v_);
  x=v[X]; y=v[Y]; z=v[Z]; w=s;
}

Quaternion Quaternion :: conj(void) const {
  return Quaternion(w, Vector(-x,-y,-z));
}

Quaternion Quaternion :: inv(void) const {
  return this->conj()/this->norm();
}

float Quaternion :: norm(void) const {
  return x*x+y*y+z*z+w*w;
}

float Quaternion :: length(void) const {
  return sqrt(norm());
}

const Quaternion& Quaternion :: normalize(void) {
  if (float a=length()){
    float b=1.0f/a;
    x*=b;
    y*=b;
    z*=b;
    w*=b;
  }
  return *this;
}

Quaternion Quaternion :: operator! () const {
  Quaternion tmp(*this);
  return tmp.normalize();
}

const Quaternion& Quaternion :: rotate(const Vector& v, float ang) {
  float a=ang/2.0f;
  float Sa = sin(a);
  float Ca = cos(a);
  Vector ax(v);
  ax.normalize();
  x=ax[X];
  y=ax[Y];
  z=ax[Z];
  x*=Sa;
  y*=Sa;
  z*=Sa;
  w=Ca;
  normalize();
  return *this;
}

float Quaternion :: angle(void) const {
  Quaternion q(*this);
  q.normalize();
  return acos(q.w)*2;
}

Vector Quaternion :: axis(void) const {
  Quaternion q(*this);
  q.normalize();
  float Sa = sqrt(1-q.w*q.w);
  if (fabs(Sa)<.0005f)
    Sa=1;
  float invSa=1.0f/Sa;
  Vector ax(q.x*invSa, q.y*invSa, q.z*invSa);
  return ax;
}

Vector Quaternion :: euler(void) const {
  Vector out;
  float y2=y*y;
  out[X]=atan((2.0f*(w*x+y*z))/(1.0f-2.0f*(x*x+y2))); //roll
  out[Y]=asin(2.0f*(w*y-z*x));                        //pitch
  out[Z]=atan((2.0f*(w*z+x*y))/(1.0f-2.0f*(y2+z*z))); //yaw
  return out;
}

void Quaternion :: vec(const Vector& v) {
  x=v[X];
  y=v[Y];
  z=v[Z];
}

Quaternion Quaternion :: dot(const Vector& angvel,
			     CoordSys cs) const {
  Quaternion q(*this);
  q.normalize();
  Quaternion qw(0, (cs==CS_BODY)? (q*angvel) : angvel);
  return .5f*(qw)*(q);
}

Matrix Quaternion :: get(void) const {
  float mat[4][4];
  float xx2=2*x*x,
    xy2=2*x*y,
    xz2=2*x*z,
    xw2=2*x*w,
    yy2=2*y*y,
    yz2=2*y*z,
    yw2=2*y*w,
    zz2=2*z*z,
    zw2=2*z*w;

  mat[X][X]=1-yy2-zz2;
  mat[Y][Y]=1-zz2-xx2;
  mat[Z][Z]=1-xx2-yy2;

  mat[X][Y]=mat[Y][X]=xy2;
  mat[X][Z]=mat[Z][X]=xz2;
  mat[Y][Z]=mat[Z][Y]=yz2;

  mat[X][Y]-=zw2; mat[Y][X]+=zw2;
  mat[X][Z]+=yw2; mat[Z][X]-=yw2;
  mat[Y][Z]-=xw2; mat[Z][Y]+=xw2;

  mat[X][W]=mat[Y][W]=mat[Z][W]=
    mat[W][X]=mat[W][Y]=mat[W][Z]=0;
  mat[W][W]=1;

  return Matrix(mat);
}

void Quaternion :: set(const Matrix& m) {
  Matrix mat(m);
  float xx=mat[X][X], yy=mat[Y][Y], zz=mat[Z][Z];

  float trace = xx+yy+zz+1;

  if (trace>0){
    float s = .5f/sqrt(trace);
    x = (mat[Z][Y]-mat[Y][Z])*s;
    y = (mat[X][Z]-mat[Z][X])*s;
    z = (mat[Y][X]-mat[X][Y])*s;
    w = .25f/s;
  }
  else {  //trade speed for accuracy
    float max=MAX3(xx,yy,zz);
    if (xx==max){
      float s = 2.0f*sqrt(1+xx-yy-zz);
      x = .5f/s;
      y = (mat[X][Y]+mat[Y][X])/s;
      z = (mat[X][Z]+mat[Z][X])/s;
      w = (mat[Y][Z]+mat[Z][Y])/s;
    } else if (yy==max){
      float s = 2.0f*sqrt(1+yy-zz-xx);
      x = (mat[X][Y]+mat[Y][X])/s;
      y = .5f/s;
      z = (mat[Y][Z]+mat[Z][Y])/s;
      w = (mat[X][Z]+mat[Z][X])/s;
    } else {
      float s = 2.0f*sqrt(1+zz-xx-yy);
      x = (mat[X][Z]+mat[Z][X])/s;
      y = (mat[Y][Z]+mat[Z][Y])/s;
      z = .5f/s;
      w = (mat[X][Y]+mat[Y][X])/s;
    }
  }
}

ostream& operator<< (ostream& o, const Quaternion& q_) {
  Quaternion q(q_);
  o << '(' << q[W] << ", ";
  o << '('
    << q[X] << ", "
    << q[Y] << ", "
    << q[Z]
    << ')';
  o << ')';
  return o;

}

const Quaternion& Quaternion :: operator+= (const Quaternion& q) {
  x += q.x;
  y += q.y;
  z += q.z;
  w += q.w;
  return *this;
}

const Quaternion& Quaternion :: operator-= (const Quaternion& q) {
  x -= q.x;
  y -= q.y;
  z -= q.z;
  w -= q.w;
  return *this;
}

const Quaternion& Quaternion :: operator*= (const Quaternion& q) {
  Quaternion v1(*this);
  Quaternion v2(q);
  //v1.normalize();
  //v2.normalize();
  w = v1.w * v2.w -
    v1.x * v2.x -
    v1.y * v2.y -
    v1.z * v2.z;
  x = v1.w * v2.x +
    v1.x * v2.w +
    v1.y * v2.z -
    v1.z * v2.y;
  y = v1.w * v2.y +
    v1.y * v2.w +
    v1.z * v2.x -
    v1.x * v2.z;
  z = v1.w * v2.z +
    v1.z * v2.w +
    v1.x * v2.y -
    v1.y * v2.x;
  //normalize();
  return *this;
}

const Quaternion& Quaternion :: operator*= (float d) {
  //this->Vertex::operator*=(d);
  this->x *= d;
  this->y *= d;
  this->z *= d;
  this->w *= d;
  return *this;
}

const Quaternion& Quaternion :: operator/= (const Quaternion& q) {
  *this *= this->inv();
  return *this;
}

const Quaternion& Quaternion :: operator/= (float d){
  this->Vertex::operator/=(d);
  return *this;
}

Quaternion Quaternion :: operator+ (const Quaternion& q) const {
  Quaternion tmp(*this);
  tmp += q;
  return tmp;
}

Quaternion Quaternion :: operator- (const Quaternion& q) const {
  Quaternion tmp(*this);
  tmp -= q;
  return tmp;
}

Quaternion Quaternion :: operator* (const Quaternion& q) const {
  Quaternion tmp(*this);
  tmp *= q;
  return tmp;
}

Quaternion Quaternion :: operator* (float d) const {
  Quaternion out(*this);
  out *= d;
  return out;
}

Quaternion operator* (float d, const Quaternion& q) {
  return q*d;
}

Matrix Quaternion :: operator* (const Matrix& m) const {
  Matrix tmp(m);
  return this->get()*tmp;
}

Matrix operator* (const Matrix& m, const Quaternion& q) {
  Matrix m_(m);
  Quaternion q_(q);
  return m_*q_.get();
}

Vertex Quaternion :: operator* (const Vertex& v) const {
  //return get()*v;
  Quaternion q(*this);
  Quaternion qv(0,v);
  Vertex tmp = q*qv*q.conj();
  tmp[W]=1;
  return tmp;
}

Vector Quaternion :: operator* (const Vector& v) const {
  return Vector(*this * Vertex(v));
}

Poly Quaternion :: operator* (const Poly& p) const {
  //return get()*p;
  Quaternion q(*this);
  Poly tmp(p);
  for (unsigned i=0; i<tmp.size(); i++)
    tmp[i]=q*tmp[i];
  return tmp;
}

Quaternion Quaternion :: operator/ (const Quaternion& q) const {
  Quaternion tmp(*this);
  tmp *= q.inv();
  return tmp;
}

Quaternion Quaternion :: operator/ (float d) const {
  Quaternion tmp(*this);
  tmp /= d;
  return tmp;
}

Quaternion operator/ (float d, const Quaternion& q) {
  Quaternion tmp(q);
  return d * tmp.inv();
}

const Quaternion& Quaternion :: operator() (float s, const Vector& v_) {
  Quaternion v(v_);
  x=v[X];
  y=v[Y];
  z=v[Z];
  w=s;
  return *this;
}

void Quaternion :: begin(void) const {
  //Matrix m = get();
  //m.begin();
  Vector ax=axis();

  glPushMatrix();
  glRotatef(angle()*r2d,ax[X],ax[Y],ax[Z]);
}

