#include "gtree.h"

#define MAX_ROW_LENGTH 1024
#define MAX_NAME_LENGTH 64


bool find_str(vector<string> haystack, string needle);




GBase *copy_tree(GBase *c) throw(bad_typeid) {
  if (c==NULL) return c;
  GBase *out;
  unsigned i;
  switch (c->type()) {
  case GBASE:
    out=new GBase(*c);
    break;
  case GOBJECT:
    out=new GObject(*dynamic_cast<GObject *>(c));
    break;
  case GFUNC:
    out=new GFunc(*dynamic_cast<GFunc *>(c));
    break;
  case VESSEL:
    out=new Vessel(*dynamic_cast<Vessel *>(c));
  case SHIP:
    out=new Ship(*dynamic_cast<Ship *>(c));
    break;
  case TORPEDO:
    out=new Torpedo(*dynamic_cast<Torpedo *>(c));
    break;
  default:
    cerr << "Illegal node type." << endl;
    throw bad_typeid();
  }

  if (!c->size() || c->type()==GOBJECT)
    return out;
  else {
    for (i=0;i<c->size();i++)
      out->add(copy_tree(&(*c)[i]));
  }
  return out;
}





/***********************************************************
 *                     Importing function                  *
 ***********************************************************/

GBase* import_obj(const string& path_, const string& filename)
  throw(exception)
{
  string path(path_);
  if (path!="" and path[path.length()-1]!='/') path+="/";
  ifstream fobj((path+filename).c_str());
  string mtlfile;
  string obj_name;

  if (fobj.fail()){
    cerr << "Could not find object file \"" << path+filename << "\"." <<  endl;
    throw exception();
  }

  //Lookup tables
  vector<string> name_list;  //object names
  StrList mtl_id;            //maps object names to material names

  StrList texture_list;      //maps material names to texture file names
  ColorList Kd, Ks, Ka;      //maps material names to material colors
  IntList Ns;                //maps material names to material shininesses
  const float alpha=1;

  vector<Texel> texels;      //pool of texels
  vector<Vertex> vertices;   //pool of vertices
  vector<Vector> normals;    //pool of vertex normals
  FaceArrList faces;         //maps object names to arrays of face indices
  BoolList use_texels;   //enabled if texel data is present for part
  BoolList use_normals;  //enabled if vertex normal data is present for part

  //Parser variables
  string word;
  bool comment=false;

  //SCAN OBJECT FILE
  while (not fobj.eof()){
    fobj >> word;
    if (word[0]=='#') comment=true;
    if (not comment){

      //MATERIALS FILE
      if (word=="mtllib")
	fobj >> mtlfile;

      //OBJECT NAME
      else if (word=="g"){
	fobj >> obj_name;
	name_list.push_back(string(obj_name));
	use_normals[obj_name]=false;
	use_texels[obj_name]=false;
      }

      //USE MATERIAL
      else if (word=="usemtl"){
	fobj >> word;
	mtl_id[obj_name]=word;
      }

      //VERTICES
      else if (word=="v"){
	float vec[3];
	fobj >> vec[X];
	fobj >> vec[Y];
	fobj >> vec[Z];
	vertices.push_back(Vertex(vec[X], vec[Y], vec[Z]));
      }

      //NORMALS
      else if (word=="vn"){
	float vec[3];
	fobj >> vec[X];
	fobj >> vec[Y];
	fobj >> vec[Z];
	normals.push_back(Vector(vec[X], vec[Y], vec[Z]));
	use_normals[obj_name]=true;
      }

      //TEXELS
      else if (word=="vt"){
	float vec[2];
	fobj >> vec[X];
	fobj >> vec[Y];
	texels.push_back(Texel(vec[X], vec[Y]));
	use_texels[obj_name]=true;
      }

      //FACES
      else if (word=="f"){
	string val;
	unsigned *vec=new unsigned[3];

	for (int i=0;i<3;i++){     //for each vertex in the triangle facet
	  fobj >> val;
	  if (val=="") break;
	  sscanf(val.c_str(),"%u",&vec[i]);
	  vec[i]--;
	}
	if (val=="") break;
	faces[obj_name].push_back(vec);
      }
    }
    if (fobj.get()=='\n') comment=false;
    fobj.unget();
  }

  fobj.close();


  //SCAN MATERIALS FILE
  ifstream fmtl((path+mtlfile).c_str());
  string id, val;
  float vec[4];
  vec[3]=alpha;
  comment=false;
  string texfile;
  string mtl_name;
  
  if (fmtl.fail()){
    cerr << "Could not find material file \"" << path+mtlfile << "\"." <<  endl;
    throw exception();
  }

  while (not fmtl.eof()){
    fmtl >> id;
    if (id[0]=='#') comment=true;
    if (not comment){
      if (id=="newmtl")
	fmtl >> mtl_name;
      else if (id=="Kd" || id=="Ks" || id=="Ka"){
	fmtl >> vec[R];
	fmtl >> vec[G];
	fmtl >> vec[B];
	if (id=="Kd")
	  Kd[mtl_name]=Color(vec);
	else if (id=="Ks")
	  Ks[mtl_name]=Color(vec);
	else
	  Ka[mtl_name]=Color(vec);
      }
      else if (id=="Ns"){
	fmtl >> val;
	Ns[mtl_name]=atoi(val.c_str());
      }
      else if (id=="map_Kd"){
	fmtl >> texfile;
	texture_list[mtl_name]=texfile;
	
	//ADD TEXTURE FILE
	GBase::tex_lib().add(path, texfile);
      }
    }
    if (fmtl.get()=='\n') comment=false;
    fmtl.unget();
  }
  fmtl.close();


  //CREATE TREE
  string treefile=filename.substr(0,filename.length()-3)+"tr";
  ifstream ftree((path+treefile).c_str());
  ObjList leaves;


  if (ftree.fail()){
    cerr << "Could not find tree file \"" << path+treefile << "\"." <<  endl;
    throw exception();
  }

  //  <leaf_name>
  //  ...
  //  {<parent_name>}+ : {<child_name>}+
  //  ...

  //  body
  //  motor_rev
  //  jet_rev
  //  motor_L
  //  jet_L
  //  motor_R
  //  jet_R
  //  player : body rev frw
  //  player rev : motor_rev jet_rev
  //  player frw : left right
  //  player frw left : motor_L gimbal_L
  //  player frw left gimbal_L : nozzle_L jet_L
  //  player frw right : motor_R gimbal_R
  //  player frw right gimbal_R : nozzle_R jet_R

  char rad[MAX_ROW_LENGTH];
  unsigned long pos=0;
  string name;


  //1) GET THE LEAVES
  while (ftree.getline(rad, MAX_ROW_LENGTH)){
    char item[MAX_NAME_LENGTH], apa;
    if (sscanf(rad, "%s %c", item,&apa)!=1) break;
    name=item;
    pos = ftree.tellg();
    { //add gobject (leaf) to list
      leaves[name] = new GObject(name);
      if (use_texels[name])
	leaves[name]->set_texture(texture_list[mtl_id[name]]);
      //insert polygons
      unsigned v_id=0;
      for (unsigned i=0;i<faces[name].size();i++) {  //for each face
	Poly poly;
	for (unsigned j=0;j<3;j++) {  //for each vertex
	  v_id=faces[name][i][j];    //vertex number
	  if (use_normals[name] and use_texels[name])
	    poly.add(vertices[v_id],
		     normals[v_id],
		     texels[v_id]);
	  else if (use_normals[name])
	    poly.add(vertices[v_id],
		     normals[v_id]);
	  else if (use_texels[name])
	    poly.add(vertices[v_id],
		     texels[v_id]);
	  else
	    poly.add(vertices[v_id]);
	}
	leaves[name]->add(poly, false);  //turn off display list recompilation
      }
      leaves[name]->set_name(name);
      leaves[name]->ambient(Ka[mtl_id[name]]);
      leaves[name]->diffuse(Kd[mtl_id[name]]);
      leaves[name]->specular(Ks[mtl_id[name]]);
      leaves[name]->shininess(Ns[mtl_id[name]]);

      //CREATE DISPLAY LISTS
      leaves[name]->recompile();  //now is a good time to create the displist
    }
  }

  //2) READ PARENT INFO AND BUILD TREE
  ftree.seekg(pos);   //resume reading
  ftree >> name;      //get root name

  ftree.seekg(pos);   //rewind to beginning of line again
  GBase
    *current = NULL,
    *root = new GBase(name);
  while (ftree.getline(rad, MAX_ROW_LENGTH)){      //for each line
    Tokenizer tok(rad);
    while (not tok.end()){
      name = tok.next();
      if (name==":") break;
      if (name==root->get_name())
	current = root;
      else
	current=&(*current)[name];          //"traverse"
    }
    while (not tok.end()) {
      name = tok.next();
      if (find_str(name_list, name))
	current->add(leaves[name]);
      else
	current->add(new GBase(name));
    }
  }
  ftree.close();



  return root;
}




bool find_str(vector<string> haystack, string needle){
  for (unsigned i=0;i<haystack.size();i++)
    if (haystack[i]==needle) return true;
  return false;
}
