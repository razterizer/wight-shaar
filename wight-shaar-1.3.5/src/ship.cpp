#include "ship.h"

Ship :: Ship(const string& n)
  : Vessel(n), w_torp(NULL)
{
  set_ancestor();
}

Ship :: Ship(const Matrix& I, State s0, unsigned Nt, float m,
	     const string& n)
  : Vessel(I, s0, Nt, m, n), w_torp(NULL)
{
  set_ancestor();
}

Ship :: Ship(const Matrix& I, State s0, unsigned Nt, float m,
	     const string& path, const string& wfobj,
	     const string& n)
  : Vessel(I, s0, Nt, m, path, wfobj, n), w_torp(NULL)
{
  set_ancestor();
}

Ship :: Ship(const Ship& sh)   //Does NOT perform deep copy. Use the add-method
  : Vessel(sh)
{
  if (sh.w_torp!=NULL)
    w_torp = new Torpedo(*sh.w_torp);
  set_ancestor();
}

Ship :: ~Ship() {
  //Must be done via deleting the tree.
  //w_torp is just a reference!
  /*
  if (w_torp!=NULL)
    delete w_torp;
  */
}



GTYPE Ship :: type(void) const {
  return SHIP;
}

void Ship :: set_torpedo(Torpedo *t) {
  w_torp = t;
}

void Ship :: activate_torpedo(void) {
  if (not w_torp->is_active()) {
    State t_state(s.pos(), s.vel(), Vector(), s.rot());
    w_torp->state(t_state);
    w_torp->activate();
  }
}

/*
void Ship :: add(GBase *torp) throw(invalid_argument) {
  if (type()!=SHIP and torp->type()==TORPEDO) {
    cerr << "If the child is a Torpedo object, "
	 << "then the parent must be a Ship object." << endl;
    throw invalid_argument("Ship::add(GBase *)");
  }
  if (type()==SHIP and torp->type()==TORPEDO)
    w_torp=dynamic_cast<Torpedo *>(torp);
  GBase::add(torp);
}

void Ship :: add(GBase *torp, const string& alt_cn) throw(invalid_argument) {
  if (type()!=SHIP and torp->type()==TORPEDO) {
    cerr << "If the child is a Torpedo object, "
	 << "then the parent must be a Ship object." << endl;
    throw invalid_argument("Ship::add(GBase *, const string&)");
  }
  if (type()==SHIP and torp->type()==TORPEDO)
    w_torp=dynamic_cast<Torpedo *>(torp);
  GBase::add(torp, alt_cn);
}
*/
