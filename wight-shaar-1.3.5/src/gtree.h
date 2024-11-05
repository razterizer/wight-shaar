// -*-c++-*-
#ifndef GTREE_H
#define GTREE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <ext/hash_map>
#include <stdexcept>
#include <GL/glut.h>

#include "gbase.h"
#include "gobject.h"
#include "gfunc.h"
#include "vessel.h"
#include "ship.h"
#include "torpedo.h"
#include "lists.h"
#include "tokenizer.h"


using namespace std;

typedef hash_map<string, GObject *, hash<string>, eqstr> ObjList;


#endif
