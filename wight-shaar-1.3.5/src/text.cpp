#include "text.h"


Texture Text :: fonts;
GLuint Text :: font_base;
bool Text :: textured_enabled = false;
text_t Text :: best_render_mode = TEXT_AUTO;   //i.e. not determined yet
const GLuint Text :: font_set_size = 1<<7;


void Text :: set_width(void) {
  text_width = 0;
  if (font_name!=NULL)
    text_width = glutBitmapLength(font_name,
				  (unsigned char*)txt.c_str());
}

void Text :: set_width(const string& s) {
  text_width = 0;
  if (font_name!=NULL)
    text_width += glutBitmapLength(font_name,
				   (unsigned char*)s.c_str());
}

void Text :: set_width(char *const s) {
  text_width = 0;
  if (font_name!=NULL)
    text_width += glutBitmapLength(font_name,
				   (unsigned char*)s);
}

void Text :: set_width(char c) {
  text_width = 0;
  if (font_name!=NULL)
    text_width += glutBitmapWidth(font_name, c);
}

void Text :: set_height(void) {
  font_height = 0;
  if (font_name==GLUT_BITMAP_8_BY_13)
    font_height = 13;
  else if (font_name==GLUT_BITMAP_9_BY_15)
    font_height = 15;
  else if (font_name==GLUT_BITMAP_TIMES_ROMAN_10 or
	   font_name==GLUT_BITMAP_HELVETICA_10)
    font_height = 10;
  else if (font_name==GLUT_BITMAP_TIMES_ROMAN_24)
    font_height = 24;
  else if (font_name==GLUT_BITMAP_HELVETICA_12)
    font_height = 12;
  else if (font_name==GLUT_BITMAP_HELVETICA_18)
    font_height = 18;

  unsigned lines = 1;
  if (font_name!=NULL) {
    for (unsigned i=0;i<txt.length();i++)
      if (txt[i]=='\n') lines++;
    
  }
  text_height = font_height*lines;
}


istream& operator>> (istream& i, Text& str) {
  i >> str.txt;
  str.set_width();
  return i;
}

ostream& operator<< (ostream& o, const Text& str) {
  o << str.txt;
  return o;
}

const Text& Text :: operator= (const Text& str) {
  txt = str.txt;
  set_width();
  //font_name = str.font_name;
  //pos = str.pos;
  //col = str.col;
  return *this;
}

bool Text :: operator== (const Text& str) const {
  return txt==str.txt;
}

bool Text :: operator!= (const Text& str) const {
  return txt!=str.txt;
}

const Text& Text :: operator+= (const Text& str) {
  txt += str.txt;
  set_width(str.txt);
  return *this;
}

const Text& Text :: operator+= (const string& str) {
  txt += str;
  set_width(str);
  return *this;
}

const Text& Text :: operator+= (char *const str) {
  txt += string(str);
  set_width(str);
  return *this;
}

const Text& Text :: operator+= (char c) {
  txt += c;
  set_width(c);
  return *this;
}

const Text& Text :: operator+= (int i) {
  char buffer[16]; sprintf(buffer, "%i", i);
  txt += buffer;
  set_width(buffer);
  return *this;
}

const Text& Text :: operator+= (float f) {
  char buffer[16]; sprintf(buffer, "%.1f", f);
  txt += buffer;
  set_width(buffer);
  return *this;
}

Text Text :: operator+ (const Text& str) const {
  Text tmp(*this);
  tmp.txt += str.txt;
  return tmp;
}

Text Text :: operator+ (const string& str) const {
  Text tmp(*this);
  tmp.txt += str;
  return tmp;
}

Text Text :: operator+ (char *const str) const {
  Text tmp(*this);
  tmp.txt += string(str);
  return tmp;
}

Text Text :: operator+ (char c) const {
  Text tmp(*this);
  tmp.txt += c;
  return tmp;
}

Text Text :: operator+ (int i) const {
  Text tmp(*this);
  tmp += i;
  return tmp;
}

Text Text :: operator+ (float f) const {
  Text tmp(*this);
  tmp += f;
  return tmp;
}

char Text :: operator[] (unsigned i) const {
  return txt[i];
}

char& Text :: operator[] (unsigned i) {
  return txt[i];
}

void Text :: init(const string& path) {
  if (textured_enabled) return; //prevent initializing textured fonts twice

  GLuint k;
  const GLuint xmax=16, ymax=8; //number of chars along dim
  GLfloat x, y;
  GLuint glyph_w, glyph_h, w, h;
  GLfloat dx, dy;
  GLfloat acc_w;       //accumulated width for non-fixed width fonts

  font_base = glGenLists(7*font_set_size);


  //8_BY_13
  fonts.add(path, "GLUT_BITMAP_8_BY_13.ppm");
  k=0;
  glyph_w=8;
  glyph_h=13;
  w=h=128;
  dx = ((GLfloat)glyph_w)/(GLfloat)w;
  dy = ((GLfloat)glyph_h)/(GLfloat)h;
  for (GLuint i=0;i<ymax;i++)  //rows
    for (GLuint j=0;j<xmax;j++){  //cols
      x = ((GLfloat)j)*dx;
      y = ((GLfloat)i)*dy-.5f/h;
      if (k!='\n'){
	glNewList(font_base+0*font_set_size+k++, GL_COMPILE);
	  glBegin(GL_QUADS);
            glTexCoord2f(x, y);
	    glVertex2i(0, -3);

	    glTexCoord2f(x+dx, y);
	    glVertex2i(glyph_w, -3);

	    glTexCoord2f(x+dx, y-dy);
	    glVertex2i(glyph_w, glyph_h-3);

	    glTexCoord2f(x, y-dy);
	    glVertex2i(0, glyph_h-3);
	  glEnd();
	  glTranslatef((GLfloat)glyph_w, 0.0f, 0.0f);
	glEndList();
      }
      else {
	glNewList(font_base+0*font_set_size+k++, GL_COMPILE);
	  glPopMatrix();
	  glTranslatef(0.0f, -(GLfloat)glyph_h, 0.0f);
	  glPushMatrix();
	glEndList();
      }
    }

  //9_BY_15
  fonts.add(path, "GLUT_BITMAP_9_BY_15.ppm");
  k=0;
  glyph_w=9;
  glyph_h=15;
  w=256; h=128;
  dx = ((GLfloat)glyph_w)/(GLfloat)w;
  dy = ((GLfloat)glyph_h)/(GLfloat)h;
  for (GLuint i=0;i<ymax;i++)
    for (GLuint j=0;j<xmax;j++){
      x = ((GLfloat)j)*dx;
      y = ((GLfloat)i)*dy-.5f/h;
      if (k!='\n'){
        glNewList(font_base+1*font_set_size+k++, GL_COMPILE);
	  glBegin(GL_QUADS);
            glTexCoord2f(x, y);
	    glVertex2i(0, -4);

	    glTexCoord2f(x+dx, y);
	    glVertex2i(glyph_w, -4);

	    glTexCoord2f(x+dx, y-dy);
	    glVertex2i(glyph_w, glyph_h-4);

	    glTexCoord2f(x, y-dy);
	    glVertex2i(0, glyph_h-4);
	  glEnd();
	  glTranslatef((GLfloat)glyph_w, 0.0f, 0.0f);
        glEndList();
      }
      else {
	glNewList(font_base+1*font_set_size+k++, GL_COMPILE);
	  glPopMatrix();
	  glTranslatef(0.0f, -(GLfloat)glyph_h, 0.0f);
	  glPushMatrix();
	glEndList();
      }
    }

  //TIMES_ROMAN_10
  fonts.add(path, "GLUT_BITMAP_TIMES_ROMAN_10.ppm");
  k=0;
  glyph_h=13;
  w=h=128;
  dy = (GLfloat)glyph_h/(GLfloat)h;
  for (GLuint i=0;i<ymax;i++){
    acc_w=0.0f;
    for (GLuint j=0;j<xmax;j++){
      glyph_w = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_10, k);
      dx = (GLfloat)glyph_w/(GLfloat)w;
      x = acc_w/w;
      y = ((GLfloat)i)*dy;
      acc_w += glyph_w;
      if (k!='\n'){
	glNewList(font_base+2*font_set_size+k++, GL_COMPILE);
          glBegin(GL_QUADS);
            glTexCoord2f(x, y);
	    glVertex2i(0, -2);

	    glTexCoord2f(x+dx, y);
	    glVertex2i(glyph_w, -2);

	    glTexCoord2f(x+dx, y-dy);
	    glVertex2i(glyph_w, glyph_h-2);

	    glTexCoord2f(x, y-dy);
	    glVertex2i(0, glyph_h-2);
	  glEnd();
	  glTranslatef((GLfloat)glyph_w, 0.0f, 0.0f);
	glEndList();
      }
      else {
	glNewList(font_base+2*font_set_size+k++, GL_COMPILE);
	  glPopMatrix();
	  glTranslatef(0.0f, -10.0, 0.0f);
	  glPushMatrix();
	glEndList();
      }
    }
  }

  //TIMES_ROMAN_24
  fonts.add(path, "GLUT_BITMAP_TIMES_ROMAN_24.ppm");
  k=0;
  glyph_h=26;
  w=512; h=256;
  dy = (GLfloat)glyph_h/(GLfloat)h;
  for (GLuint i=0;i<ymax;i++){
    acc_w=0.0f;
    for (GLuint j=0;j<xmax;j++){
      glyph_w = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, k);
      dx = (GLfloat)glyph_w/(GLfloat)w;
      x = acc_w/w+3.0f/w;
      y = ((GLfloat)i)*dy;
      acc_w += glyph_w;
      if (k!='\n'){
	glNewList(font_base+3*font_set_size+k++, GL_COMPILE);
          glBegin(GL_QUADS);
            glTexCoord2f(x, y);
	    glVertex2i(1, -6);

	    glTexCoord2f(x+dx, y);
	    glVertex2i(glyph_w+1, -6);

	    glTexCoord2f(x+dx, y-dy);
	    glVertex2i(glyph_w+1, glyph_h-6);

	    glTexCoord2f(x, y-dy);
	    glVertex2i(1, glyph_h-6);
	  glEnd();
	  glTranslatef((GLfloat)glyph_w, 0.0f, 0.0f);
	glEndList();
      }
      else {
	glNewList(font_base+3*font_set_size+k++, GL_COMPILE);
	  glPopMatrix();
	  glTranslatef(0.0f, -24.0f, 0.0f);
	  glPushMatrix();
	glEndList();
      }
    }
  }

  //HELVETICA_10
  fonts.add(path, "GLUT_BITMAP_HELVETICA_10.ppm");
  k=0;
  glyph_h=13;
  w=h=128;
  dy = (GLfloat)glyph_h/(GLfloat)h;
  for (GLuint i=0;i<ymax;i++){
    acc_w=0.0f;
    for (GLuint j=0;j<xmax;j++){
      glyph_w = glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, k);
      dx = (GLfloat)glyph_w/(GLfloat)w;
      x = acc_w/w;
      y = ((GLfloat)i)*dy;
      acc_w += glyph_w;
      if (k!='\n'){
	glNewList(font_base+4*font_set_size+k++, GL_COMPILE);
          glBegin(GL_QUADS);
            glTexCoord2f(x+.25f/w, y);
	    glVertex2i(1, -3);

	    glTexCoord2f(x+dx, y);
	    glVertex2i(glyph_w+1, -3);

	    glTexCoord2f(x+dx, y-dy);
	    glVertex2i(glyph_w+1, glyph_h-3);

	    glTexCoord2f(x+.25f/w, y-dy);
	    glVertex2i(1, glyph_h-3);
	  glEnd();
	  glTranslatef((GLfloat)glyph_w, 0.0f, 0.0f);
	glEndList();
      }
      else {
	glNewList(font_base+4*font_set_size+k++, GL_COMPILE);
	  glPopMatrix();
	  glTranslatef(0.0f, -10.0, 0.0f);
	  glPushMatrix();
	glEndList();
      }
    }
  }

  //HELVETICA_12
  fonts.add(path, "GLUT_BITMAP_HELVETICA_12.ppm");
  k=0;
  glyph_h=15;
  w=256; h=128;
  dy = (GLfloat)glyph_h/(GLfloat)h;
  for (GLuint i=0;i<ymax;i++){
    acc_w=0.0f;
    for (GLuint j=0;j<xmax;j++){
      glyph_w = glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, k);
      dx = (GLfloat)glyph_w/(GLfloat)w;
      x = acc_w/w;
      y = ((GLfloat)i)*dy;
      acc_w += glyph_w;
      if (k!='\n'){
	glNewList(font_base+5*font_set_size+k++, GL_COMPILE);
          glBegin(GL_QUADS);
            glTexCoord2f(x, y);
	    glVertex2i(1, -3);

	    glTexCoord2f(x+dx, y);
	    glVertex2i(glyph_w+1, -3);

	    glTexCoord2f(x+dx, y-dy);
	    glVertex2i(glyph_w+1, glyph_h-3);

	    glTexCoord2f(x, y-dy);
	    glVertex2i(1, glyph_h-3);
	  glEnd();
	  glTranslatef((GLfloat)glyph_w, 0.0f, 0.0f);
	glEndList();
      }
      else {
	glNewList(font_base+5*font_set_size+k++, GL_COMPILE);
	  glPopMatrix();
	  glTranslatef(0.0f, -12.0, 0.0f);
	  glPushMatrix();
	glEndList();
      }
    }
  }

  //HELVETICA_18
  fonts.add(path, "GLUT_BITMAP_HELVETICA_18.ppm");
  k=0;
  glyph_h=22;
  w=h=256;
  dy = (GLfloat)glyph_h/(GLfloat)h;
  for (GLuint i=0;i<ymax;i++){
    acc_w=0.0f;
    for (GLuint j=0;j<xmax;j++){
      glyph_w = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, k);
      dx = (GLfloat)glyph_w/(GLfloat)w;
      x = acc_w/w;
      y = ((GLfloat)i)*dy;
      acc_w += glyph_w;
      if (k!='\n'){
	glNewList(font_base+6*font_set_size+k++, GL_COMPILE);
          glBegin(GL_QUADS);
            glTexCoord2f(x+.25f/w, y);
	    glVertex2i(1, -4);

	    glTexCoord2f(x+dx, y);
	    glVertex2i(glyph_w+1, -4);

	    glTexCoord2f(x+dx, y-dy);
	    glVertex2i(glyph_w+1, glyph_h-4);

	    glTexCoord2f(x+.25f/w, y-dy);
	    glVertex2i(1, glyph_h-4);
	  glEnd();
	  glTranslatef((GLfloat)glyph_w, 0.0f, 0.0f);
	glEndList();
      }
      else {
	glNewList(font_base+6*font_set_size+k++, GL_COMPILE);
	  glPopMatrix();
	  glTranslatef(0.0f, -18.0, 0.0f);
	  glPushMatrix();
	glEndList();
      }
    }
  }

  textured_enabled = true;
}

void Text :: destroy(void) {
  if (not textured_enabled) return;
  glDeleteLists(font_base, 7*font_set_size);
  textured_enabled = false;
}

text_t Text :: evaluate(void) {
  //return if font textures aren't inited
  if (not textured_enabled) return best_render_mode;
  Text foo = "test";
  foo.font(GLUT_BITMAP_8_BY_13);
  Timer t_bm, t_tx;
  unsigned char i;
  _time::tic();
  for (i=0;i<255;i++)
    foo.render(TEXT_BITMAP);
  t_bm = _time::toc();
  _time::tic();
  for (i=0;i<255;i++)
    foo.render(TEXT_TEXTURE);
  t_tx = _time::toc();
  if (t_bm<t_tx)
    best_render_mode = TEXT_BITMAP;
  else
    best_render_mode = TEXT_TEXTURE;
  return best_render_mode;
}

Text Text :: substr(unsigned start, unsigned length) const {
  Text tmp(*this);   //make copy to preserve font settings
  tmp.txt = txt.substr(start, length);
  return tmp;
}

void* Text :: font(void *font) {
  font_name = font;
  set_height();
  set_width();
  return font_name;
}

void* Text :: font(void) const {
  return font_name;
}

Vertex Text :: position(const Vertex& pos) {
  return this->pos=pos;
}

Vertex Text :: position(void) const {
  return pos;
}

Vertex Text :: add_position(const Vertex& p) {
  return pos += p;
}

Vertex Text :: center_x(void) {
  if (font_name==NULL) return pos;
  glGetIntegerv(GL_VIEWPORT, vp);
  int width = vp[2]-vp[0];
  float x0 = (float)vp[0];
  float x = ((float)width)*.5f;
  pos[X] = x0+x-((float)text_width)*.5f;
  return pos;
}

Vertex Text :: center_y(void) {
  if (font_name==NULL) return pos;
  glGetIntegerv(GL_VIEWPORT, vp);
  int height = vp[3]-vp[1];
  float y0 = (float)vp[1];
  float y = ((float)height)*.5f;
  pos[X] = y0+y+((float)text_height)*.5f;
  return pos;
}

Vertex Text :: center(void) {
  if (font_name==NULL) return pos;
  glGetIntegerv(GL_VIEWPORT, vp);

  int width = vp[2]-vp[0];
  float x0 = (float)vp[0];
  float x = ((float)width)*.5f;
  pos[X] = x0+x-((float)text_width)*.5f;

  int height = vp[3]-vp[1];
  float y0 = (float)vp[1];
  float y = ((float)height)*.5f;
  pos[Y] = y0+y+((float)text_height)*.5f-((float)font_height);

  return pos;
}

Vertex Text :: left(void) {
  if (font_name==NULL) return pos;
  pos[X]=0.0f;
  return pos;
}

Vertex Text :: right(void) {
  if (font_name==NULL) return pos;
  glGetIntegerv(GL_VIEWPORT, vp);
  pos[X] = vp[2]-text_width;
  return pos;
}

Vertex Text :: top(void) {
  if (font_name==NULL) return pos;
  glGetIntegerv(GL_VIEWPORT, vp);
  pos[Y] = vp[3]-font_height;
  return pos;
}

Vertex Text :: bottom(void) {
  if (font_name==NULL) return pos;
  glGetIntegerv(GL_VIEWPORT, vp);
  pos[Y] = text_height-font_height;
  return pos;
}

unsigned Text :: width(void) const {
  return text_width;
}

unsigned Text :: height(void) const {
  return text_height;
}

unsigned Text :: font_size(void) const {
  return font_height;
}

Color Text :: color(const Color& col) {
  return this->col=col;
}

Color Text :: color(void) const {
  return col;
}

void Text :: render(text_t mode) const
  throw(invalid_argument, logic_error)
{
  if (font_name==NULL){
    cerr << "Unable to render text with NULL font.\n"
	 << "Please first select a font using Text::font(void *)." << endl;
    throw logic_error("Text::render()");
  }
  GLfloat newlines=0;

  switch(mode){
  case TEXT_BITMAP:    //~1ms/char
    col.render(GL_COLOR);
    glRasterPos3f(pos[X],pos[Y],pos[Z]);
    for (unsigned i=0; i<txt.length(); i++)
      if (txt[i]!='\n')
	glutBitmapCharacter(font_name, txt[i]);
      else
	glRasterPos3f(pos[X],
		      pos[Y]-((GLfloat)font_height)*++newlines,
		      pos[Z]);
    break;
  case TEXT_TEXTURE:   //~20 times faster
    if (textured_enabled) {
      glPushAttrib(GL_ENABLE_BIT);
      glEnable(GL_TEXTURE_2D);
      col.render(GL_COLOR);
      glPushMatrix();
      glTranslatef(pos[X], pos[Y], pos[Z]);
      glPushMatrix();
      if (font_name==GLUT_BITMAP_8_BY_13){
	fonts.render("GLUT_BITMAP_8_BY_13.ppm");
	glListBase(font_base);
      }
      else if (font_name==GLUT_BITMAP_9_BY_15){
	fonts.render("GLUT_BITMAP_9_BY_15.ppm");
	glListBase(font_base+font_set_size);
      }
      else if (font_name==GLUT_BITMAP_TIMES_ROMAN_10){
	fonts.render("GLUT_BITMAP_TIMES_ROMAN_10.ppm");
	glListBase(font_base+2*font_set_size);
      }
      else if (font_name==GLUT_BITMAP_TIMES_ROMAN_24){
	fonts.render("GLUT_BITMAP_TIMES_ROMAN_24.ppm");
	glListBase(font_base+3*font_set_size);
      }
      else if (font_name==GLUT_BITMAP_HELVETICA_10){
	fonts.render("GLUT_BITMAP_HELVETICA_10.ppm");
	glListBase(font_base+4*font_set_size);
      }
      else if (font_name==GLUT_BITMAP_HELVETICA_12){
	fonts.render("GLUT_BITMAP_HELVETICA_12.ppm");
	glListBase(font_base+5*font_set_size);
      }
      else if (font_name==GLUT_BITMAP_HELVETICA_18){
	fonts.render("GLUT_BITMAP_HELVETICA_18.ppm");
	glListBase(font_base+6*font_set_size);
      }
      glCallLists(txt.length(), GL_BYTE, txt.c_str());
      glPopMatrix();
      glPopMatrix();
      glPopAttrib();
      //glDisable(GL_TEXTURE_2D);
    }
    break;
  case TEXT_AUTO:
    if (best_render_mode==TEXT_AUTO) {
      cerr << "Best rendering mode has not yet been determined.\n"
	   << "Please invoke Text::evaluate(void) first." << endl;
      throw logic_error("Text::render()");
    }
    else
      render(best_render_mode);
    break;
  default:
    cerr << "Unknown text rendering mode." << endl;
    throw invalid_argument("Text::render()");
  }
}
