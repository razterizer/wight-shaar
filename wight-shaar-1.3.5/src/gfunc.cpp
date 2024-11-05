#include "gfunc.h"



/*******************************************
 *                  GFunc                  *
 *******************************************/

//PRIVATE
void GFunc :: recompile(void) {
  //glNewList(displist_id, GL_COMPILE_AND_EXECUTE); //plain stupid!
  glNewList(displist_id, GL_COMPILE);
  texture_lib.render(texture);

  (*func)();

  glEndList();
}


GFunc :: GFunc(void (*f)(void), const BB& box, const string& n)
  : GBase(n), func(f), damage(0)
{
  obb = box;
  recompile();
}

//COPY CONSTRUCTOR
GFunc :: GFunc(const GFunc& go)
  : GBase(go), func(go.func), damage(go.damage)
{}

//DESTRUCTOR
GFunc :: ~GFunc() {}


GTYPE GFunc :: type(void) const {
  return GFUNC;
}

void GFunc :: set_texture(const string& tfile) {
  GBase::set_texture(tfile);
  recompile();
}

void GFunc :: render(void) {
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

void GFunc :: vfc_render(void) {
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

void GFunc :: set_damage(unsigned pts) {
  damage = pts;
}

unsigned GFunc :: get_damage(void) const {
  return damage;
}

void GFunc :: hit(unsigned dp, bool kill) {
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
