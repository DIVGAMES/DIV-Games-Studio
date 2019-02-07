#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_structs.h>
#include <stdio.h>

#include <math.h>

// int main(void) {}

#define GLOBALS
#include "div.h"

#define to_degrees(X) (X * (180.0 / M_PI))

#define WALL_MAX 1000

int phys_test(void);

static cpVect gravity;
static cpSpace *space;
static cpShape *walls[WALL_MAX];
int wallnum = 0;

// static cpBody *ballBody;
//[65535];

int nbody = 0;

// static cpShape *ballShape;
static cpFloat timeStep;
static cpVect pos;
static double ang2;

static int init = 0;

#define iloc_len (mem[5] + mem[6])
#define imem (mem[8])
#define MAX_JOINTS 10

#define TRUE 1
#define FALSE 0

typedef struct procbody {
  cpBody *body;
  cpShape *shape;
  int procid;
  struct procbody *next;
  struct procbody *prev;
  byte changed;
  struct cpConstraint *joints[MAX_JOINTS];
  int idjoin[MAX_JOINTS];

} procbody;

procbody *bodies = NULL;
#define proc(x) ((process *)&mem[x])

#define MAX_POINTS 255

typedef struct _polystruct {
  int num;
  cpVect *points;
} polystruct;

typedef struct _gps {
  int file;
  int graph;
  polystruct *ps;
} gps;

//  MAXSIZEX = 32; // arbitrary
//  MAXSIZEY = 32; // abritrary

#define MAX_POLYS 200

gps polys[MAX_POLYS];

typedef struct _vector {
  int charnum;
  int prev;
  int sx, sy;
  int ex, ey;
  int x, y;
  int next;
  int status;
} vector;

int a[255][255];
//  A: array[1..MAXSIZEX, 1..MAXSIZEY] of integer;
vector v[655350];
int Vnum = 0;

int addsquarevector(j, k) {

  int m;

  //  Vnum := Vnum + 1;

  v[Vnum].prev = Vnum + 3;
  v[Vnum].sx = j;
  v[Vnum].sy = k;
  v[Vnum].ex = j + 1;
  v[Vnum].ey = k;
  v[Vnum].next = Vnum + 1;
  v[Vnum].status = 0;

  Vnum++;

  v[Vnum].prev = Vnum - 1;
  v[Vnum].sx = j + 1;
  v[Vnum].sy = k;
  v[Vnum].ex = j + 1;
  v[Vnum].ey = k + 1;
  v[Vnum].next = Vnum + 1;
  v[Vnum].status = 0;

  Vnum++;

  v[Vnum].prev = Vnum - 1;
  v[Vnum].sx = j + 1;
  v[Vnum].sy = k + 1;
  v[Vnum].ex = j;
  v[Vnum].ey = k + 1;
  v[Vnum].next = Vnum + 1;
  v[Vnum].status = 0;

  Vnum++;

  v[Vnum].prev = Vnum - 1;
  v[Vnum].sx = j;
  v[Vnum].sy = k + 1;
  v[Vnum].ex = j;
  v[Vnum].ey = k;
  v[Vnum].next = Vnum - 3;
  v[Vnum].status = 0;

  Vnum++;
}

void procvector(pid) {

  int j, k;
  int *ptr;
  byte *a;
  int w;
  int h;

  if ((ptr = (int *)graphs[proc(pid)->file].grf[proc(pid)->graph]) == NULL) {
    return;
  }
  a = (byte *)ptr + 64 + ptr[15] * 4;
  w = ptr[13];
  h = ptr[14];

  Vnum = 0;

  for (j = 0; j < w; j++)
    for (k = 0; k < h; k++)
      if (a[j + k * w] != 0)
        addsquarevector(j, k);
}

void removevector(int mm, int mm2) {

  int p, n;

  p = v[mm].prev;
  v[p].next = v[mm2].next;

  n = v[mm2].next;
  v[n].prev = p;
}

void removevectors(int m, int m2) {

  removevector(m, m2);
  removevector(m2, m);

  v[m].status = -1;
  v[m2].status = -1;
}

byte equalpoints(int p1x, int p1y, int p2x, int p2y) {
  byte r;

  r = FALSE;

  if (p1x == p2x && p1y == p2y)
    r = TRUE;

  return r;
}

byte equalvectors(int m, int m2) {

  int msx, msy, mex, mey;
  int m2sx, m2sy, m2ex, m2ey;

  byte r;

  r = FALSE;

  if (v[m].status != -1) {

    msx = v[m].sx;
    msy = v[m].sy;
    mex = v[m].ex;
    mey = v[m].ey;
    m2sx = v[m2].sx;
    m2sy = v[m2].sy;
    m2ex = v[m2].ex;
    m2ey = v[m2].ey;

    if (equalpoints(msx, msy, m2sx, m2sy) && equalpoints(mex, mey, m2ex, m2ey))
      r = TRUE;

    if (equalpoints(msx, msy, m2ex, m2ey) && equalpoints(mex, mey, m2sx, m2sy))
      r = TRUE;
  }

  return r;
}

void lengthenvector(void) {

  int m, m2;

  // now we have vectors, but some vectors have multiple points.
  // so let's turn two vectors into one longer vector. Okay? Okay!

  for (m = 0; m < Vnum; m++) {

    if (v[m].prev != 0 && v[m].status > -1) {
      if ((v[v[m].prev].sx == v[m].ex) || (v[v[m].prev].sy == v[m].ey)) {
        v[v[m].prev].ex = v[m].ex;
        v[v[m].prev].ey = v[m].ey;
        v[v[m].prev].next = v[m].next;
        v[v[m].next].prev = v[m].prev;
        v[m].status = -1;
      }
    }
  }
}

void simplifyvector(void) {

  int m, m2;

  for (m = 0; m < Vnum; m++)
    for (m2 = m + 1; m2 < Vnum; m2++)
      if (equalvectors(m, m2))
        removevectors(m, m2);
}

#define mappixel(x, y) si[y * h + x]

#define savepoint                                                              \
  ps->points[numpoints] = cpv(x, y);                                           \
  ps->num = ++numpoints;                                                       \
  dir++;                                                                       \
  if (dir == 4)                                                                \
    dir = 0;                                                                   \
  printf("point[%d]: %d %d %x\n", numpoints, x, y, si[y * h + x]);

polystruct *pixel_to_poly(pid) {

  int *ptr;
  int numpoints = 0;
  int x = 0;
  int y = 0;
  int xt = 0;
  int yt = 0;
  int w = 0;
  int h = 0;
  int dir = -1;
  int i = 0;
  int j = 0;

  byte *si = NULL;
  polystruct *ps;

  if ((ptr = (int *)graphs[proc(pid)->file].grf[proc(pid)->graph]) == NULL) {
    printf("NO GRAPH!\n");
    return NULL;
  }

  w = ptr[13] / 2;
  h = ptr[14] / 2;

  for (i = 0; i < MAX_POLYS; i++) {
    if (polys[i].ps == NULL)
      break;

    if (polys[i].file == proc(pid)->file &&
        polys[i].graph == proc(pid)->graph) {
      return polys[i].ps;
    }
  }

  ps = (polystruct *)div_malloc(sizeof(polystruct));

  procvector(pid);

  simplifyvector();

  lengthenvector();

  j = 0;
  i = 0;
  do { // v[i].status==0) {
    if (v[i].status == 0) {
      //		printf("[%d] %d %d %d %d ->[%d]\n",i,v[i].sx, v[i].sy,
      //v[i].ex, v[i].ey, v[i].next);
      v[i].x = (v[i].sx * (proc(pid)->size / 100)) - w;
      v[i].y = (v[i].sy * (proc(pid)->size / 100)) - h;
      //		printf("i: %d, x: %d y: %d\n",numpoints,x,y);
      numpoints++;
    }
    i = v[i].next;
    j++;
  } while (i != 0);

  ps->num = numpoints;
  ps->points = (cpVect *)div_malloc(sizeof(cpVect) * numpoints);

  j = 0;
  for (i = 0; i < numpoints; i++) {
    ps->points[i] = cpv(v[j].x, v[j].y);
    j = v[j].next;
  }

  for (i = 0; i < MAX_POLYS; i++) {
    if (polys[i].ps == NULL)
      break;

    if (polys[i].file == proc(pid)->file &&
        polys[i].graph == proc(pid)->graph) {
      return polys[i].ps;
    }
  }

  polys[i].file = proc(pid)->file;
  polys[i].graph = proc(pid)->graph;
  polys[i].ps = ps;
  printf("new poly rendered: %d\n", i);

  //  return NULL;
  return ps;
}

struct procbody *findbody(int id) {
  struct procbody *f = bodies;

  for (; f; f = f->next) {
    if (f->procid == id) {
      return f;
    }
  }

  return NULL;
}

procbody *procbody_new(int pid, cpFloat mass, cpFloat moment) {
  procbody *newb = (procbody *)div_malloc(sizeof(procbody));

  if (!newb)
    return NULL;

  memset(newb->joints, 0, sizeof(struct cpConstraint *) * MAX_JOINTS);
  memset(newb->idjoin, 0, MAX_JOINTS);

  newb->next = bodies;
  newb->prev = NULL;
  newb->body = cpSpaceAddBody(
      space, cpBodyNew(mass, moment)); // : moment);cpBodyNew(mass, moment));
  newb->procid = pid;
  newb->changed = 0;

  if (pid != 0) {
    //	cpBodySetAngle(newb->body,-(proc(pid)->angle)/1000);
  }
  // link it in
  if (bodies) {
    bodies->prev = newb;
  }
  bodies = newb;
  return newb;
}

void procbody_delete(procbody *body) {
  procbody *body2;

  int i = 0;
  int j = 0;
  if (body == NULL)
    return;

  procbody *p = body->prev;
  procbody *n = body->next;

  if (p != NULL) {
    p->next = body->next;
  } else {
    bodies = body->next;
  }

  if (n != NULL) {
    n->prev = body->prev;
  }

  cpSpaceRemoveShape(space, body->shape);
  cpSpaceRemoveBody(space, body->body);
  cpShapeFree(body->shape);
  cpBodyFree(body->body);

  for (i = 0; i < MAX_JOINTS; i++) {

    if (body->joints[i] != NULL) {
      cpSpaceRemoveConstraint(space, body->joints[i]);
      body2 = findbody(body->idjoin[i]);

      for (j = 0; j < MAX_JOINTS; j++) {
        if (body2->joints[j] == body->joints[i]) {
          body2->joints[j] = NULL;
          j = MAX_JOINTS;
        }
      }
      body->joints[i] = NULL;
    }
  }

  if (i != MAX_JOINTS) {
  }

  div_free(body);
}

void phy_wall_create(void) {

  int diam = getparm();
  int y2 = getparm();
  int x2 = getparm();
  int y1 = getparm();
  int x1 = getparm();

  int wallnum = 0;
  while (walls[wallnum] != NULL && wallnum < WALL_MAX) {
    wallnum++;
  }
  if (wallnum >= WALL_MAX) {
    printf("too many walls\n");
    retval(-1);
    return;
  }
  //	printf("wallnum: %d\n",wallnum);

  //	walls[wallnum] = (cpShape*)div_malloc(sizeof(cpShape));

  walls[wallnum] =
      cpSegmentShapeNew(space->staticBody, cpv(x1, y1), cpv(x2, y2), diam);
  walls[wallnum]->e = 0.9;

  cpShapeSetFriction(walls[wallnum], 1);
  cpSpaceAddShape(space, walls[wallnum]);

  //	wallnum++;
  retval(wallnum);
}

void phy_wall_destroy(void) {
  int id = getparm();

  if (cpSpaceContainsShape(space, walls[id])) {
    cpSpaceRemoveShape(space, walls[id]);
    cpShapeFree(walls[id]);
    //		printf("Removed wall %d\n",id);
    walls[id] = NULL;
    retval(0);
  }

  retval(id);
}

void phy_body_create_complex(void) {

  int mass = getparm();
  int h = getparm();
  int w = getparm();
  int pid = getparm();

  polystruct *ps = pixel_to_poly(pid);
  if (ps == NULL) {
    printf("poly creation failed\n");
    retval(0);
  }

  cpVect points[] = {cpv(-w / 2, -h / 2), cpv(w / 2, -h / 2), cpv(w / 2, h / 2),
                     cpv(-w / 2, h / 2)};

  // printf("points: %d\n",ps->num);

  cpFloat moment = cpMomentForPoly(mass, ps->num, ps->points, cpvzero, 0);
  // cpFloat moment = cpMomentForPoly(mass, 4, points, cpvzero,0);

  // Setup procbody
  procbody *newb = NULL;
  cpBody *body = NULL;
  if (pid > 0) {
    newb = procbody_new(pid, mass, moment);
    cpBodySetPosition(newb->body, cpv(proc(pid)->x, proc(pid)->y));
    body = newb->body;
  } else {
    body = space->staticBody;
  }
  // for (i=0;i<ps->num;i++)
  // printf("x: %d y: %d\n",ps->points[i]
  cpShape *shape = cpPolyShapeNewRaw(body, ps->num, ps->points, 0);
  //    cpShape* shape = cpPolyShapeNewRaw(body, 4, points,0);
  //    cpShape* shape = cpBoxShapeNew(body, (cpFloat)w, (cpFloat)h,0);

  //    cpShape* shape = cpBoxShapeNew(body, ps->num, ps->points,0);

  shape = cpSpaceAddShape(space, shape);
  shape->e = 0.75;
  cpShapeSetFriction(shape, 0.7);
  if (pid > 0) {
    newb->shape = shape;
  }

  retval(0);
}

void phy_body_create_box(void) {

  int mass = getparm();
  int h = getparm();
  int w = getparm();
  int pid = getparm();

  cpVect points[] = {cpv(-w / 2, -h / 2), cpv(w / 2, -h / 2), cpv(w / 2, h / 2),
                     cpv(-w / 2, h / 2)};

  cpFloat moment = cpMomentForPoly(mass, 4, points, cpvzero, 0);

  // Setup procbody
  procbody *newb = NULL;
  cpBody *body = NULL;
  if (pid > 0) {
    newb = procbody_new(pid, mass, moment);
    cpBodySetPosition(newb->body, cpv(proc(pid)->x, proc(pid)->y));
    body = newb->body;
  } else {
    body = space->staticBody;
  }
  cpShape *shape = cpPolyShapeNewRaw(body, 4, points, 0);

  //    cpShape* shape = cpBoxShapeNew(body, (cpFloat)w, (cpFloat)h,0);
  shape = cpSpaceAddShape(space, shape);
  shape->e = 0.75;
  cpShapeSetFriction(shape, 0.7);
  if (pid > 0) {
    newb->shape = shape;
  }

  retval(0);
}

void phy_body_create_circle(void) {

  int mass = getparm();
  int diameter = getparm();
  int pid = getparm();

  // The moment of inertia is like mass for rotation
  // Use the cpMomentFor*() functions to help you approximate it.
  cpFloat radius = ((float)diameter) / 2; // 20*(proc(pid)->size*100)/10000;
  cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

  // Setup procbody
  procbody *newb = NULL;
  cpBody *body = NULL;
  if (pid > 0) {
    newb = procbody_new(pid, mass, moment);
    cpBodySetPosition(newb->body, cpv(proc(pid)->x, proc(pid)->y));
    body = newb->body;
  } else {
    body = space->staticBody;
  }

  cpShape *shape = cpCircleShapeNew(body, radius, cpvzero);
  shape = cpSpaceAddShape(space, shape);
  shape->e = 0.75;
  cpShapeSetFriction(shape, 0.7);
  cpBodySetAngle(newb->body, -(proc(pid)->angle) / 1000);

  if (pid > 0) {
    newb->shape = shape;
  }

  retval(0);
}

void phy_body_create_box_center(void) {
  int cy = getparm();
  int cx = getparm();
  int friction = getparm();
  int elasticity = getparm();
  int momenti = getparm();
  int mass = getparm();
  int h = getparm();
  int w = getparm();
  int pid = getparm();

  cpVect points[] = {cpv(-cx, -cy), cpv(-cx, h - cy), cpv(w - cx, h - cy),
                     cpv(w - cx, -cy)};

  // cpFloat moment = ;
  cpFloat moment = (momenti == 0x7fffffff)
                       ? INFINITY
                       : (momenti == 0)
                             ? cpMomentForPoly(mass, 4, points, cpvzero, 0)
                             : (cpFloat)momenti;

  // Setup procbody
  procbody *newb = NULL;
  cpBody *body = NULL;
  if (pid > 0) {
    newb = procbody_new(pid, mass, moment);
    cpBodySetPosition(newb->body, cpv(proc(pid)->x, proc(pid)->y));
    body = newb->body;
    // cpBodySetCenterOfGravity(newb->body, cpv(w/2-cx, h/2-cy));
  } else {
    body = space->staticBody;
  }

  cpShape *shape = cpPolyShapeNew(body, 4, points, cpTransformIdentity, 0);
  shape = cpSpaceAddShape(space, shape);
  shape->e = 0.75;
  cpShapeSetFriction(shape, 0.7);

  if (pid > 0) {
    newb->shape = shape;
  }

  retval(0);
}

void phy_init(void) {
  // cpVect is a 2D vector and cpv() is a shortcut for initializing them.

  int a = 0;

  for (a = 0; a < WALL_MAX; a++) {
    walls[a] = NULL;
  }

  gravity = cpv(0, 1000);
  //	bodies = (struct procbody *)div_malloc(sizeof(struct procbody));
  //	bodies->next = NULL;
  //	bodies->prev = NULL;
  //	bodies->procid=0;
  //	bodies->shape=NULL;
  //	bodies->body=NULL;

  // Create an empty space.
  space = cpSpaceNew();

  cpSpaceSetGravity(space, gravity);

  // Add a static line segment shape for the ground.
  // We'll make it slightly tilted so the ball will roll off.
  // We attach it to space->staticBody to tell Chipmunk it shouldn't be movable.

  /*	walls[0] = cpSegmentShapeNew(space->staticBody, cpv(-60,height-220),
  cpv(wide/2,height-120), 10); walls[1] = cpSegmentShapeNew(space->staticBody,
  cpv(50,height+20), cpv(wide+60,height-40), 10); walls[2] =
  cpSegmentShapeNew(space->staticBody, cpv(200,height-240),
  cpv(wide+60,height-340), 10);
  //	walls[2] = cpSegmentShapeNew(space->staticBody, cpv(-60,height-420),
  cpv(wide-150,height-320), 5);

          walls[0]->e=0.9;
          walls[1]->e=0.9;
          walls[2]->e=0.9;

          cpShapeSetFriction(walls[0], 1);
          cpSpaceAddShape(space, walls[0]);
          cpShapeSetFriction(walls[1], 1);
          cpSpaceAddShape(space, walls[1]);
          cpShapeSetFriction(walls[2], 1);
          cpSpaceAddShape(space, walls[2]);
  */

  printf("iloc_len: %d id_start: %d\n", iloc_len, imem);

  // Now let's make a ball that falls onto the line and rolls off.
  // First we need to make a cpBody to hold the physical properties of the
  // object. These include the mass, position, velocity, angle, etc. of the
  // object. Then we attach collision shapes to the cpBody to give it a size and
  // shape.

  // The cpSpaceAdd*() functions return the thing that you are adding.
  // It's convenient to create and add an object in one line.
  // Now we create the collision shape for the ball.
  // You can create multiple collision shapes that point to the same body.
  // They will all be attached to the body and move around to follow it.

  //	ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius,
  //cpvzero)); 	cpShapeSetFriction(ballShape, 0.7);

  // Now that it's all set up, we simulate all the objects in the space by
  // stepping forward through time in small increments called steps.
  // It is *highly* recommended to use a fixed size time step.
  // cpSpaceStep(space, timeStep);
  /*
   *

  for(cpFloat time = 0; time < 2; time += timeStep){
          cpVect pos = cpBodyGetPos(ballBody);
          cpVect vel = cpBodyGetVel(ballBody);
          cpFloat ang = cpBodyGetAngle(ballBody);
          double ang2;
          ang2=to_degrees((double)ang);

          if(time>=2-timeStep)
                  printf(
                          "Time is %5.2f. ballBody is at (%5.2f, %5.2f, %5.2f
  (%5.2f)). It's velocity is (%5.2f, %5.2f)\n", time, pos.x, pos.y, ang, ang2,
  vel.x, vel.y
                  );



          cpSpaceStep(space, timeStep);
  }
*/
  // printf("BallBody: %x\n",(unsigned int)ballBody);
  init = 1;
  retval(0);
}

void phy_loop(void) {

  //	struct procbody *f = bodies;

  //	while(f!=NULL) {
  //		if(f->procid>0)
  //			if(proc(f->procid)->reserved.status==0)
  //				procbody_delete(f);
  //
  //		f=f->next;
  //
  //	}
  //    return;

  //	if(!ballBody)
  //		phys_init();

  //		for(cpFloat time = 0; time < 2; time += timeStep){
  timeStep = 1 / (float)(fps * 1.0f);

  //	printf("fps: %x\n",graphs[0].fpg);

  cpSpaceStep(space, timeStep);
  /*
          pos = cpBodyGetPos(ballBody);
          vel = cpBodyGetVel(ballBody);
          ang = cpBodyGetAngle(ballBody);

          ang2=to_degrees((double)ang);

  //	if(time>=2-timeStep)
                  printf(
                          //"Time is %5.2f. "
                          "ballBody is at (%5.2f, %5.2f, %5.2f (%5.2f)). It's
  velocity is (%5.2f, %5.2f)\n",
                          //time,
                          pos.x, pos.y, ang, ang2, vel.x, vel.y
                  );

          */

  // check for dead processes
}

void phy_body_move(void) {
  int y = getparm();
  int x = getparm();
  int id = getparm();

  procbody *body = findbody(id);

  if (body) {
    cpVect pos = cpBodyGetPosition(body->body);
    pos.x += x;
    pos.y += y;
    cpBodySetPosition(body->body, pos);
  }
  retval(0);
}

void phy_body_set_position(void) {
  int y = getparm();
  int x = getparm();
  int id = getparm();
  procbody *body = findbody(id);

  if (body) {
    cpBodySetPosition(body->body, cpv(x, y));
  }
  retval(0);
}

int getjnum(procbody *body) {
  int jnum = 0;

  while (body->joints[jnum] != NULL && jnum < MAX_JOINTS)
    jnum++;

  return jnum;
}

void phy_add_joint(void) {

  int angle = getparm();
  int distance = getparm();

  int offx1 = 0;
  int offy1 = 0;

  int offy2 = 0;
  int offx2 = 0;

  int id1 = getparm();
  int id2 = getparm();
  int j1 = 0;
  int j2 = 0;
  procbody *body1 = findbody(id1);
  procbody *body2 = findbody(id2);
  if (body1 != NULL && body2 != NULL) {
    j1 = getjnum(body1);
    j2 = getjnum(body2);

    if (j1 < MAX_JOINTS && j2 < MAX_JOINTS) {
      cpConstraint *joint = cpSpaceAddConstraint(
          space, cpPinJointNew(body1->body, body2->body,
                               cpv(-distance, -(angle / 1000)),
                               cpv(distance, -(angle / 1000))));
      body1->joints[j1] = joint;
      body1->idjoin[j1] = id2;

      body2->joints[j2] = joint;
      body2->idjoin[j2] = id1;
    }
  }
  retval(0);
}

void phy_body_apply_force_xy(void) {
  int y = getparm();
  int x = getparm();
  int id = getparm();
  procbody *body = findbody(id);
  if (body) {
    cpVect force = cpv(x, y);
    // cpBodyApplyForceAtLocalPoint(body->body, force, cpvzero);
    cpBodyApplyImpulseAtLocalPoint(body->body, force, cpvzero);
  }
  retval(0);
}

void phy_body_get_speed(void) {
  int id = getparm();
  procbody *body = findbody(id);
  if (body) {
    cpVect speed = cpBodyGetVelocity(body->body);
    double val = sqrt(speed.x * speed.x + speed.y * speed.y);
    retval(val);
  } else {
    retval(0);
  }
}

void phy_setxy(void) {
  int y = getparm();
  int x = getparm();
  int pid = getparm();

  procbody *pbody = findbody(pid);

  if (pbody != NULL) {
    cpBodySetPosition(pbody->body, cpv(x, y));
    cpBodySetVelocity(pbody->body, cpv(0, 0));
    proc(pid)->x = x;
    proc(pid)->y = y;
  }

  pbody->changed = 1;
  retval(0);
}

void phy_end(void) {
  // Clean up our objects and exit!
  // cpShapeFree(ballShape);
  // cpBodyFree(ballBody);
  // cpShapeFree(walls[0]);
  // cpSpaceFree(space);

  //	return 0;
}

void post_process(void) {

  if (!id_offset)
    return;

  procbody *f = findbody(id_offset);

  if (f == NULL)
    return;

  if (proc(id_offset)->reserved.status == 0) {
    procbody_delete(f);
    return;
  }

  pos = cpBodyGetPosition(f->body);

  ang2 = to_degrees(cpBodyGetAngle(f->body));

  //	vel = cpBodyGetVel(f->body);
  //	ang2=to_degrees((double)ang);

  //	cpCircleShapeSetRadius(f->shape, 20*(proc(id_offset)->size*100)/10000);
  if (f->changed == 0) {
    proc(id_offset)->x = pos.x;
    proc(id_offset)->y = pos.y;
    proc(id_offset)->angle = -ang2 * 1000;
  }

  f->changed = 0;

  //	printf("Proc data: id: %d radius: %d x0:%d y0:%d x1:%d
  //y1:%d\n",id_offset, (proc(id_offset)->radius),
  //(proc(id_offset)->reserved.x0),(proc(id_offset)->reserved.y0),(proc(id_offset)->reserved.x1),(proc(id_offset)->reserved.y1));
  //	printf("Proc data: id: %d graph: %d x0:%d y0:%d x1:%d
  //y1:%d\n",id_offset, (proc(id_offset)->graph),
  //(proc(id_offset)->reserved.x0),(proc(id_offset)->reserved.y0),(proc(id_offset)->reserved.x1),(proc(id_offset)->reserved.y1));
}

void __export divlibrary(LIBRARY_PARAMS) {

  COM_export("phy_init", phy_init, 0);

  COM_export("phy_body_create_circle", phy_body_create_circle, 3);
  COM_export("phy_body_create_complex", phy_body_create_complex, 4);
  COM_export("phy_body_create_box", phy_body_create_box, 4);
  COM_export("phy_body_create_box_center", phy_body_create_box_center, 9);
  COM_export("phy_body_set_position", phy_body_set_position, 3);
  COM_export("phy_body_move", phy_body_move, 3);
  COM_export("phy_body_get_speed", phy_body_get_speed, 1);
  COM_export("phy_setxy", phy_setxy, 3);
  COM_export("phy_body_apply_force_xy", phy_body_apply_force_xy, 3);

  COM_export("phy_wall_create", phy_wall_create, 5);
  COM_export("phy_wall_destroy", phy_wall_destroy, 1);

  COM_export("phy_add_joint", phy_add_joint, 4);

  // old stuff
  COM_export("phys_init", phy_init, 0);
  COM_export("add_fixed_body", phy_wall_create, 5);
  COM_export("remove_fixed_body", phy_wall_destroy, 1);
}

void process_fpg(char *fpg, int32_t len) {
  printf("Processing fpg: %d\n", len);
  return;
}
void process_map(char *map, int32_t len) {
  printf("Processing map: %d\n", len);
  return;
}

void post_process_buffer(void) {
  if (!init)
    phy_init();
  phy_loop();
}

void __export divmain(COMMON_PARAMS) {
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("post_process_buffer", post_process_buffer);
  //	DIV_export("background_to_buffer",background_to_buffer);
  DIV_export("process_fpg", process_fpg);
  DIV_export("process_map", process_map);

  DIV_export("post_process", post_process);
  //	phys_init();
}

void __export divend(COMMON_PARAMS) {}

/*

procedure raster2vector;

begin
  procvector;

  simplifyvector;

  lengthenvector;
end;

*/
