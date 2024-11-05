// -*-c++-*-
#ifndef STUFF_H
#define STUFF_H

#include <limits>
#include <cmath>

#define INF std::numeric_limits<float>::infinity()

static const float d2r = M_PI/180.0f;
static const float r2d = 180.0f/M_PI;

#define MAX2(a,b)     (a>b)? (a):(b)
#define MAX3(a,b,c)   MAX2(a,MAX2(b,c))
#define MAX4(a,b,c,d) MAX2(a,MAX3(b,c,d))
#define MIN2(a,b)     (a<b)? (a):(b)
#define MIN3(a,b,c)   MIN2(a,MIN2(b,c))

#define SIGN(x) (x<0)? -1:1


#endif
