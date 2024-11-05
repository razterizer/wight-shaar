// -*-c++-*-
#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <ext/hash_map>
#include <stdexcept>
#include <GL/glut.h>

#include "geometry.h"
#include "hashcmp.h"

using namespace __gnu_cxx;


typedef struct {
  GLubyte *image;
  unsigned width, height;
} TexObj;

//Hashing with texture filenames as keys
typedef hash_map<string, TexObj, hash<string>, eqstr> TexList;


class Texture {
  TexList tex_list;
  vector<string> fname_list;
  vector<GLclampf> priorities;
  vector<GLuint> id_list;
  void compile(void);
public:
  ~Texture();
  void add(const string& path, const string& filename)
    throw(exception);
  void set_priority(const string& filename, float priority)
    throw(invalid_argument);
  bool is_resident(const string& filename);
  GLuint get(const string& filename);   //use filename as identifier
  GLubyte *image(const string& filename); //--"--
  void render(const string& filename);
};




#endif
