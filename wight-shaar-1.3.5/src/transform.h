// -*-c++-*-
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <GL/glut.h>

#include "geometry.h"
#include "bb.h"
#include "stuff.h"


typedef enum {CS_BODY, CS_WORLD} CoordSys;


class Matrix {
  static const int ROWS=4, COLS=4;
  float mat[ROWS][COLS];
  /*
  float xx,xy,xz,xw,
    yx, yy, yz, yw,
    zx, zy, zz, zw,
    wx, wy, wz, ww;
  */
public:
  Matrix() {*this=identity();}
  Matrix(float m[ROWS][COLS]);
  Matrix(GLfloat *glm);             //OpenGL-matrix
  Matrix(GLdouble *glm);            //OpenGL-matrix
  Matrix identity(void) const;
  Vertex get_row(int i) const;
  Vertex get_col(int j) const;
  const Matrix& transpose(void);
  //const Matrix& inverse(void);
  const Matrix& translate(const Vector& v);
  const Matrix& translate(const Vertex& v);
  const Matrix& translate(float x, float y, float z);
  const Matrix& scale(const Vector& v);
  const Matrix& scale(float x, float y, float z);
  const Matrix& rotate(float pitch, float yaw, float roll);  //avoid (obsolete)
  const Matrix& rotate(const Vector& v, float ang);          //avoid (obsolete)
  const Matrix& rotate(float x, float y, float z, float ang);//avoid (obsolete)
  friend ostream& operator<< (ostream& o, const Matrix& m);
  bool operator== (const Matrix& m) const;
  bool operator!= (const Matrix& m) const;
  const Matrix& operator+= (const Matrix& m);
  const Matrix& operator+= (float d);
  const Matrix& operator-= (const Matrix& m);
  const Matrix& operator-= (float d);
  const Matrix& operator*= (const Matrix& m);
  const Matrix& operator*= (float d);
  const Matrix& operator/= (float d);
  Matrix operator+ (const Matrix& m) const;
  Matrix operator+ (float d) const;
  friend Matrix operator+ (float d, const Matrix& m);
  Matrix operator- (const Matrix& m) const;
  Matrix operator- (float d) const;
  friend Matrix operator- (float d, const Matrix& m);
  Matrix operator* (const Matrix& m) const;
  Matrix operator* (float d) const;
  friend Matrix operator* (float d, const Matrix& m);
  Vertex operator* (const Vertex& v) const;
  Vector operator* (const Vector& v) const;
  Poly operator* (const Poly& p) const;
  BB operator* (const BB& b) const;
  Matrix operator/ (float d) const;
  Matrix operator+ () const;
  Matrix operator- () const;
  Vertex operator[] (int i);
  float& operator() (int i, int j);
  void begin(void) const;
  void end(void) const;
};



class Quaternion : public Vector {
public:
  Quaternion()
    : Vector(0,0,0,1) {}
  Quaternion(float x, float y, float z)
    : Vector(x,y,z,1) {}
  //Quaternion(float s, float x, float y, float z)
  //  : Vector(x,y,z,s) {}
  Quaternion(const Vector& v)
    : Vector(v) {}
  Quaternion(float s, const Vector& v);
  Quaternion(const Matrix& m) {set(m);}
  Quaternion conj(void) const;
  Quaternion inv(void) const;
  float norm(void) const;
  float length(void) const;
  const Quaternion& normalize(void);
  Quaternion operator! () const;
  const Quaternion& rotate(const Vector& v, float ang);
  float angle(void) const;
  Vector axis(void) const;
  Vector euler(void) const;
  void vec(const Vector& v);
  Quaternion dot(const Vector& angvel, CoordSys cs) const;
  Matrix get(void) const;
  void set(const Matrix& m);
  friend ostream& operator<< (ostream& o, const Quaternion& q);
  const Quaternion& operator+= (const Quaternion& q);
  const Quaternion& operator-= (const Quaternion& q);
  const Quaternion& operator*= (const Quaternion& q);
  const Quaternion& operator*= (float d);
  const Quaternion& operator/= (const Quaternion& q);
  const Quaternion& operator/= (float d);
  Quaternion operator+ (const Quaternion& q) const;
  Quaternion operator- (const Quaternion& q) const;
  Quaternion operator* (const Quaternion& q) const;
  Quaternion operator* (float d) const;
  friend Quaternion operator* (float d, const Quaternion& q);
  Matrix operator* (const Matrix& m) const;
  friend Matrix operator* (const Matrix& m, const Quaternion& q);
  Vertex operator* (const Vertex& v) const;
  Vector operator* (const Vector& v) const;
  Poly operator* (const Poly& p) const;
  Quaternion operator/ (const Quaternion& q) const;
  Quaternion operator/ (float d) const;
  friend Quaternion operator/ (float d, const Quaternion& q);
  const Quaternion& operator() (float s, const Vector& v);
  void begin(void) const;
};



#endif
