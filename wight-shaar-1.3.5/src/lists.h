// -*-c++-*-
#ifndef LISTS_H
#define LISTS_H

#include <ext/hash_map>

#include "geometry.h"
#include "hashcmp.h"

using namespace __gnu_cxx;

typedef hash_map<string, unsigned, hash<string>, eqstr> NumList;
typedef hash_map<string, string, hash<string>, eqstr> StrList;
//typedef hash_map<string, vector<Texel>, hash<string>, eqstr> TexelArrList;
//typedef hash_map<string, vector<Vertex>, hash<string>, eqstr> VertexArrList;
//typedef hash_map<string, vector<Vector>, hash<string>, eqstr> VectorArrList;
typedef hash_map<string, vector<unsigned *>, hash<string>, eqstr> FaceArrList;
typedef hash_map<string, Color, hash<string>, eqstr> ColorList;
typedef hash_map<string, int, hash<string>, eqstr> IntList;
typedef hash_map<string, bool, hash<string>, eqstr> BoolList;

#endif
