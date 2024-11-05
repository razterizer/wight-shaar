#include "texture.h"


float log_2(float x){
  return log(x)/M_LN2;
}




Texture :: ~Texture() {
  string id;
  for (unsigned i=0;i<id_list.size();i++)
    glDeleteTextures(1, &id_list[i]);
  for (unsigned t=0;t<fname_list.size();t++){
    id = fname_list[t];
    delete[] tex_list[id].image;
  }
}





void Texture :: compile(void) {
  unsigned i=id_list.size();
  unsigned id=i+1;
  unsigned width, height;
  string fname=fname_list[i];

  width=tex_list[fname].width;
  height=tex_list[fname].height;

  id_list.push_back(id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &id_list[i]);
  glBindTexture(GL_TEXTURE_2D, id_list[i]);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //default anyway

  if (log_2(width)!=(int)log_2(width) ||
      log_2(height)!=(int)log_2(height)){    //if dimensions not 2^n
    //printf("mipmap\n");
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
		      GL_UNSIGNED_BYTE, tex_list[fname].image);
  } else {
    //printf("normal\n");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		 0, GL_RGB, GL_UNSIGNED_BYTE, tex_list[fname].image);
  }
}







void Texture :: add(const string& path_, const string& filename)
  throw(exception)
{
  bool found=false;
  for (unsigned i=0;i<fname_list.size();i++)
    if (fname_list[i]==filename){
      found=true;
      break;
    }
  //Do nothing if texture is already loaded.
  if (found) return;


  string path(path_);
  if (path!="" and path[path.length()-1]!='/') path+="/";
  ifstream ftex((path+filename).c_str());

  if (ftex.fail()){
    cerr << "Could not find texture \"" << path+filename << "\"." <<  endl;
    throw exception();
  }

  string fubar;
  unsigned width, height;

  ftex >> fubar;             //P3
  if (fubar != "P3"){
    cerr << "Texture \"" << path+filename << "\" is not a ppm image.";
    throw invalid_argument("Texture:add(const string&, const string&)");
  }
  while (true) {             //# comment
    ftex >> fubar;
    if (ftex.get()=='\n'){
      ftex.unget();
      break;
    }
  }
  ftex >> width;
  ftex >> height;
  ftex >> fubar;             //255

  tex_list[filename].width = width;
  tex_list[filename].height = height;
  //image[height][width][3]
  tex_list[filename].image = new GLubyte[height*width*3];

  unsigned ival;
  unsigned l=0;
  for (unsigned i=0;i<height;i++)
    for (unsigned j=0;j<width;j++)
      for (unsigned k=0;k<3;k++){
	ftex >> ival;
	tex_list[filename].image[l++]=ival;
      }

  ftex.close();

  fname_list.push_back(filename);
  priorities.push_back(0);

  compile();
}



void Texture :: set_priority(const string& filename, float priority)
  throw(invalid_argument)
{
  bool found=false;
  unsigned i;
  for (i=0;i<fname_list.size();i++)
    if (fname_list[i]==filename){
      found=true;
      break;
    }
  if (not found) return;

  if (priority<0 or priority>1) {
    cerr << "Priority must be a value between 0 and 1." << endl;
    throw invalid_argument("Texture::set_priority(const string&, float)");
  }

  priorities[i]=priority;

  glPrioritizeTextures(1, &id_list[i], &priorities[i]);
}



bool Texture :: is_resident(const string& filename) {
  bool found=false;
  unsigned i;
  for (i=0;i<fname_list.size();i++)
    if (fname_list[i]==filename){
      found=true;
      break;
    }
  if (not found) return false;

  GLboolean fubar;

  return glAreTexturesResident(1, &id_list[i], &fubar);
}



GLuint Texture :: get(const string& filename) {
  bool found=false;
  unsigned i;
  for (i=0;i<fname_list.size();i++)
    if (fname_list[i]==filename){
      found=true;
      break;
    }
  if (not found){
    return 0;
    //return NULL;
    //cerr << "Texture \"" << filename << "\" is not loaded yet." << endl;
    //throw invalid_argument("Texture::get(const string&)");
  }

  return id_list[i];
}






GLubyte* Texture :: image(const string& filename) {
  bool found=false;
  for (unsigned i=0;i<fname_list.size();i++)
    if (fname_list[i]==filename){
      found=true;
      break;
    }
  if (not found){
    return NULL;
    //cerr << "Texture \"" << filename << "\" is not loaded yet." << endl;
    //throw invalid_argument("Texture::image(const string&)");
  }

  return tex_list[filename].image;
}


void Texture :: render(const string& filename) {
  GLuint dl_id=get(filename);
  if (!dl_id)
    glDisable(GL_TEXTURE_2D);
  else {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, dl_id);
  }
}
