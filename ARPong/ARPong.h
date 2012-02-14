#ifndef ARPONG_H
#define ARPONG_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/arMulti.h>

void init(void);
void cleanup(void);
void keyEvent(unsigned char key, int x, int y);
void mainLoop(void);
void draw(bool field_visible, double field_trans[3][4], bool pad1_visible, double pad1_trans[3][4]);

float timeElapsed(void);

#endif
