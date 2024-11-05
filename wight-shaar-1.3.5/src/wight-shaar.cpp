#include "wight-shaar.h"



/*****************************************
 *            INITIALIZATIONS            *
 *                                       *
 *****************************************/
void init(void){

  // SCENE
  if (LOADINFO)
    cout << "Assembling the scene ..." << endl;
  scn::scene = new GBase("scene");

  // PLAYER
  if (LOADINFO)
    cout << " Creating player ..." << endl;
  init_player();

  // ENEMIES
  if (LOADINFO)
    cout << " Creating enemy drones ..." << endl;
  init_enemies();

  // SPACE BASE
  if (LOADINFO)
    cout << " Creating space base ..." << endl;
  init_spacebase();



  // CAMERA
  if (LOADINFO)
    cout << "Setting up 3rd person camera ..." << endl;
  cam::camera = new Ship(scn::player_ship->inertia(),
		    scn::player_ship->state(), 0,
		    scn::player_ship->mass(), "camera");
  cam::camera->state().pos() = Vector(cam::eye);  //alter the initial state
  //zeta=1, T0=3;
  float zeta=.6, T0=3;    //damping coefficient and oscillation period.
  float w0=2.0*M_PI/T0;
  float k=w0*w0*cam::camera->mass();
  float c=zeta*2.0*sqrt(k*cam::camera->mass());
  //cout << k << endl;
  //cout << c << endl;
  cam::camparams.push_back(k);
  cam::camparams.push_back(c);
  cam::camparams.push_back(cam::eye[X]);
  cam::camparams.push_back(cam::eye[Y]);
  cam::camparams.push_back(cam::eye[Z]);

  // PROJECTION MATRIX
  if (LOADINFO)
    cout << "Fetching projection matrix ..." << endl;
  omforma(WIDTH, HEIGHT);
  glGetDoublev(GL_PROJECTION_MATRIX, xf::gl_projection);
  xf::projection=Matrix(xf::gl_projection);


  // PHYSICS MOTORS
  if (PHYSICS) {
    if (LOADINFO)
      cout << "Initializing physics motors ..." << endl;
    scn::solver = new Euler(diff_rigid, scn::player_ship);
    cam::camsolver = new Euler(diff_spring, cam::camera);
    scn::trpsolver = new Euler(diff_rigid, scn::player_torpedo);
  }


  // COLLISION DETECTOR
  if (CDET) {
    if (LOADINFO)
      cout << "Initializing collision detector ..." << endl;
    colldet = new CD(collision, aim_lock);
  }


  // ENVIRONMENT
  if (SKYBOX) {
    if (LOADINFO)
      cout << "Creating environment ..." << endl;
    GLint id0 = glGenLists(6);
    for (unsigned i=0;i<6;i++)
      starbox_list[i]=id0+i;
    init_starbox();
  }


  // LIGHT
  if (LIGHTING) {
    if (LOADINFO)
      cout << "Setting up lighting ..." << endl;
    float lmodel_Ka[]={.6,.6,.1,1};
    float light_Ka[]={1,1,1,1};
    float light_Kd[]={1,1,.5,1};
    float light_Ks[]={1,1,1,1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_Ka);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Ks);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
  }
  else 
    GBase::lighting(false);


  // VARIOUS CONTROLS
  if (LOADINFO)
    cout << "Enabling stuff ..." << endl;
  if (BFC)
    glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);

  glClearColor(0,0,0,0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  GBase::detail_culling(DETAILC);
  GBase::set_detail_limit(.2);


  // SET TEXT PROPERTIES
  if (TEXT) {
    if (LOADINFO)
      cout << "Setting text properties ..." << endl;
    Text::init("data/fonts");
    txt::paused.font(GLUT_BITMAP_TIMES_ROMAN_24);
    txt::paused.color(col::green);
    txt::gameover.font(GLUT_BITMAP_TIMES_ROMAN_24);
    txt::gameover.color(col::red);
    txt::distance.font(txt::velocity.font(txt::orientation.font(
	 GLUT_BITMAP_8_BY_13)));
    txt::distance.color(txt::velocity.color(txt::orientation.color(
	 col::blue)));
    txt::distance.position(Vertex(4,46,0));
    txt::velocity.position(Vertex(4,32,0));
    txt::orientation.position(Vertex(4,18,0));
  }

  // CREATE HULL DAMAGE BAR
  if (LOADINFO)
    cout << "Initializing hull damage indicator ..." << endl;
  init_armour();


  // REFRESH BOUNDING VOLUME HIERARCHY
  if (LOADINFO)
    cout << "Refreshing bvh ..." << endl;
  scn::scene->refresh_bb();

  // TIME
  if (LOADINFO)
    cout << "Initializing time and random seed ..." << endl;
  _time::t0.update();
  _time::ct=_time::pt=_time::t0;
  srand(time(NULL));

  if (LOADINFO)
    cout << "Heads up!" << endl << endl;
}

/*****************************************
 *           GAME CONTROL INFO           *
 *                                       *
 *****************************************/
void info(void) {
  cout << endl
       << "   ***************************************" << endl
       << "   *                                     *" << endl
       << "   *              CONTROLS               *" << endl
       << "   *                                     *" << endl
       << "   * Forward:                PgUp        *" << endl
       << "   * Backward:               PgDn        *" << endl
       << "   * Pitch up:               Up-arrow    *" << endl
       << "   * Pitch down:             Down-arrow  *" << endl
       << "   * Yaw left:               Left-arrow  *" << endl
       << "   * Yaw right:              Right-arrow *" << endl
       << "   * Roll left:              Del         *" << endl
       << "   * Roll right:             End         *" << endl
       << "   *                                     *" << endl
       << "   * 10% throttle:           1           *" << endl
       << "   * 50% throttle:           2           *" << endl
       << "   * 100% throttle:          3           *" << endl
       << "   *                                     *" << endl
       << "   * Fire torpedo:           Space       *" << endl
       << "   *                                     *" << endl
       << "   * 1st person view:        F1          *" << endl
       << "   * Static world view:      F2          *" << endl
       << "   * 3rd person view:        F3          *" << endl
       << "   * Semistatic view back:   F4          *" << endl
       << "   * Semistatic view front:  F5          *" << endl
       << "   *                                     *" << endl
       << "   * Pause game:             P           *" << endl
       << "   * Exit game:              Q, X, Esc   *" << endl
       << "   *                                     *" << endl
       << "   ***************************************" << endl
       << endl;
}

/*****************************************
 *           COMMAND LINE HELP           *
 *                                       *
 *****************************************/
void help(void) {
  cout << "Command line options: " << endl;
  cout << "  -nosplash  : disables the splash \"screen\" text" << endl;
  cout << "  -noloadnfo : disables loading info" << endl;
  cout << "  -nosky     : disables the environment backdrop" << endl;
  cout << "  -bvh       : renders the bounding volume hierarchy" << endl;
  cout << "  -width #   : sets the width of the game window" << endl;
  cout << "  -height #  : sets the height of the game window" << endl;
  cout << "  -bench     : enables benchmarking" << endl;
  cout << "  -novfc     : disables view frustum culling" << endl;
  cout << "  -nodetailc : disables detail culling" << endl;
  cout << "  -nobfc     : disables backface culling" << endl;
  cout << "  -nocd      : disables collision detection" << endl;
  cout << "  -nophysics : disables physics motors" << endl;
  cout << "  -nolight   : disables lighting" << endl;
  cout << "  -notext    : disables rendering of text strings" << endl;
  cout << "  -nocursor  : disables rendering of the aiming lock" << endl
       << "               cursor and the distance information" << endl;
  cout << "  -help      : displays info about command line options and" << endl
       << "               gaming controls." << endl;
  cout << endl << "pleasant gaming!" << endl;
  cout << endl;
}

/*****************************************
 *            SPLASH "SCREEN"            *
 *                                       *
 *****************************************/
void splash(void) {
  cout << endl
       << "    ************************************************" << endl
       << "    *                                              *" << endl
       << "    *           W I G H T  -  S H A A R            *" << endl
       << "    *                                              *" << endl
       << "    *               The adventures of              *" << endl
       << "    *                  Wight-Shaar                 *" << endl
       << "    *            the great bounty hunter           *" << endl
       << "    *                                              *" << endl
       << "    *                   Ep I:                      *" << endl
       << "    *   \"The retribution of the dreadful drones\"   *" << endl
       << "    *                                              *" << endl
       << "    *                                              *" << endl
       << "    * (c) B. Rasmus Anthin, 2006                   *" << endl
       << "    *    - all rights reserved.                    *" << endl
       << "    ************************************************" << endl
       << endl;  
}



/*****************************************
 *       M     M    AA    III  N   N     *
 *       MM   MM   A  A    I   NN  N     *
 *       M M M M  A    A   I   NN  N     *
 *       M  M  M  AAAAAA   I   N N N     *
 *       M     M  A    A   I   N  NN     *
 *       M     M  A    A   I   N   N     *
 *       M     M  A    A  III  N   N     *
 *****************************************/

int main(int argc, char **argv){
  //PARSE COMMAND LINE OPTIONS
  string argument;
  int fubar=0;
  for (int i=1; i<argc; i++){
    argument=argv[i];
    if (argument=="-nosky")
      SKYBOX=false;
    else if (argument=="-bvh")
      RENDERBVH=true;
    else if (argument=="-width")
      if (i+1<argc and sscanf(argv[i+1],"%i",&fubar))
	WIDTH=atoi(argv[++i]);
      else {
	cerr << "width information is not supplied." << endl << endl;
	exit(EXIT_FAILURE);
      }
    else if (argument=="-height")
      if (i+1<argc and sscanf(argv[i+1],"%i",&fubar))
	HEIGHT=atoi(argv[++i]);
      else {
	cerr << "height information is not supplied." << endl << endl;
	exit(EXIT_FAILURE);
      }
    else if (argument=="-bench")
      BENCHMARK=true;
    else if (argument=="-novfc")
      VFC=false;
    else if (argument=="-nodetailc")
      DETAILC=false;
    else if (argument=="-nobfc")
      BFC=false;
    else if (argument=="-nocd")
      CDET=false;
    else if (argument=="-nosplash")
      SPLASH=false;
    else if (argument=="-noloadnfo")
      LOADINFO=false;
    else if (argument=="-nophysics")
      PHYSICS=false;
    else if (argument=="-nolight")
      LIGHTING=false;
    else if (argument=="-notext")
      TEXT=false;
    else if (argument=="-nocursor")
      CURSOR=false;
    else if (argument=="-help"){
      info();
      help();
      SPLASH=false;
      exit(EXIT_SUCCESS);
    }
    else {
      cerr << "Unknown option: \"" << argument << "\"." << endl;
      cerr << "Use \"-help\" for information on available options." << endl;
      cerr << endl;
      SPLASH=false;
      exit(EXIT_FAILURE);
    }
  }

  if (SPLASH)
    splash();
  cout << "Type \"wight-shaar -help\" for a list of controls\n"
       << "and command line options." << endl << endl;


  //GLUT SETUP
  glutInit(&argc,argv);
  if (not BENCHMARK)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("Wight-Shaar 1.3.5");
  //glutGameModeString("1280x1024:16@75");
  //glutEnterGameMode();
  init();
  glutReshapeFunc(omforma);
  glutDisplayFunc(visa);
  glutIdleFunc(tomgang);
  glutKeyboardFunc(tangent);
  glutKeyboardUpFunc(upptangent);
  glutSpecialFunc(specialtangent);
  glutSpecialUpFunc(specialupptangent);
  glutMainLoop();
  return 0;
}

void forward_on(void){
  if (not ctrl::forward){
    ctrl::forward=true;
    Color d(1, 0.9728, 0.32),
      a(1, .6, 0.23);

    scn::player_ship->thruster(0).enabled(true);
    scn::player_ship->thruster(1).enabled(true);
    (*scn::player_ship)["frw"]["left"]["gimbal"]["jet_L"].diffuse(d);
    (*scn::player_ship)["frw"]["left"]["gimbal"]["jet_L"].ambient(a);
    (*scn::player_ship)["frw"]["right"]["gimbal"]["jet_R"].diffuse(d);
    (*scn::player_ship)["frw"]["right"]["gimbal"]["jet_R"].ambient(a);
  }
}

void forward_off(void){
  if (ctrl::forward){
    ctrl::forward=false;
    (*scn::player_ship).thruster(0).enabled(false);
    (*scn::player_ship).thruster(1).enabled(false);
    (*scn::player_ship)["frw"]["left"]["gimbal"]["jet_L"].diffuse(col::black);
    (*scn::player_ship)["frw"]["left"]["gimbal"]["jet_L"].ambient(col::black);
    (*scn::player_ship)["frw"]["right"]["gimbal"]["jet_R"].diffuse(col::black);
    (*scn::player_ship)["frw"]["right"]["gimbal"]["jet_R"].ambient(col::black);
  }
}

void backward_on(void){
  if (not ctrl::backward){
    ctrl::backward=true;
    Color d(1, 0.9728, 0.32),
      a(1, .6, 0.23);
  
    scn::player_ship->thruster(2).enabled(true);
    (*scn::player_ship)["rev"]["jet_rev"].diffuse(d);
    (*scn::player_ship)["rev"]["jet_rev"].ambient(a);
  }
}

void backward_off(void){
  if (ctrl::backward){
    ctrl::backward=false;
    (*scn::player_ship).thruster(2).enabled(false);
    (*scn::player_ship)["rev"]["jet_rev"].diffuse(col::black);
    (*scn::player_ship)["rev"]["jet_rev"].ambient(col::black);
  }
}


void init_starbox(){
  int max=10;      //maximum skybox dimensions [m]
  float eps=1e-3;  //to get rid of the border

  GBase::tex_lib().add("data","front.ppm");
  //GBase::tex_lib().set_priority("front.ppm", 1);
  GBase::tex_lib().add("data","back.ppm");
  //GBase::tex_lib().set_priority("back.ppm", 1);
  GBase::tex_lib().add("data","left.ppm");
  //GBase::tex_lib().set_priority("left.ppm", 1);
  GBase::tex_lib().add("data","right.ppm");
  //GBase::tex_lib().set_priority("right.ppm", 1);
  GBase::tex_lib().add("data","top.ppm");
  //GBase::tex_lib().set_priority("top.ppm", 1);
  GBase::tex_lib().add("data","bottom.ppm");
  //GBase::tex_lib().set_priority("bottom.ppm", 1);

  glNewList(starbox_list[0], GL_COMPILE);
  GBase::tex_lib().render("front.ppm");
  glBegin(GL_QUADS);
  glTexCoord2f(0+eps, 1-eps);
  glVertex3f(-max, -max, -max);

  glTexCoord2f(1-eps, 1-eps);
  glVertex3f(max, -max, -max);

  glTexCoord2f(1-eps, 0+eps);
  glVertex3f(max, max, -max);

  glTexCoord2f(0+eps, 0+eps);
  glVertex3f(-max, max, -max);
  glEnd();
  glEndList();

  glNewList(starbox_list[1], GL_COMPILE);
  GBase::tex_lib().render("back.ppm");
  glBegin(GL_QUADS);
  glTexCoord2f(0+eps, 1-eps);
  glVertex3f(max, -max, max);

  glTexCoord2f(1-eps, 1-eps);
  glVertex3f(-max, -max, max);

  glTexCoord2f(1-eps, 0+eps);
  glVertex3f(-max, max, max);

  glTexCoord2f(0+eps, 0+eps);
  glVertex3f(max, max, max);
  glEnd();
  glEndList();

  glNewList(starbox_list[2], GL_COMPILE);
  GBase::tex_lib().render("left.ppm");
  glBegin(GL_QUADS);
  glTexCoord2f(0+eps, 1-eps);
  glVertex3f(-max, -max, max);

  glTexCoord2f(1-eps, 1-eps);
  glVertex3f(-max, -max, -max);

  glTexCoord2f(1-eps, 0+eps);
  glVertex3f(-max, max, -max);

  glTexCoord2f(0+eps, 0+eps);
  glVertex3f(-max, max, max);
  glEnd();
  glEndList();

  glNewList(starbox_list[3], GL_COMPILE);
  GBase::tex_lib().render("right.ppm");
  glBegin(GL_QUADS);
  glTexCoord2f(0+eps, 1-eps);
  glVertex3f(max, -max, -max);

  glTexCoord2f(1-eps, 1-eps);
  glVertex3f(max, -max, max);

  glTexCoord2f(1-eps, 0+eps);
  glVertex3f(max, max, max);

  glTexCoord2f(0+eps, 0+eps);
  glVertex3f(max, max, -max);
  glEnd();
  glEndList();

  glNewList(starbox_list[4], GL_COMPILE);
  GBase::tex_lib().render("top.ppm");
  glBegin(GL_QUADS);
  glTexCoord2f(0+eps, 1-eps);
  glVertex3f(-max, max, -max);

  glTexCoord2f(1-eps, 1-eps);
  glVertex3f(max, max, -max);

  glTexCoord2f(1-eps, 0+eps);
  glVertex3f(max, max, max);

  glTexCoord2f(0+eps, 0+eps);
  glVertex3f(-max, max, max);
  glEnd();
  glEndList();

  glNewList(starbox_list[5], GL_COMPILE);
  GBase::tex_lib().render("bottom.ppm");
  glBegin(GL_QUADS);
  glTexCoord2f(0+eps, 1-eps);
  glVertex3f(-max, -max, max);

  glTexCoord2f(1-eps, 1-eps);
  glVertex3f(max, -max, max);

  glTexCoord2f(1-eps, 0+eps);
  glVertex3f(max, -max, -max);

  glTexCoord2f(0+eps, 0+eps);
  glVertex3f(-max, -max, -max);
  glEnd();
  glEndList();
}

void init_player(void) {
  // SHIP
  float x=1.990, y=0.276, z=1.531, m=1500;
  Matrix I;
  I(X,X) = m*(y*y+z*z);
  I(Y,Y) = m*(z*z+x*x);
  I(Z,Z) = m*(x*x+y*y);

  Quaternion q0;
  q0.rotate(Vector(1,0,0),0);
  State s0(Vector(0,0,0),
	   Vector(0,0,0),
	   Vector(0,0,0),
	   q0);
  scn::player_ship = new Ship(I, s0, 3, m, "player");  //name is overridden anyway.
  scn::player_ship->import("data", "ship.obj");
  scn::player_ship->set_ancestor();

  (*scn::player_ship)["ship"].shininess(50);
  (*scn::player_ship)["frw"]["left"]["motor_L"].shading(GL_SMOOTH);
  (*scn::player_ship)["frw"]["right"]["motor_R"].shading(GL_SMOOTH);

  // DAMAGE RESISTANCE / HIT POINTS
  (*scn::player_ship)["ship"].set_damage(scn::hull);
  (*scn::player_ship)["rev"]["motor_rev"].set_damage(50);
  (*scn::player_ship)["frw"]["left"]["motor_L"].set_damage(50);
  (*scn::player_ship)["frw"]["left"]["gimbal"]["nozzle_L"].set_damage(20);
  (*scn::player_ship)["frw"]["right"]["motor_R"].set_damage(50);
  (*scn::player_ship)["frw"]["right"]["gimbal"]["nozzle_R"].set_damage(20);

  // THRUSTERS
  Color glow(.2, 0, 0);

  scn::player_ship->thruster(0).pos(Vector(-1, 0, 1.317));
  scn::player_ship->thruster(0).force(Vector(0,0,-FORCEFW*.1));
  //scn::player_ship->thruster(0).max_angles(30,70);
  (*scn::player_ship)["frw"]["left"]["gimbal"]["jet_L"].diffuse(col::black);
  (*scn::player_ship)["frw"]["left"]["gimbal"]["jet_L"].ambient(col::black);
  (*scn::player_ship)["frw"]["left"]["gimbal"]["jet_L"].emission(glow);
  scn::player_ship->bind_thruster(0, (*scn::player_ship)["frw"]["left"]["gimbal"], Vector(-1,0,1.065));

  scn::player_ship->thruster(1).pos(Vector(+1, 0, 1.317));
  scn::player_ship->thruster(1).force(Vector(0,0,-FORCEFW*.1));
  //scn::player_ship->thruster(1).max_angles(30,70);
  (*scn::player_ship)["frw"]["right"]["gimbal"]["jet_R"].diffuse(col::black);
  (*scn::player_ship)["frw"]["right"]["gimbal"]["jet_R"].ambient(col::black);
  (*scn::player_ship)["frw"]["right"]["gimbal"]["jet_R"].emission(glow);
  scn::player_ship->bind_thruster(1, (*scn::player_ship)["frw"]["right"]["gimbal"], Vector(+1,0,1.065));

  //scn::player_ship->thruster(2).pos(Vector(0,-0.0689,-1.495));
  scn::player_ship->thruster(2).pos(Vector(0,0,-1.495));
  scn::player_ship->thruster(2).force(Vector(0,0,FORCEBW*.1));
  //scn::player_ship->thruster(2).max_angles(30,70);
  //scn::player_ship->thruster(2).gimbal(-5,0);
  (*scn::player_ship)["rev"]["jet_rev"].diffuse(col::black);
  (*scn::player_ship)["rev"]["jet_rev"].ambient(col::black);
  (*scn::player_ship)["rev"]["jet_rev"].emission(glow);

  x=.03; y=.03; z=.25; m=1.5;
  float m_f=.5, I_sp=80, t_b=10;   //I_sp=80
  Matrix It;
  It(X,X) = m*(y*y+z*z);
  It(Y,Y) = m*(z*z+x*x);
  It(Z,Z) = m*(x*x+y*y);
  scn::player_torpedo = new Torpedo(It, State(), m, m_f, I_sp, t_b);
  scn::player_torpedo->import("data","torpedo.obj");
  scn::player_torpedo->set_ancestor();
  scn::player_ship->set_torpedo(scn::player_torpedo);

  GBase *player = new GBase("Wight-Shaar");
  player->add(scn::player_ship);
  player->add(scn::player_torpedo);
  scn::scene->add(player);
}


void init_enemies(void) {
  float x=1.49, y=0.895, z=1.63, m=250;
  Matrix I;
  I(X,X) = m*(y*y+z*z);
  I(Y,Y) = m*(z*z+x*x);
  I(Z,Z) = m*(x*x+y*y);

  Quaternion q0;
  q0.rotate(Vector(1,0,0),0);
  scn::enemies = new GBase("enemies");  //the batch of enemy rows
  GBase *row;                      //row of enemies
  Ship *enemy = new Ship (I, State(), 0, m, "enemy");
  enemy->import("data","drone.obj");
  enemy->set_ancestor();
  (*enemy)["body"].shading(GL_SMOOTH);

  // HITPOINTS
  (*enemy)["body"].set_damage(30);
  (*enemy)["turret_gimb"]["turret"].set_damage(10);
  (*enemy)["turret_gimb"]["turret_pipe"].set_damage(5);
  (*enemy)["legs"]["leg_1"].set_damage(5);
  (*enemy)["legs"]["leg_2"].set_damage(5);
  (*enemy)["legs"]["leg_3"].set_damage(5);
  (*enemy)["legs"]["leg_4"].set_damage(5);

  string e_name;
  char e_id[3];
  string row_name;
  char row_id[3];
  for (int i=0;i<NENEM_X;i++){
    sprintf(row_id, "%i", i);
    row_name="row";
    row_name+=row_id;
    row = new GBase(row_name);
    for (int j=0;j<NENEM_Y;j++){
      //State s0(Vector(i*7+50,0,-j*7-300),
      //State s0(Vector(i*7-10,0,-j*7-10),
      State s0(Vector(i*7-15,0,-j*7-30),
	       Vector(0,0,0),
	       Vector(0,0,0),
	       q0);
      sprintf(e_id, "%i", i*NENEM_Y+j);
      e_name="enemy";
      e_name+=e_id;
      row->add(enemy, e_name);
      dynamic_cast<Ship&>((*row)[e_name]).state(s0);
    }
    scn::enemies->add(row);
  }
  scn::scene->add(scn::enemies);
}

void init_spacebase(void) {
  BB sb_box = {-100, 100, -100, 100, -100, 100};
  scn::spacebase = new GFunc(render_spacebase, sb_box, "space base");
  GBase::tex_lib().add("data", "earth.ppm");
  //GBase::tex_lib().set_priority("earth.ppm", .8);
  scn::spacebase->set_texture("earth.ppm");
  scn::spacebase->translate(Vector(0,0,-1e3));
  scn::spacebase->ambient(col::black);
  scn::spacebase->diffuse(col::white);
  scn::spacebase->specular(col::white);
  scn::spacebase->emission(col::black);
  scn::spacebase->set_damage(1000);
  scn::scene->add(scn::spacebase);
}

void init_armour(void) {
  //rendered in ortho view
  armour_list = glGenLists(1);
  glNewList(armour_list, GL_COMPILE);
    glBegin(GL_QUADS);
      glVertex2i(0,0);
      glVertex2i(100,0);
      glVertex2i(100,10);
      glVertex2i(0,10);
    glEnd();
  glEndList();
}

void render_cursor(void) {
  glColor3f(1.0f, 1.0f, 0.2f);
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  if (LIGHTING){
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
  }
  glDisable(GL_TEXTURE_2D);
  glRasterPos3f(cursor::position[X],
		cursor::position[Y],
		cursor::position[Z]);
  glBitmap(16, 16, 8.0f, 8.0f, 0.0f, 0.0f, cursor::data);
  glPopAttrib();
}

void render_starbox(Vector& dir){
  GLfloat m_modelview[16];
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  if (LIGHTING){
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
  }
  glGetFloatv(GL_MODELVIEW_MATRIX, m_modelview);
  m_modelview[12]=m_modelview[13]=m_modelview[14]=0;
  glPushMatrix();
  glLoadMatrixf(m_modelview);
  //simple VFC
  if (dir[Z]<=0)
    glCallList(starbox_list[FRONT]);
  if (dir[Z]>=0)
    glCallList(starbox_list[BACK]);
  if (dir[X]<=0)
    glCallList(starbox_list[LEFT]);
  if (dir[X]>=0)
    glCallList(starbox_list[RIGHT]);
  if (dir[Y]>=0)
    glCallList(starbox_list[TOP]);
  if (dir[Y]<=0)
    glCallList(starbox_list[BOTTOM]);
  glPopMatrix();
  glPopAttrib();
  glDepthMask(GL_TRUE);
}


void render_spacebase(void) {
  glFrontFace(GL_CW);
  glutSolidTeapot(100);
  glFrontFace(GL_CCW); 
}

//~83 us
void render_armour(void) {
  //rendered in ortho view
  float ratio = (float)(*scn::player_ship)["ship"].get_damage()*scn::hullinv;
  armour_color.hsv(.33f*ratio,
		   1.0f,
		   1.0f);
  armour_color.render(GL_COLOR);
  glPushMatrix();
  glTranslatef(4.0f,2.0f,0.0f);
  glScalef(ratio,1.0f,1.0f);
  glCallList(armour_list);
  glPopMatrix();

}

void collision(GBase *n1, GBase *n2) {
  //cout << n1->get_name() << " >X< " << n2->get_name() << endl;
  GBase   //ancestors
    *a1=n1->get_ancestor(),
    *a2=n2->get_ancestor();
  
  //ship against droid
  if (a1!=NULL and a1->get_name()=="player" and
      a2!=NULL and a2->get_name().substr(0,5)=="enemy") {
    //player
    if (n1->get_name()=="ship") //hull
      n1->hit(5,true);
    else
      n1->hit(10);

    /*
    //THIS SHOULDN'T HAVE TO BE NECESSARY
    //rev motor
    if (n1->get_name()=="motor_rev" and not n1->is_active())
      (*a1)["rev"]["jet_rev"].set_active(false);
    //left motor
    if (n1->get_name()=="motor_L" and not n1->is_active())
      n1=&(*a1)["frw"]["left"]["gimbal"]["nozzle_L"];
    //left nozzle
    if (n1->get_name()=="nozzle_L" and not n1->is_active())
      (*a1)["frw"]["left"]["gimbal"]["jet_L"].set_active(false);
    if (n1->get_name()=="nozzle_L" and not n1->is_active())
      (*a1)["frw"]["left"]["gimbal"].set_active(false);
    //right motor
    if (n1->get_name()=="motor_R" and not n1->is_active())
      n1=&(*a1)["frw"]["right"]["gimbal"]["nozzle_R"];
    //right nozzle
    if (n1->get_name()=="nozzle_R" and not n1->is_active())
      (*a1)["frw"]["right"]["gimbal"]["jet_R"].set_active(false);
    if (n1->get_name()=="nozzle_R" and not n1->is_active())
      (*a1)["frw"]["right"]["gimbal"].set_active(false);
    */

    //droid
    if (n2->get_name()=="body") //hull
      n2->hit(5,true);
    else
      n2->hit(10);
  }

  //ship against space base
  if (a1!=NULL and a1->get_name()=="player" and
      n2->get_name()=="space base") {
    if (n1->get_name()=="ship") //hull
      n1->hit(50,true);
    else
      n1->hit(70);
    n2->hit(40);
  }

  //torpedo against droid
  if (a1!=NULL and a1->get_name()=="torpedo" and
      a2!=NULL and a2->get_name().substr(0,5)=="enemy") {
    a1->set_active(false);  //torpedo is destroyed
    if (n2->get_name()=="body")
      n2->hit(100,true);
    else
      n2->hit(100);
  }

  //torpedo against space base
  if (a1!=NULL and a1->get_name()=="torpedo" and
      n2->get_name()=="space base") {
    a1->set_active(false);  //torpedo is destroyed
    n2->hit(100);
  }
}

void aim_lock(GBase *n, const Ray& r){
  if (r.time() < cursor::location){
    cursor::position = n->get_bb_centre();
    cursor::location = r.time();
  }
  cursor::enabled = true;
  //cout << r << endl;
}

void extract_vf(Matrix& vf) {
  //verified and works correctly
  vf=xf::projection*xf::modelview;
}

void ortho(void) {
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  if (LIGHTING){
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
  }
  glDisable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,WIDTH,0,HEIGHT,-1,+1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

void perspective(void) {
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
}



/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                  G L U T       F U N C T I O N S                 **
 **                                                                  **
 **********************************************************************
 **********************************************************************/



/*****************************************
 *           DISPLAY FUNCTION            *
 *                                       *
 *****************************************/

//52 ms
void visa(void){
  //295 us
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  scn::pos = scn::player_ship->state().pos();
  scn::rot = scn::player_ship->state().rot();
  Vector up_ = scn::rot * cam::up;   //up vector in body frame to inertial frame
  scn::frw = scn::rot * scn::frw0;        //ship forward vector (i-frame)


  // CAMERA MODES (22.5 us)
  switch (cam::camera_mode) {
  case 1:       //1st PERSON
    cam::eye1=scn::pos+scn::rot*cam::eye0;
    cam::cen1=scn::pos+scn::rot*cam::cen0;
    gluLookAt(cam::eye1[X], cam::eye1[Y], cam::eye1[Z],
	      cam::cen1[X], cam::cen1[Y], cam::cen1[Z],
	      up_[X], up_[Y], up_[Z]);
    cam::dir=cam::cen1-cam::eye1;
    break;
  case 2:       //WORLD VIEW
    cam::eye1=cam::eye;
    gluLookAt(cam::eye[X],cam::eye[Y],cam::eye[Z],
	      cam::cen[X],cam::cen[Y],cam::cen[Z],
	      cam::up[X], cam::up[Y], cam::up[Z]);
    cam::dir=cam::cen-cam::eye;
    break;
  case 3:       //3rd PERSON
    cam::old_up[FIFO_SIZE-1]=up_;
    for (int i=1;i<FIFO_SIZE;i++)
      cam::old_up[i-1]=cam::old_up[i];
    up_=cam::old_up[0];
    
    cam::eye1 = cam::camera->state().pos();
    gluLookAt(cam::eye1[X], cam::eye1[Y], cam::eye1[Z],
	      scn::pos[X], scn::pos[Y], scn::pos[Z],
	      up_[X], up_[Y], up_[Z]);
    cam::dir=scn::pos-cam::eye1;
    break;
  case 4:       //SEMI-STATIC FORWARD
    cam::eye1=scn::pos+Vector(0,0,5);
    gluLookAt(cam::eye1[X], cam::eye1[Y], cam::eye1[Z],
	      scn::pos[X], scn::pos[Y], scn::pos[Z],
	      cam::up[X], cam::up[Y], cam::up[Z]);
    cam::dir=scn::pos-cam::eye1;
    break;
  case 5:       //SEMI-STATIC BACKWARD
    cam::eye1=scn::pos+Vector(0,0,-5);
    gluLookAt(cam::eye1[X], cam::eye1[Y], cam::eye1[Z],
	      scn::pos[X], scn::pos[Y], scn::pos[Z],
	      cam::up[X], cam::up[Y], cam::up[Z]);
    cam::dir=scn::pos-cam::eye1;
    break;
  }
  cam::dir.normalize();
  //---------------------------


  // REPOSITION LIGHT (4.5 us)
  if (LIGHTING)
    glLightfv(GL_LIGHT0, GL_POSITION, scn::light_pos);

  // RENDER SOME STUFF (44 ms)
  glColor3f(1.0f, 1.0f, 1.0f);   //2 us
  if (SKYBOX)
    render_starbox(cam::dir);
  if (VFC)                //bottleneck
    scn::scene->vfc_render();  //43 ms
  else
    scn::scene->render();      //43 ms
  if (CURSOR and cursor::enabled and scn::player_ship->is_active())
    render_cursor();      //1.5 ms
  if (RENDERBVH)
    scn::scene->render_bb();


  //cout << scn::player_torpedo->ray() << endl;

  //Vector frw2=scn::frw*100.0f;
  //frw2.draw(scn::pos);

  // RENDER INFORMATION TEXTS (830 us)
  if (TEXT) {
    ortho();

    if (!frame and not BENCHMARK)
      Text::evaluate();    //which rendering method is the best?
    if (paused) {
      txt::paused.center();
      txt::paused.render(TEXT_AUTO);
    }
    if (not scn::player_ship->is_active()) {
      txt::gameover.center();
      txt::gameover.render(TEXT_AUTO);
    }
    if (CURSOR and cursor::enabled and scn::player_ship->is_active()) {
      sprintf(txt::buffer, "d = %.1f m",
	      (scn::pos - cursor::position).length());
      txt::distance = txt::buffer;
      if (BENCHMARK)
	txt::distance.render(TEXT_TEXTURE);
      else
	txt::distance.render(TEXT_AUTO);
    }
    Vector vel=scn::player_ship->state().vel();
    sprintf(txt::buffer, "v = |(%.1f, %.1f, %.1f)| = %.1f m/s",
	    vel[X], vel[Y], vel[Z], vel.length());
    txt::velocity = txt::buffer;
    if (BENCHMARK)
      txt::velocity.render(TEXT_TEXTURE);  //240 us
    else
      txt::velocity.render(TEXT_AUTO);
    Vector ang=scn::rot.euler();
    sprintf(txt::buffer, "ang = (%.1f, %.1f, %.1f)",
	    ang[X]*r2d, ang[Y]*r2d, ang[Z]*r2d);
    txt::orientation = txt::buffer;
    if (BENCHMARK)
      txt::orientation.render(TEXT_TEXTURE);
    else
      txt::orientation.render(TEXT_AUTO);

    render_armour();  //83 us

    perspective();
  }
    
  //16 us
  glFlush();
  if (not BENCHMARK)
    glutSwapBuffers();

  ++frame;
}


/*****************************************
 *           RESHAPE FUNCTION            *
 *                                       *
 *****************************************/

void omforma(int width, int height){
  WIDTH=width; HEIGHT=height;
  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  gluPerspective(55, (GLfloat)WIDTH/(GLfloat)HEIGHT, CNEAR, CFAR);
}


/*****************************************
 *               MAIN LOOP               *
 *                                       *
 *****************************************/

//2 ms
void tomgang(void){
  _time::pt=_time::ct;             //previous time [us]
  _time::ct.update();       //current time [us]
  _time::dt=(_time::ct-_time::pt).get();          //timestep [s]

  //UPDATE THE MODELVIEW MATRIX (9 us)
  glGetDoublev(GL_MODELVIEW_MATRIX, xf::gl_modelview);
  xf::modelview=Matrix(xf::gl_modelview);

  //98 us
  if (PHYSICS and not paused) {
    cam::cam0=scn::pos + scn::rot*cam::eye;
    cam::camparams[2]=cam::cam0[X];
    cam::camparams[3]=cam::cam0[Y];
    cam::camparams[4]=cam::cam0[Z];

    //MODEL PROPAGATION
    scn::solver->step(_time::dt);                 //Player rigid body motion
    cam::camsolver->step(cam::camparams, _time::dt);   //Camera spring motion
    scn::trpsolver->step(_time::dt);              //Torpedo rigid body motion
    
    scn::player_torpedo->consume_fuel(_time::dt); //Torpedo fuel consumtion
  }

  //BVH UPDATING (1.75 ms)
  scn::scene->refresh_bb();

  //DISABLE CURSOR BY DEFAULT
  cursor::enabled=false;
  cursor::location=INF;

  //COLLISION DETECTION (30 us)
  if (CDET) {
    colldet->detect(scn::player_ship, scn::enemies);
    colldet->detect(scn::player_ship, scn::spacebase);
    colldet->detect(scn::player_torpedo, scn::enemies);
    colldet->detect(scn::player_torpedo, scn::spacebase);
    if (CURSOR) {
      //_time::tic();
      colldet->detect(scn::enemies, scn::player_ship->ray());   //cursor
      //cout << _time::toc() << endl;
      colldet->detect(scn::spacebase, scn::player_ship->ray()); //cursor
    }
  }

  //CULLING PARAMS (11 us)
  if (VFC){
    extract_vf(xf::vf);
    GBase::set_vf(xf::vf);
  }
  if (DETAILC)
    GBase::set_detail(cam::eye1, cam::dir, CNEAR, WIDTH, HEIGHT);


  //2.5 us
  if (not paused)
    glutPostRedisplay();
}


/***************************
 *    KEYBOARD FUNCS       *
 ***************************/

void tangent(unsigned char key, int x, int y){
  if (not paused) {
    switch(key){
    case '1':
      throttle_mode=1;
      scn::player_ship->thruster(0).force(Vector(0,0,-FORCEFW*.1));
      scn::player_ship->thruster(1).force(Vector(0,0,-FORCEFW*.1));
      scn::player_ship->thruster(2).force(Vector(0,0,FORCEBW*.1));
      break;
    case '2':
      throttle_mode=2;
      scn::player_ship->thruster(0).force(Vector(0,0,-FORCEFW*.5));
      scn::player_ship->thruster(1).force(Vector(0,0,-FORCEFW*.5));
      scn::player_ship->thruster(2).force(Vector(0,0,FORCEBW*.5));
      break;
    case '3':
      throttle_mode=3;
      scn::player_ship->thruster(0).force(Vector(0,0,-FORCEFW));
      scn::player_ship->thruster(1).force(Vector(0,0,-FORCEFW));
      scn::player_ship->thruster(2).force(Vector(0,0,FORCEBW));
      break;
    case 127:        //DEL //ROLL LEFT
      scn::player_ship->thruster(0).gimbal(-ctrl::dx*2,0);
      scn::player_ship->thruster(1).gimbal(+ctrl::dx*2,0);
      forward_on();
      break;
    case ' ':        //FIRE WEAPON
      scn::player_ship->activate_torpedo();
      break;
    case 'f':
    case 'F':
      /*
      fullscreen = !fullscreen;
      if (fullscreen)
	glutEnterGameMode();
      else
	glutLeaveGameMode();
      */
      break;
    }
  }
  
  switch(key){
  case 'p':
  case 'P':
    paused = !paused;
    break;
  case 'q':        //Quit
  case 'Q':
  case 'x':
  case 'X':
  case 27:
    //ok, this isn't paricularly elegant, but it works... for now.
    (*scn::enemies)["row0"]["enemy0"].free_upon_delete(true);
    (*scn::enemies)["row0"].remove("enemy0");  //remove shared data
    scn::enemies->free_upon_delete(false);  //lock all other leaves with same data
    delete scn::scene;
    delete cam::camera;
    delete scn::solver;
    delete cam::camsolver;
    delete colldet;
    if (SKYBOX)
      glDeleteLists(starbox_list[0], 6);
    if (TEXT){
      Text::destroy();
      glDeleteLists(armour_list, 1);
    }
    cout << "Oh, thank you! Thank you sooo much for exiting properly!!!" << endl;
    if (BENCHMARK)
      cout << "mean fps = " << frame/(_time::ct - _time::t0) << endl;
    cout << endl;
    //glutLeaveGameMode();
    exit(EXIT_SUCCESS);
  }
}

void upptangent(unsigned char key, int x, int y){
  if (not paused) {
    switch(key){
    case 127:        //DEL
      scn::player_ship->thruster(0).gimbal_reset();
      scn::player_ship->thruster(1).gimbal_reset();
      forward_off();
    }
  }
}

void specialtangent(int key, int x, int y){
  if (not paused) {
    switch(key){
    case GLUT_KEY_F1:
      cam::camera_mode=1;
      break;
    case GLUT_KEY_F2:
      cam::camera_mode=2;
      break;
    case GLUT_KEY_F3:
      cam::camera_mode=3;
      break;
    case GLUT_KEY_F4:
      cam::camera_mode=4;
      break;
    case GLUT_KEY_F5:
      cam::camera_mode=5;
      break;
    case GLUT_KEY_UP:
      scn::player_ship->thruster(0).gimbal(-ctrl::dx,0);
      scn::player_ship->thruster(1).gimbal(-ctrl::dx,0);
      forward_on();
      break;
    case GLUT_KEY_DOWN:
      scn::player_ship->thruster(0).gimbal(+ctrl::dx,0);
      scn::player_ship->thruster(1).gimbal(+ctrl::dx,0);
      forward_on();
      break;
    case GLUT_KEY_LEFT:
      /* nonrealistic (jerky gimbals), but yields better control */
      scn::player_ship->thruster(0).gimbal(0,-ctrl::dy);
      scn::player_ship->thruster(1).gimbal(0,-ctrl::dy);
      forward_on();
      break;
    case GLUT_KEY_RIGHT:
      scn::player_ship->thruster(0).gimbal(0,+ctrl::dy);
      scn::player_ship->thruster(1).gimbal(0,+ctrl::dy);
      forward_on();
      break;
    case GLUT_KEY_PAGE_UP:
      forward_on();
      break;
    case GLUT_KEY_PAGE_DOWN:
      backward_on();
      break;
    case GLUT_KEY_HOME:
      break;
    case GLUT_KEY_END:           //ROLL RIGHT
      scn::player_ship->thruster(0).gimbal(+ctrl::dx*2,0);
      scn::player_ship->thruster(1).gimbal(-ctrl::dx*2,0);
      forward_on();
      break;
    case GLUT_KEY_INSERT:
      break;
    }

    glutPostRedisplay();
  }
}

void specialupptangent(int key, int x, int y){
  if (not paused) {
    scn::player_ship->thruster(0).gimbal_reset();
    scn::player_ship->thruster(1).gimbal_reset();
    switch(key){
    case GLUT_KEY_UP:
      forward_off();
      break;
    case GLUT_KEY_DOWN:
      forward_off();
      break;
    case GLUT_KEY_LEFT:
      forward_off();
      break;
    case GLUT_KEY_RIGHT:
      forward_off();
      break;
    case GLUT_KEY_PAGE_UP:
      forward_off();
      break;
    case GLUT_KEY_PAGE_DOWN:
      backward_off();
      break;
    case GLUT_KEY_HOME:
      break;
    case GLUT_KEY_END:
      forward_off();
      break;
    case GLUT_KEY_INSERT:
      break;
    }
    
    glutPostRedisplay();
  }
}
