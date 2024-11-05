// -*-c++-*-
#ifndef WIGHT_SHAAR_H
#define WIGHT_SHAAR_H

/*************************
 *     Include Stuff     *
 *************************/
#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <GL/glut.h>

#include "stuff.h"
#include "ode.h"
#include "gtree.h"
#include "cd.h"
#include "timer.h"
#include "text.h"


/***********************************
 *     Some switches/constants     *
 ***********************************/
#define FIFO_SIZE 10
#define CFAR 10e3
#define CNEAR 1e-1
#define NENEM_X 6
#define NENEM_Y 4
#define FORCEFW 800.0
#define FORCEBW 1000.0

/***********************
 *     Some macros     *
 ***********************/
#define GL_ERR printf("GL FEL = %s\n", gluErrorString(glGetError()));

/*************************
 *    Primary globals    *
 *************************/
static int WIDTH=800, HEIGHT=600;
static bool SKYBOX=true, RENDERBVH=false, BENCHMARK=false,
  DETAILC=true, VFC=true, BFC=true,
  CDET=true, PHYSICS=true, LIGHTING=true,
  SPLASH=true, LOADINFO=true, TEXT=true, CURSOR=true;

enum StarBox {FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM};




/*********************************
 *      Function Definitions     *
 *********************************/
//Program Start
void init(void);
void info(void);
void help(void);
void splash(void);
int main(int argc, char **argv);
//Controls
void forward_on(void);
void forward_off(void);
void backward_on(void);
void backward_off(void);
//Initializers
void init_starbox(void);
void init_player(void);
void init_enemies(void);
void init_spacebase(void);
void init_armour(void);
//Renderers
void render_cursor(void);
void render_starbox(Vector& dir);
void render_spacebase(void);
void render_armour(void);
//Collision handlers
void collision(GBase *n1, GBase *n2);  //event handler aabb-aabb
void aim_lock(GBase *n, const Ray& r); //event handler aabb-ray
//Administrative
void extract_vf(Matrix& vf);         //extract view frustum
void ortho(void);
void perspective(void);


/**************************
 *     GLUT Functions     *
 **************************/
void visa(void);
void omforma(int width, int height);
void tomgang(void);
void tangent(unsigned char key, int x, int y);
void upptangent(unsigned char key, int x, int y);
void specialtangent(int key, int x, int y);
void specialupptangent(int key, int x, int y);


/****************************************************************************
 *                                      "scene"                             *
 *                                         |                                *
 *                +------------------------+------------------------+       *
 *                |                        |                        |       *
 *           "Wight-Shaar"             "enemies"               "space base" *
 *                |                        |                                *
 *             +--+-------+        +-------+------+---....                  *
 *             |          |        |       |      |                         *
 *          "player"  "torpedo"  "row0"  "row1" "row2"                      *
 *             |          |        |       |      |                         *
 *             |          |                   +---+---...                   *
 *       +-----+---+      +----+---+          |   |                         *
 *       |     |   |      |    |   |    "enemy8" "enemy9"                   *
 *      ship  rev frw  body nozzle jet            |                         *
 *             |   |                  +-----------+----------------+        *
 *    +--------+   +---+-----+        |           |                |        *
 *    |        |       |     |       body     turret_gimb         legs      *
 *motor_rev jet_rev   left right                  |                |        *
 *                     |     |    +-------+-------+   +-----+----+-+----+   *
 *     +-------+-------+     |    |       |           |     |    |      |   *
 *     |       |             |  turret turret_pipe leg_1 leg_2 leg_3 leg_4  *
 * motor_L   gimbal   +------+                                              *
 *             |      |      |                                              *
 * +-----------+  motor_R  gimbal                                           *
 * |           |             |                                              *
 *nozzle_L   jet_L  +--------+                                              *
 *                  |        |                                              *
 *              nozzle_R   jet_R                                            *
 ****************************************************************************/


//Some globals   (shame on me)

namespace scn {   //scene objects and stuff
  static GBase *scene = NULL;
  static Ship *player_ship = NULL; //child of player
  static Torpedo *player_torpedo = NULL; //child of player
  static GBase *enemies = NULL;
  static GFunc *spacebase = NULL;

  static const Vector frw0(0,0,-1);
  static Vector frw;   //ship forward vector
  static Vector pos;     //position of the ship
  static Quaternion rot; //orientation of the ship

  static ODE *solver = NULL;
  static ODE *trpsolver = NULL;

  static const unsigned hull = 100;
  static const float hullinv = .01f;

  static const float light_pos[]={-5,2,2,0};
}

namespace cam {   //camera stuff
  static Ship *camera = NULL;
  static ODE *camsolver = NULL;
  static vector<float> camparams;
  static Vertex cam0;  //camera nominal position (i-frame)
  static unsigned char camera_mode=3;

  static const Vector eye(0,5,10), cen(0,5,0), up(0,1,0);
  static const Vector eye0(0,0.274,-1.1);//(0,0.274,-1.481);
  static const Vector cen0(0,0,-10);
  static Vector eye1, cen1;

  static Vector old_up[FIFO_SIZE];
  static Vector dir;   //viewing direction (for skybox culling)

}

namespace col {   //some colours
  static const Color black(0,0,0,1);
  static const Color white(1,1,1,1);
  static const Color red(1,0,0,1);
  static const Color green(0,1,0,1);
  static const Color blue(0,0,1,1);
}

namespace _time {   //time related stuff
  static Timer ct, pt, t0;
  static float dt;
}

static GLuint starbox_list[6];
static GLuint armour_list;
static Color armour_color = col::black;

static unsigned char throttle_mode = 1;

static CD *colldet = NULL;

namespace cursor {   //cursor stuff
  static Vertex position;
  static float location = INF;
  static const GLubyte data[32] = {
    0xf0, 0x0f,
    0x80, 0x01,
    0x80, 0x01,
    0x80, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x80, 0x01,
    0x80, 0x01,
    0x80, 0x01,
    0xf0, 0x0f
  };
  static bool enabled = false;
}

namespace xf {   //transform stuff
  static GLdouble gl_projection[16];
  static Matrix projection;
  static GLdouble gl_modelview[16];
  static Matrix modelview;
  //static GLint gl_viewport[4];
  static Matrix vf;    //view frustum
}

static bool paused = false;

namespace txt {   //info text strings
  static Text paused = "PAUSED";
  static Text gameover = "GAME OVER !";
  static Text distance;
  static Text velocity;
  static Text orientation;

  static char buffer[256];
}

namespace ctrl {   //spacecraft control stuff
  static const float dx=30, dy=60;   //angular rate of change for gimbals
  static bool forward=false, backward=false;
}

//static bool fullscreen = false;


//-----------------------
//Globals related to the display function

static unsigned long frame=0;   //for benchmarking/clocking





#endif
