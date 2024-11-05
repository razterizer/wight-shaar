#include "gbase.h"


/*********************************************
 *                   GBase                   *
 *********************************************/

Texture GBase :: texture_lib;

Vector GBase :: vf_planes[6];

Vertex GBase :: eye_position;
Vector GBase :: viewing_direction(0,0,-1);
float GBase :: near_plane=1e-1;
float GBase :: screen_width=640.0f;
float GBase :: screen_height=480.0f;
float GBase :: detail_limit=2.0f;
bool GBase :: use_detail_cull=false;

bool GBase :: use_lighting=true;

const Color GBase :: box_color(.3,.8,0);
const Color GBase :: black(0,0,0);
const Color GBase :: white(1,1,1);


int GBase :: name_id(const string& cn) {
  for (ch_num=0;ch_num<ch_size;ch_num++)
    if (toc_names[ch_num]==cn)
      return ch_num;
  return -1;
}

GBase :: GBase(const string& n)
  : name(n), ch_size(0), ancestor(NULL),
    bb_centre(Vertex(NAN,NAN,NAN)),
    pivot(new Vector()), pivot_heap(true),
    rotation(new Quaternion()), rotation_heap(true),
    translation(new Vector()), translation_heap(true),
    active(true), updated(false), dynamic(false),
    texture(""),
    m_a(1,1,1,1), m_d(1,1,1,1), m_s(1,1,1,1), m_e(0,0,0,1),
    S_e(30),
    displist_id(glGenLists(1)), //doesn't matter much if the displaylist is empty
    last_vf_plane(VF_NEAR)
{
  init_bb();
  T_mat=transform();
}

GBase :: GBase(const GBase& gb)
  : name(gb.name), ch_size(0), ancestor(NULL),
    obb(gb.obb), aabb(gb.aabb), bb_centre(gb.bb_centre),
    pivot(new Vector(*gb.pivot)), pivot_heap(true),
    rotation(new Quaternion(*gb.rotation)), rotation_heap(true),
    translation(new Vector(*gb.translation)), translation_heap(true),
    T_mat(gb.T_mat),
    active(gb.active),
    updated(gb.updated), dynamic(gb.dynamic),
    texture(gb.texture),
    m_a(gb.m_a), m_d(gb.m_d), m_s(gb.m_s), m_e(gb.m_e),
    S_e(gb.S_e),
    displist_id(gb.displist_id), //share display list
    last_vf_plane(gb.last_vf_plane)
{}

GBase :: ~GBase() {
  //cout << "deleting: " << name << endl;
  glDeleteLists(displist_id, 1);
  if (pivot_heap)
    delete pivot;
  if (rotation_heap)
    delete rotation;
  if (translation_heap)
    delete translation;

  for (ch_num=0;ch_num<ch_size;ch_num++)
    delete children[ch_num];
}

const string& GBase :: get_name(void) const {
  return name;
}

void GBase :: set_name(const string& s) throw(invalid_argument) {
  if (s=="") {
    cerr << "A node name cannot be empty." << endl;
    throw invalid_argument("GBase::set_name(const string&)");
  }
  name = s;
}

GTYPE GBase :: type(void) const {
  return GBASE;
}

bool GBase :: is_leaf(void) const {
  return !ch_size;
}

bool GBase :: is_active(void) const {
  return active;
}

bool GBase :: set_active(bool flag) {
  return active = flag;
}

GBase* GBase :: get_ancestor(void) const {
  return ancestor;
}

void GBase :: set_ancestor(void) {
  for (ch_num=0; ch_num<ch_size; ch_num++)
    children[ch_num]->set_ancestor(this);
}

//PROTECTED
void GBase :: set_ancestor(GBase *a) {
  ancestor = a;
  for (ch_num=0; ch_num<ch_size; ch_num++)
    children[ch_num]->set_ancestor(a);
}

Texture& GBase :: tex_lib(void) {
  return texture_lib;
}

void GBase :: set_texture(const string& tfile) {
  texture=tfile;
}

string GBase :: get_texture(void) const {
  return texture;
}

bool GBase :: lighting(bool flag) {
  return use_lighting = flag;
}

void GBase :: render(void) {
  if (use_detail_cull and detail_cull()) return;
  if (active) {
    if (dynamic) {
      translation->begin();                     //move object
         pivot->begin();                         //translate back
            rotation->begin();                   //rotate
               Vector v=(-*pivot);
	       v.begin();                       //center around pivot
    }
    for (ch_num=0; ch_num<ch_size; ch_num++)
         children[ch_num]->render();
    if (dynamic) {
               pivot->end();
            rotation->end();
         pivot->end();
      translation->end();
    }
  } 
}

void GBase :: vfc_render(void) {
  CULLTYPE cull=vf_cull();
  /*         //for testing purposes
  if (name=="enemy0"){
    cout << last_vf_plane << endl;
    switch(cull){
    case INSIDE:
      cout << "inside" << endl;
      break;
    case OUTSIDE:
      cout << "outside" << endl;
      break;
    case INTERSECT:
      cout << "intersect" << endl;
    }
  }
  */
  if (cull==OUTSIDE) return;
  if (use_detail_cull and detail_cull()) return;
  if (active) {
    if (dynamic) {
      translation->begin();                     //move object
         pivot->begin();                         //translate back
            rotation->begin();                   //rotate
               Vector v=(-*pivot);
	       v.begin();                       //center around pivot
    }
    if (cull==INSIDE)
      for (ch_num=0; ch_num<ch_size; ch_num++)
	children[ch_num]->render();
    else  //cull==INTERSECT
      for (ch_num=0; ch_num<ch_size; ch_num++)
	children[ch_num]->vfc_render();
    if (dynamic) {
               pivot->end();
            rotation->end();
         pivot->end();
      translation->end();
    }
  } 
}

void GBase :: set_vf(Matrix& vf) {
  //set the VF-planes only once per frame.
  vf_planes[VF_LEFT] = 
    Vector(vf(W,X)+vf(X,X), vf(W,Y)+vf(X,Y), vf(W,Z)+vf(X,Z), vf(W,W)+vf(X,W));
  vf_planes[VF_RIGHT] = 
    Vector(vf(W,X)-vf(X,X), vf(W,Y)-vf(X,Y), vf(W,Z)-vf(X,Z), vf(W,W)-vf(X,W));
  vf_planes[VF_BOTTOM] = 
    Vector(vf(W,X)+vf(Y,X), vf(W,Y)+vf(Y,Y), vf(W,Z)+vf(Y,Z), vf(W,W)+vf(Y,W));
  vf_planes[VF_TOP] = 
    Vector(vf(W,X)-vf(Y,X), vf(W,Y)-vf(Y,Y), vf(W,Z)-vf(Y,Z), vf(W,W)-vf(Y,W));
  vf_planes[VF_NEAR] = 
    Vector(vf(W,X)+vf(Z,X), vf(W,Y)+vf(Z,Y), vf(W,Z)+vf(Z,Z), vf(W,W)+vf(Z,W));
  vf_planes[VF_FAR] = 
    Vector(vf(W,X)-vf(Z,X), vf(W,Y)-vf(Z,Y), vf(W,Z)-vf(Z,Z), vf(W,W)-vf(Z,W));
}

void GBase :: set_detail(Vertex eye_pos, Vector view_dir,
			 float near_, float width, float height) {
  eye_position = eye_pos;
  viewing_direction = view_dir;
  near_plane = near_;
  screen_width = width;
  screen_height = height;
}

void GBase :: set_detail_limit(float lim) {
  detail_limit = lim;
}

bool GBase :: detail_culling(bool flag) {
  return use_detail_cull = flag;
}

//PROTECTED
CULLTYPE GBase :: vf_cull(void) {
  Vertex v[]={         //vertices to test
    Vertex(aabb.xmin, aabb.ymin, aabb.zmin),
    Vertex(aabb.xmax, aabb.ymin, aabb.zmin),
    Vertex(aabb.xmin, aabb.ymax, aabb.zmin),
    Vertex(aabb.xmax, aabb.ymax, aabb.zmin),
    Vertex(aabb.xmin, aabb.ymin, aabb.zmax),
    Vertex(aabb.xmax, aabb.ymin, aabb.zmax),
    Vertex(aabb.xmin, aabb.ymax, aabb.zmax),
    Vertex(aabb.xmax, aabb.ymax, aabb.zmax)
  };
  unsigned char in=0, out=0;
  //first test against the plane that previously rejected this aabb
  for (unsigned char j=0; j<8 and (!in or !out); j++)
    if (v[j]*vf_planes[last_vf_plane]<0)
      out++;
    else
      in++;
  if (!in)
    return OUTSIDE;
  if (out)
    return INTERSECT;

  //test against the remaining planes
  for (unsigned char i=0; i<6; i++)     //for each plane
    if (i != last_vf_plane){         //skip previously tested plane
      in=0; out=0;
      for (unsigned char j=0; j<8 and (!in or !out); j++)
	if (v[j]*vf_planes[i]<0)     //operand order is extremely important!
	  out++;
	else
	  in++;
      if (!in){  //if all corners are outside
	last_vf_plane = (VFTYPE)i;
	return OUTSIDE;
      }
      if (out){  //if some corners are outside
	last_vf_plane = (VFTYPE)i;
	return INTERSECT;
      }
    }
  return INSIDE;
}

//PROTECTED
bool GBase :: detail_cull(void) {
  Vector box_diag =
    Vertex(aabb.xmax, aabb.ymax, aabb.zmax)-
    Vertex(aabb.xmin, aabb.ymin, aabb.zmin);
  Vector viewing_distance = bb_centre - eye_position;

  float box_size = box_diag.length();

  float p = near_plane*box_size/
    (viewing_direction*viewing_distance);

  if (M_PI*p*p*screen_width*screen_height < detail_limit){
    //cout << name << endl;   //for debugging
    return true;
  }
  return false;
}

void GBase :: add(GBase *c) throw(invalid_argument) {
  if (c==NULL) {
    cerr << "Child is uninitialized." << endl;
    throw invalid_argument("GBase::add(GBase *)");
  }
  if ((c->type()==GOBJECT || c->type()==GFUNC) and not c->is_leaf()) {
    cerr << "Child \"" << c->name << "\" is not a leaf." << endl;
    throw invalid_argument("GBase::add(GBase *)");
  }
  if (not name_id(c->name)<0) {
    cerr << "Child with name \"" << c->name
	 << "\" already exists in this node." << endl;
    throw invalid_argument("GBase::add(GBase *)");
  }

  children.push_back(c);
  toc[c->name]=c;
  toc_names.push_back(c->name);
  ch_size++;
}

void GBase :: add(GBase *c, const string& alt_cn) throw(invalid_argument) {
  if (c==NULL) {
    cerr << "Child is uninitialized." << endl;
    throw invalid_argument("GBase::add(GBase *, const string&)");
  }
  if (alt_cn=="") {
    cerr << "A node name cannot be empty." << endl;
    throw invalid_argument("GBase::add(GBase *, const string&)");
  }
  if (not name_id(alt_cn)<0) {
    cerr << "Child with name \"" << alt_cn
	 << "\" already exists in this node." << endl;
    throw invalid_argument("GBase::add(GBase *, const string&)");
  }

  GBase *child=copy_tree(c);
  child->name=alt_cn;
  children.push_back(child);
  toc[alt_cn]=child;
  toc_names.push_back(alt_cn);
  ch_size++;
  if (child->type()==SHIP or child->type()==TORPEDO)
    child->set_ancestor();
}

void GBase :: remove(unsigned ci) throw(out_of_range) {
  if (ci>=ch_size) {
    cerr << "No child with index " << ci << "." << endl;
    throw out_of_range("GBase::remove(unsigned ci)");
  }
  delete children[ci];
  children.erase(children.begin()+ci);
  toc.erase(toc_names[ci]);
  toc_names.erase(toc_names.begin()+ci);
  ch_size--;
}

void GBase :: remove(const string& cn) throw(out_of_range) {
  int ci=name_id(cn);
  if (ci<0) {
    cerr << "No child named \"" << cn << "\"." << endl;
    throw out_of_range("GBase::remove(const string& cn)");
  }
  delete children[ci];
  children.erase(children.begin()+ci);
  toc.erase(cn);
  toc_names.erase(toc_names.begin()+ci);
  ch_size--;
}

unsigned GBase :: size(void) const {
  return ch_size;
}

void GBase :: free_upon_delete(bool flag) {
  for (ch_num=0; ch_num<ch_size; ch_num++)
    children[ch_num]->free_upon_delete(flag);
}

GBase& GBase :: operator[] (const string& cn) throw(out_of_range) {
  int ci=name_id(cn);
  if (ci<0) {
    cerr << "No child named \"" << cn << "\"." << endl;
    throw out_of_range("GBase::operator[](const string& cn)");
  }
  return *toc[cn];
}

GBase& GBase :: operator[] (unsigned ci) throw(out_of_range) {
  if (ci>=ch_size) {
    cerr << "No child with index " << ci << "." << endl;
    throw out_of_range("GBase::operator[](unsigned ci)");
  }
  return *children[ci];
}

ostream& operator<< (ostream& o, const GBase& n) {
  o << boolalpha;
  o << "name: " << n.name << endl
    << "type: " << n.type() << endl
    << "children: " << n.ch_size << endl
    << "pivot: " << *(n.pivot) << endl
    << "rotation: " << *(n.rotation) << endl
    << "translation: " << *(n.translation) << endl
    << "active: " << n.active << endl
    << "updated: " << n.updated << endl
    << "dynamic: " << n.dynamic;
  return o;
}

/************* BEGIN **************
 *                                *
 *      BOUNDING BOX ROUTINES     *
 **********************************/

BB GBase :: get_obb(void) const {
  return obb;
}

BB GBase :: get_aabb(void) const {
  return aabb;
}

void GBase :: refresh_bb(void) {
  aabb = refresh_bb(transform());
}

void GBase :: render_bb(void) {
  const BB& box = aabb;
  if (active) {
    if (use_lighting)
      glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    box_color.render(GL_COLOR);
    glBegin(GL_LINE_STRIP);
    glVertex3f(box.xmin, box.ymin, box.zmin);
    glVertex3f(box.xmax, box.ymin, box.zmin);
    glVertex3f(box.xmax, box.ymax, box.zmin);
    glVertex3f(box.xmin, box.ymax, box.zmin);
    glVertex3f(box.xmin, box.ymin, box.zmin);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(box.xmin, box.ymin, box.zmax);
    glVertex3f(box.xmax, box.ymin, box.zmax);
    glVertex3f(box.xmax, box.ymax, box.zmax);
    glVertex3f(box.xmin, box.ymax, box.zmax);
    glVertex3f(box.xmin, box.ymin, box.zmax);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(box.xmin, box.ymin, box.zmin);
    glVertex3f(box.xmin, box.ymin, box.zmax);
    glVertex3f(box.xmax, box.ymin, box.zmin);
    glVertex3f(box.xmax, box.ymin, box.zmax);
    glVertex3f(box.xmin, box.ymax, box.zmin);
    glVertex3f(box.xmin, box.ymax, box.zmax);
    glVertex3f(box.xmax, box.ymax, box.zmin);
    glVertex3f(box.xmax, box.ymax, box.zmax);
    glEnd();
    if (use_lighting)
      glEnable(GL_LIGHTING);
    white.render(GL_COLOR);
    for (ch_num=0; ch_num<ch_size; ch_num++)
      children[ch_num]->render_bb();
  }
}

Vertex GBase :: get_bb_centre(void) const {
  return bb_centre;
}

//PRIVATE:
void GBase :: init_bb(void) {
  obb.xmin=obb.ymin=obb.zmin=INF;
  obb.xmax=obb.ymax=obb.zmax=-INF;
  aabb=obb;
}

const BB& GBase :: refresh_bb(Matrix Tp) {
  if (updated or dynamic)
    T_mat = transform();  //optimization
  Matrix T = Tp * T_mat;
  if (ch_size)
    init_bb();
  for (ch_num=0; ch_num<ch_size; ch_num++)
    if (children[ch_num]->active)
      aabb = merge_bb(aabb, children[ch_num]->refresh_bb(T));
  if (!ch_size)  //is_leaf()
    aabb = T*obb;

  //if (use_detail_cull) //(not) only used in detail culling
  bb_centre = Vertex((aabb.xmin+aabb.xmax)*.5f,
		     (aabb.ymin+aabb.ymax)*.5f,
		     (aabb.zmin+aabb.zmax)*.5f);

  return aabb;
}

Matrix GBase :: transform(void) {
  Matrix mrot=rotation->get(),
    mpiv, m;

  float piv[3] = {(*pivot)[X], (*pivot)[Y], (*pivot)[Z]};
  mpiv(X,W) = -piv[X];
  mpiv(Y,W) = -piv[Y];
  mpiv(Z,W) = -piv[Z];

  m=mrot*mpiv;

  m(X,W) += piv[X] + (*translation)[X];
  m(Y,W) += piv[Y] + (*translation)[Y];
  m(Z,W) += piv[Z] + (*translation)[Z];

  return m;
}

BB GBase :: merge_bb(const BB& b1, const BB& b2) {
  BB out;
  out.xmin = MIN2(b1.xmin, b2.xmin);
  out.xmax = MAX2(b1.xmax, b2.xmax);
  out.ymin = MIN2(b1.ymin, b2.ymin);
  out.ymax = MAX2(b1.ymax, b2.ymax);
  out.zmin = MIN2(b1.zmin, b2.zmin);
  out.zmax = MAX2(b1.zmax, b2.zmax);
  return out;
}

/************** END ***************
 *                                *
 *      BOUNDING BOX ROUTINES     *
 **********************************/

void GBase :: rotate(const Vector& p) {
  *pivot=Vector(p);
  updated=true;
  dynamic=true;
}

void GBase :: rotate(const Quaternion& r) {
  *rotation=Quaternion(r);
  updated=true;
  dynamic=true;
}

void GBase :: rotate(const Vector& p, const Quaternion& r) {
  *pivot=Vector(p);
  *rotation=Quaternion(r);
  updated=true;
  dynamic=true;
}

void GBase :: translate(const Vector& t) {
  *translation=Vector(t);
  updated=true;
  dynamic=true;
}

void GBase :: bind_pivot(Vector *p) {
  if (pivot_heap)
    delete pivot;
  pivot = p;
  pivot_heap = false;
  dynamic = true;
  updated = true;      //always true since we don't know where its pointing
}

void GBase :: unbind_pivot(void) {
  if (not pivot_heap)
    pivot = new Vector(*pivot);   //use a copy
  pivot_heap = true;
  dynamic = false;
}

void GBase :: bind_rotation(Quaternion *r) {
  if (rotation_heap)
    delete rotation;
  rotation = r;
  rotation_heap = false;
  dynamic = true;
  updated = true;
}

void GBase :: unbind_rotation(void) {
  if (not rotation_heap)
    rotation = new Quaternion(*rotation);
  rotation_heap = true;
  dynamic = false;
}

void GBase :: bind_translation(Vector *t) {
  if (translation_heap)
    delete translation;
  translation = t;
  translation_heap = false;
  dynamic = true;
  updated = true;
}

void GBase :: unbind_translation(void) {
  if (not translation_heap)
    translation = new Vector(*translation);
  translation_heap = true;
  dynamic = false;
}

void GBase :: import(const string& path, const string& filename)
  throw(exception)
{
  try {
    GBase *foo = import_obj(path, filename);
    this->name=foo->name;
    this->ch_size = foo->ch_size;
    this->obb = foo->obb;
    this->aabb = foo->aabb;
    *(this->pivot) = *(foo->pivot);
    *(this->rotation) = *(foo->rotation);
    *(this->translation) = *(foo->translation);
    //this->active = foo->active;    //preserve flags
    //this->updated = foo->updated;
    //this->dynamic = foo->dynamic;
    this->children = foo->children;
    this->toc = foo->toc;
    this->toc_names = foo->toc_names;
  }
  catch (exception) {
    throw exception();
  }
}

Color GBase :: ambient(void) const { return m_a;}

const Color& GBase :: ambient(const Color& a) {
  m_a=a;
  return m_a;
}

Color GBase :: diffuse(void) const { return m_d;}

const Color& GBase :: diffuse(const Color& d) {
  m_d=d;
  return m_d;
}

Color GBase :: specular(void) const { return m_s;}

const Color& GBase :: specular(const Color& s) {
  m_s=s;
  return m_s;
}

Color GBase :: emission(void) const { return m_e;}

const Color& GBase :: emission(const Color& e) {
  m_e=e;
  return m_e;
}

int& GBase :: shininess(void) { return S_e;}

int GBase :: shininess(int se) { return S_e=se;}



//Inherited stuff
GLenum GBase :: shading(GLenum mode) {
  return 0;
}

GLenum GBase :: shading(void) const {
  return 0;
}

void GBase :: set_damage(unsigned pts) {}
unsigned GBase :: get_damage(void) const {return 0;}
void GBase :: hit(unsigned dp, bool kill) {}
