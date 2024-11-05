// -*-c++-*-
#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <iostream>
#include <GL/glut.h>

#include "geometry.h"
#include "texture.h"
#include "timer.h"

typedef enum {TEXT_BITMAP, TEXT_TEXTURE, TEXT_AUTO} text_t;


class Text {
  GLint vp[4];
protected:
  string txt;
  void *font_name;
  Vertex pos;
  Color col;
  static Texture fonts;
  static GLuint font_base;
  static bool textured_enabled;
  static text_t best_render_mode;
  static const GLuint font_set_size;
  unsigned text_width, text_height, font_height;
  void set_width(void);
  void set_width(const string& s);
  void set_width(char *const s);
  void set_width(char c);
  void set_height(void);
public:
  Text()
    : txt(""), font_name(NULL) {}
  Text(char *const str)
    : txt(str), font_name(NULL) {}
  Text(const string& str)
    : txt(str), font_name(NULL) {}
  Text(const Text& str)
    : txt(str.txt), font_name(NULL) {}

  friend istream& operator>> (istream& i, Text& str);
  friend ostream& operator<< (ostream& o, const Text& str);
  const Text& operator= (const Text& str);
  bool operator== (const Text& str) const;
  bool operator!= (const Text& str) const;
  const Text& operator+= (const Text& str);
  const Text& operator+= (const string& str);
  const Text& operator+= (char *const str);
  const Text& operator+= (char c);
  const Text& operator+= (int i);
  const Text& operator+= (float f);
  Text operator+ (const Text& str) const;
  Text operator+ (const string& str) const;
  Text operator+ (char *const str) const;
  Text operator+ (char c) const;
  Text operator+ (int i) const;
  Text operator+ (float f) const;
  char operator[] (unsigned i) const;
  char& operator[] (unsigned i);

  static void init(const string& path="fonts");
  static void destroy(void);
  static text_t evaluate(void);
  Text substr(unsigned start, unsigned length) const;
  void *font(void *font);
  void *font(void) const;
  Vertex position(const Vertex& pos);
  Vertex position(void) const;
  Vertex add_position(const Vertex& pos);
  Vertex center_x(void);
  Vertex center_y(void);
  Vertex center(void);
  Vertex left(void);
  Vertex right(void);
  Vertex top(void);
  Vertex bottom(void);
  unsigned width(void) const;
  unsigned height(void) const;
  unsigned font_size(void) const;
  Color color(const Color& col);
  Color color(void) const;
  void render(text_t mode) const throw(invalid_argument, logic_error);
};

#endif
