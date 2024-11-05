// -*-c++-*-
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <GL/glut.h>

#include "stuff.h"

#define X 0
#define Y 1
#define Z 2
#define W 3
#define R 0
#define G 1
#define B 2
#define A 3




using namespace std;


class Texel {
protected:
  float x,y;
public:
  Texel()
    : x(0), y(0) {}
  Texel(float x, float y)
    : x(x), y(y) {}
  void set(float x, float y);
  friend ostream& operator<< (ostream& o, const Texel& t);
  bool operator== (const Texel& t) const;
  bool operator!= (const Texel& t) const;
  const Texel& operator+= (const Texel& t);
  const Texel& operator+= (float d);
  const Texel& operator-= (const Texel& t);
  const Texel& operator-= (float d);
  const Texel& operator*= (float d);
  const Texel& operator/= (float d) throw(overflow_error);
  Texel operator+ (const Texel& t) const;
  Texel operator+ (float d) const;
  friend Texel operator+ (float d, const Texel& t);
  Texel operator- (const Texel& t) const;
  Texel operator- (float d) const;
  friend Texel operator- (float d, const Texel& t);
  Texel operator* (float d) const;
  friend Texel operator* (float d, const Texel& t);
  Texel operator/ (float d) const;
  Texel operator+ () const;
  Texel operator- () const;
  float operator[] (int i) const throw(out_of_range);
  float& operator[] (int i) throw(out_of_range);
  void render(void) const;
};



class Vertex : public Texel {
protected:
  float z,w;
public:
  Vertex()
    : z(0), w(1) {}
  Vertex(float x, float y, float z)
    : Texel(x,y), z(z), w(1) {}
  Vertex(float x, float y, float z, float w)
    : Texel(x,y), z(z), w(w) {}
  void set(float x, float y, float z);  //primitive access (obsolete)
  void set(float x, float y, float z, float w);
  friend ostream& operator<< (ostream& o, const Vertex& v);
  bool operator== (const Vertex& v) const;
  bool operator!= (const Vertex& v) const;
  const Vertex& operator+= (const Vertex& v);
  const Vertex& operator+= (float d);
  const Vertex& operator-= (const Vertex& v);
  const Vertex& operator-= (float d);
  const Vertex& operator^= (const Vertex& v);
  const Vertex& operator*= (float d);
  const Vertex& operator/= (float d) throw(overflow_error);
  Vertex operator^ (const Vertex& v) const; // (x,y,z) x (x,y,z)
  float operator* (const Vertex& v) const;  // (x,y,z) dot (x,y,z)
  Vertex operator* (float d) const;
  friend Vertex operator* (float d, const Vertex& v);
  Vertex operator/ (float d) const;
  Vertex operator+ (const Vertex& v) const;
  Vertex operator+ (float d) const;
  friend Vertex operator+ (float d, const Vertex& v);
  Vertex operator- (const Vertex& v) const;
  Vertex operator- (float d) const;
  friend Vertex operator- (float d, const Vertex& v);
  Vertex operator+ () const;
  Vertex operator- () const;
  float operator[] (int i) const throw(out_of_range);
  float& operator[] (int i) throw(out_of_range);
  void render(void) const;
};


class Vector : public Vertex {
public:
  Vector()
    : Vertex(0,0,0,1) {}
  Vector(float x, float y, float z)
    : Vertex(x,y,z,0) {}
  Vector(float x, float y, float z, float w)
    : Vertex(x,y,z,w) {}
  Vector(const Vertex& v)
    : Vertex(v) {w=0;}
  float length(void) const;
  const Vector& normalize(void);
  Vector operator! () const;
  const Vector& operator+= (const Vector& v);
  const Vector& operator+= (float d);
  const Vector& operator-= (const Vector& v);
  const Vector& operator-= (float d);
  const Vector& operator^= (const Vector& v);
  const Vector& operator*= (float d);
  const Vector& operator/= (float d);
  bool operator< (const Vector& v) const;
  bool operator> (const Vector& v) const;
  bool operator<= (const Vector& v) const;
  bool operator>= (const Vector& v) const;
  Vector operator^ (const Vector& v) const; // (x,y,z) x (x,y,z)
  float operator* (const Vector& v) const;  // (x,y,z) dot (x,y,z)
  Vector operator* (float d) const;
  friend Vector operator* (float d, const Vector& v);
  Vector operator/ (float d) const;
  Vector operator+ (const Vector& v) const;
  Vector operator+ (float d) const;
  friend Vector operator+ (float d, const Vector& v);
  Vector operator- (const Vector& v) const;
  Vector operator- (float d) const;
  friend Vector operator- (float d, const Vector& v);
  Vector operator+ () const;
  Vector operator- () const;
  float operator[] (int i) const;
  float& operator[] (int i);
  void render(void) const;
  void draw(const Vertex& v) const;
  void begin(void) const;
  void end(void) const;
};


class Color : public Vector {
  Vector::draw;             //won't be needing this
public:
  Color()
    : Vector(0,0,0,0) {}
  Color(float r, float g, float b)
    : Vector(r,g,b,0) {}
  Color(float r, float g, float b, float a)
    : Vector(r,g,b,a) {}
  Color(const Vector& rgba)
    : Vector(rgba) {}
  Color(const float *rgba) { set(rgba);}
  void set(float r, float g, float b, float a);
  void set(const GLfloat *rgba);
  const Color& normalize(void);
  float& red(void);
  float& green(void);
  float& blue(void);
  float& alpha(void);
  Vector hsva(void) const;
  void hsva(const Vector& hsva);
  void hsv(const Vector& hsv);
  void hsva(const float *hsva);
  void hsv(const float *hsv);
  void hsva(float h, float s, float v, float a);
  void hsv(float h, float s, float v);
  float hue(void) const;
  float saturation(void) const;
  float value(void) const;
  GLenum render(GLenum type) const throw(invalid_argument);
};



class Poly {
private:
  vector<Vertex> v;
  vector<Texel> t;
private:
  vector<Vector> vn;
  Vector fn;
  float A_f;
  bool smooth;
  bool store_vertex_normals;   //flag for error checking
  bool store_texels;           //flag for error checking
  void eval_normal(void);
public:
  Poly() :
    fn(Vector()), A_f(0), smooth(false),
    store_vertex_normals(false), store_texels(false) {}
  Poly(const Poly& p);
  void add(const Vertex& v)
    throw(logic_error);
  void add(const Vertex& v, const Vector& vn)
    throw(logic_error);
  void add(const Vertex& v, const Texel& t)
    throw(logic_error);
  void add(const Vertex& v, const Vector& vn, const Texel& t)
    throw(logic_error);
  void remove(void);
  void remove(int i);
  unsigned size(void) const;
  const Vector& get_normal(void) const;
  void set_normal(const Vector& v);
  void render(void);
  GLenum shading(GLenum mode) throw(invalid_argument);
  GLenum shading(void) const;
  Vertex centre(void) const;
  float area(void) const;
  void invert(void);
  Vertex& operator[] (int i) throw(out_of_range);
  friend ostream& operator<< (ostream& o, const Poly& p);
};


class Ray {
private:
  Vertex o;
  Vector d;
  float t;
public:
  Ray()
    : o(Vertex()), d(Vector()), t(0) {}
  Ray(const Vertex& origin, const Vector& direction, float time)
    : o(origin), d(direction), t(time) {}
  Ray(const Ray& r)
    : o(r.o), d(r.d), t(r.t) {}
  friend ostream& operator<< (ostream& o, const Ray& r);
  Vertex origin(const Vertex& orig);
  Vertex origin(void) const;
  Vector direction(const Vector& dir);
  Vector direction(void) const;
  float time(float t);
  float time(void) const;
};


#endif
