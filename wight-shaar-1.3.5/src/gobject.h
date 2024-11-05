// -*-c++-*-
#ifndef GOBJECT_H
#define GOBJECT_H

#include <string>
#include <iostream>
#include <vector>
#include <ext/hash_map>
#include <stdexcept>
#include <GL/glut.h>

#include "geometry.h"
#include "transform.h"
#include "gbase.h"
#include "hashcmp.h"
#include "stuff.h"
//#include "timer.h"


//It should be more appropritate to also inherit from a general leaf-class
//"GLeaf". Maybe l8r.

class GObject : public GBase {
protected:
  vector<Poly> *object;             //object (polygon-list)
  unsigned damage;
private:  //administrative stuff
  bool free_object;             //shall we delete shared geometry?
  void make_obb(void);
  void update_obb(const Poly& p);    //faster on-the-fly version of make_bb
private:
  GObject();
public:
  void recompile(void);
  GObject(const string& n);
  GObject(const GObject& go);
  GObject(GBase& gb);
  ~GObject();
  GTYPE type(void) const;
  void add(const Poly& p, bool do_recompile=true); //add a polygon
  void remove(int p);           //remove a polygon
  unsigned size(void) const;
  void set_texture(const string& tfile);
  void render(void);
  void vfc_render(void);
  GLenum shading(GLenum mode);
  GLenum shading(void) const;
  void invert(void);
  Poly& operator[] (unsigned i);
  void free_upon_delete(bool flag);
  void set_damage(unsigned pts);
  unsigned get_damage(void) const;
  void hit(unsigned dp, bool kill=false);
};



#endif
