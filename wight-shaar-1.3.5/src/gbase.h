// -*-c++-*-
#ifndef GBASE_H
#define GBASE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <ext/hash_map>
#include <cmath>
#include <stdexcept>
#include <GL/glut.h>

#include "geometry.h"
#include "transform.h"
#include "bb.h"
#include "stuff.h"
#include "texture.h"
#include "lists.h"



using namespace std;
using namespace __gnu_cxx;


class Ship;
class GBase;
GBase *copy_tree(GBase *c) throw(bad_typeid);    //implemented in gtree.cpp
GBase *import_obj(const string& path_, const string& filename)
    throw(exception);

typedef hash_map<string, GBase *, hash<string>, eqstr> NodeList;
typedef enum {GBASE, GOBJECT, GFUNC, VESSEL, SHIP, TORPEDO} GTYPE;
typedef enum {INSIDE, INTERSECT, OUTSIDE} CULLTYPE;
typedef enum {VF_LEFT=0, VF_RIGHT, VF_BOTTOM, VF_TOP, VF_NEAR, VF_FAR} VFTYPE;


class GBase {
protected:
  string name;
  unsigned ch_size;  //optimization
  unsigned ch_num;   //optimization?

  vector<GBase *> children;
  NodeList toc;
  vector<string> toc_names;
  GBase *ancestor;      //does this node belong to some important ancestor

  BB obb, aabb;
  Vertex bb_centre;

  Vector *pivot;
  bool pivot_heap;       //pointer to memory in heap?
  Quaternion *rotation;
  bool rotation_heap;
  Vector *translation;
  bool translation_heap;
  Matrix T_mat;          //an optimization

  bool active;           //false if object is "killed"
  bool updated;          //true if position/rotation is updated
  bool dynamic;          //true if node is bound (unknown updating)
protected:
  static Texture texture_lib;
  string texture;

  Color m_a, m_d, m_s, m_e;
  static const Color black, white;
  int S_e;

  GLuint displist_id;

  unsigned damage;

  void set_ancestor(GBase *a);

  CULLTYPE vf_cull(void);
  VFTYPE last_vf_plane;          //optimization, plane-coherence
  static Vector vf_planes[6];    //optimization, only calculate planes once.

  bool detail_cull(void);
  static Vertex eye_position;
  static Vector viewing_direction;
  static float near_plane;
  static float screen_width;
  static float screen_height;
  static float detail_limit;
  static bool use_detail_cull;

  static bool use_lighting;
private:
  GBase();  //hide the default constructor
  int name_id(const string& cn);
  static const Color box_color;


public:
  GBase(const string& n);
  GBase(const GBase& gb);
  virtual ~GBase();
public:
  const string& get_name(void) const;
  void set_name(const string& s) throw(invalid_argument);
  virtual GTYPE type(void) const;
  bool is_leaf(void) const;
  bool is_active(void) const;
  bool set_active(bool flag);
  GBase *get_ancestor(void) const;
  void set_ancestor(void);
  static Texture& tex_lib(void);
  virtual void set_texture(const string& tfile);
  string get_texture(void) const;
  virtual void render(void);
  virtual void vfc_render(void);
  static void set_vf(Matrix& vf); //calculate VF-planes (projection*worldview).
  //set dynamic detail culling params
  static void set_detail(Vertex eye_pos, Vector view_dir,
  			 float near, float width, float height);
  static void set_detail_limit(float lim);
  static bool detail_culling(bool flag);
  static bool lighting(bool flag);
  //add a leaf
  virtual void add(GBase *c) throw(invalid_argument);
  //copy tree into child
  virtual void add(GBase *c, const string& alt_cn) throw(invalid_argument);
  virtual void remove(unsigned ci) throw(out_of_range);
  virtual void remove(const string& cn) throw(out_of_range);
  virtual unsigned size(void) const;
  virtual void free_upon_delete(bool flag);
public:
  GBase& operator[] (const string& cn) throw(out_of_range);
  GBase& operator[] (unsigned ci) throw(out_of_range);
  friend ostream& operator<< (ostream& o, const GBase& n);
public:
  BB get_obb(void) const;
  BB get_aabb(void) const;
  void refresh_bb(void);
  void render_bb(void);
  Vertex get_bb_centre(void) const;
private:
  void init_bb(void);
  const BB& refresh_bb(Matrix Tp);
  Matrix transform(void);
  BB merge_bb(const BB& b1, const BB& b2);
public:
  void rotate(const Vector& p); //set rotation pivot
  void rotate(const Quaternion& r); //set rotation quat.
  void rotate(const Vector& p, const Quaternion& r);  //set rotation pivot and quaternion for this object
  void translate(const Vector& t);
  void bind_pivot(Vector *p);
  void unbind_pivot(void);
  void bind_rotation(Quaternion *r);
  void unbind_rotation(void);
  void bind_translation(Vector *t);
  void unbind_translation(void);
  void import(const string& path, const string& filename) throw(exception);
public:
  Color ambient(void) const;
  const Color& ambient(const Color& a);
  Color diffuse(void) const;
  const Color& diffuse(const Color& d);
  Color specular(void) const;
  const Color& specular(const Color& s);
  Color emission(void) const;
  const Color& emission(const Color& e);
  int& shininess(void);
  int shininess(int se);

  //inherited stuff
  virtual GLenum shading(GLenum mode);
  virtual GLenum shading(void) const;
  virtual void set_damage(unsigned pts);
  virtual unsigned get_damage(void) const;
  virtual void hit(unsigned dp, bool kill=false);
};


#endif
