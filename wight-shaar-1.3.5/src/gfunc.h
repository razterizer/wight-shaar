// -*-c++-*-
#ifndef GFUNC_H
#define GFUNC_H

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


//It should be more appropritate to also inherit from a general leaf-class
//"GLeaf". Maybe l8r.

class GFunc : public GBase {
protected:
  void (*func)(void);             //object (function ptr)
  unsigned damage;
private:  //administrative stuff
  void recompile(void);
private:
  GFunc();
  void add(void) {}
  void remove(void) {}
  unsigned size(void) {return 0;}
public:
  GFunc(void (*f)(void), const BB& box, const string& n);
  GFunc(const GFunc& go);
  GFunc(GBase& gb);
  ~GFunc();
  GTYPE type(void) const;
  void set_texture(const string& tfile);
  void render(void);
  void vfc_render(void);
  void set_damage(unsigned pts);
  unsigned get_damage(void) const;
  void hit(unsigned dp, bool kill=false);
};



#endif
