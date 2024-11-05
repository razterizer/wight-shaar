#include "gobject.h"



/*********************************************
 *                  GObject                  *
 *********************************************/

void GObject :: recompile(void) {
  //glNewList(displist_id, GL_COMPILE_AND_EXECUTE);  //plain stupid!
  glNewList(displist_id, GL_COMPILE);
  texture_lib.render(texture);

  for (unsigned j=0;j<size();j++)  //for each polygon
    (*object)[j].render();

  glEndList();
}

void GObject :: make_obb(void) {
  string id;
  for (unsigned i=0;i<size();i++)
    for (unsigned j=0;j<(*object)[i].size();j++){
      obb.xmin = MIN2(obb.xmin, (*object)[i][j][X]);
      obb.xmax = MAX2(obb.xmax, (*object)[i][j][X]);
      obb.ymin = MIN2(obb.ymin, (*object)[i][j][Y]);
      obb.ymax = MAX2(obb.ymax, (*object)[i][j][Y]);
      obb.zmin = MIN2(obb.zmin, (*object)[i][j][Z]);
      obb.zmax = MAX2(obb.zmax, (*object)[i][j][Z]);
    }
}

void GObject :: update_obb(const Poly& p_) {
  Poly p(p_);
  for (unsigned i=0;i<p.size();i++){
    obb.xmin = MIN2(obb.xmin, p[i][X]);
    obb.xmax = MAX2(obb.xmax, p[i][X]);
    obb.ymin = MIN2(obb.ymin, p[i][Y]);
    obb.ymax = MAX2(obb.ymax, p[i][Y]);
    obb.zmin = MIN2(obb.zmin, p[i][Z]);
    obb.zmax = MAX2(obb.zmax, p[i][Z]);
  }
}

GObject :: GObject(const string& n)
  : GBase(n), damage(0), free_object(false)
{
  object = new vector<Poly>;
}

//COPY CONSTRUCTOR
GObject :: GObject(const GObject& go)
  : GBase(go), damage(go.damage), free_object(false)
{
  object = go.object;            //share polygon memory.
}

//DESTRUCTOR
GObject :: ~GObject() {
  if (free_object)
    delete object;
}


GTYPE GObject :: type(void) const {
  return GOBJECT;
}

void GObject :: add(const Poly& p, bool do_recompile) {
  object->push_back(Poly(p));
  if (do_recompile)
    recompile();
  update_obb(p);
}

void GObject :: remove(int p) {
  object->erase(object->begin()+p);
  recompile();
  make_obb();      //reshape the intrinsic oriented bounding box
}

unsigned GObject :: size(void) const { return object->size();}

void GObject :: set_texture(const string& tfile) {
  GBase::set_texture(tfile);
  recompile();
}

void GObject :: render(void) {
  //Timer t0;
  if (use_detail_cull and detail_cull()) return;  //don't render if too small
  if (active) {
    //5 us
    m_a.render(GL_AMBIENT);
    m_d.render(GL_DIFFUSE);
    m_s.render(GL_SPECULAR);
    m_e.render(GL_EMISSION);
    //white.render(GL_COLOR);
    glMaterialf(GL_FRONT, GL_SHININESS, S_e);
    //2 us
    if (dynamic) {
      translation->begin();                  //move object
         pivot->begin();                         //translate back
	    rotation->begin();                   //rotate
	       Vector v=(-*pivot);
	       v.begin();                       //center around pivot
    }
    //1.7 ms | 40.5 ms ("body")
    //_time::tic(t0);
    glCallList(displist_id);
    //if (name=="body") cout << _time::toc(t0) << endl;
    //7 us
    if (dynamic) {
	       pivot->end();
	    rotation->end();
	 pivot->end();
      translation->end();
    }
  }
}

void GObject :: vfc_render(void) {
  if (vf_cull()==OUTSIDE) return;  //render if intersecting or inside
  if (use_detail_cull and detail_cull()) return;  //don't render if too small
  if (active) {
    m_a.render(GL_AMBIENT);
    m_d.render(GL_DIFFUSE);
    m_s.render(GL_SPECULAR);
    m_e.render(GL_EMISSION);
    //white.render(GL_COLOR);
    glMaterialf(GL_FRONT, GL_SHININESS, S_e);
    if (dynamic) {
      translation->begin();                  //move object
         pivot->begin();                         //translate back
	    rotation->begin();                   //rotate
	       Vector v=(-*pivot);
	       v.begin();                       //center around pivot
    }
    glCallList(displist_id);
    if (dynamic) {
	       pivot->end();
	    rotation->end();
	 pivot->end();
      translation->end();
    }
  }
}

GLenum GObject :: shading(GLenum mode) {
  for (unsigned i=0;i<size();i++)  //each polygon
      (*object)[i].shading(mode);
  recompile();
  return mode;
}

GLenum GObject :: shading(void) const {
  if (not size())
    return 0;
  return (*object)[0].shading();
}

void GObject :: invert(void) {
  for (unsigned j=0;j<size();j++)
    (*object)[j].invert();
  recompile();
}

Poly& GObject :: operator[] (unsigned i) {
  return (*object)[i];
}

void GObject :: free_upon_delete(bool flag) {
  free_object = flag;
}

void GObject :: set_damage(unsigned pts) {
  damage = pts;
}

unsigned GObject :: get_damage(void) const {
  return damage;
}

void GObject :: hit(unsigned dp, bool kill) {
  if (damage >= dp)
    damage -= dp;
  else
    damage = 0;
  if (!damage){
    active = false;
    if (kill)
      ancestor->set_active(false);
  }
}

