// Simple game using SDL and OpenGL

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 640
#define HEIGHT 480

GLfloat yaw;
GLfloat pitch;
int level;

static void subdivide (GLfloat point0[3], GLfloat point1[3], GLfloat point2[3], int level) {
	int coord;

	GLfloat midpoint[3][3];

	// Don't subdivide more at 0, just draw triangle
	if (level == 0) {
		glColor3fv(point0);
		glVertex3fv(point0);
        glColor3fv(point1);
		glVertex3fv(point1);
		glColor3fv(point2);
		glVertex3fv(point2);
		return;
	}

	// Calculate midpoint on each triangle
	for (coord = 0; coord < 3; coord++) {
		midpoint[0][coord] = (point0[coord] + point1[coord])*0.5;
		midpoint[1][coord] = (point1[coord] + point2[coord])*0.5;
		midpoint[2][coord] = (point2[coord] + point0[coord])*0.5;
	}

	// Subdivide each triangle into three more
	level--;
	subdivide(point0, midpoint[0], midpoint[2], level);
	subdivide(point1, midpoint[1], midpoint[0], level);
	subdivide(point2, midpoint[2], midpoint[1], level);
}

static void repaint() {
	int i;

	static GLfloat point[6][3] = {
		{1.0f,0.0f,0.0f},{-1.0f,0.0f,0.0f},
		{0.0f,1.0f,0.0f},{0.0f,-1.0f,0.0f},
		{0.0f,0.0f,1.0f},{0.0f,0.0f,-1.0f}
	};

	static int triangle[8][3] = { 
		{2,4,0},{2,0,5},{2,5,1},{2,1,4},{3,0,4},{3,5,0},{3,1,5},{3,4,1}
	};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -2.0f);

     // Rotate the object
	glRotatef(pitch, 1.0f, 0.0f, 0.0f);
	glRotatef(yaw, 0.0f, 1.0f, 0.0f);

	// Draw the triangles which make up the object
	glBegin(GL_TRIANGLES);

	for (i=0; i<8; i++) {
		subdivide(point[triangle[i][0]],point[triangle[i][1]],point[triangle[i][2]],level);
	}

	glEnd();
	yaw = yaw + 0.05;

	SDL_GL_SwapBuffers();
}

static void setup_sdl() {
	const SDL_VideoInfo* video;

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialise SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Quit SDL properly on exit
	atexit(SDL_Quit);

	// Get current video information
	video = SDL_GetVideoInfo();
	if (video == NULL) {
		fprintf(stderr, "Couldn't get video information %s\n", SDL_GetError());
		exit(1);
	}

	// Set minimim requirements for the OpenGL window
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_SetVideoMode( WIDTH, HEIGHT, video->vfmt->BitsPerPixel, SDL_OPENGL ) == 0) {
		fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
		exit(1);
	}
}

static void setup_opengl() {
	float aspect = (float)WIDTH / (float)HEIGHT;

	// Viewport to cover entire window
	glViewport(0, 0, WIDTH, HEIGHT);

	// Set camera projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, aspect, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);

	// Set clear color to gray
	glClearColor(0.5, 0.5, 0.5, 0);

	// Enable z-buffer tests
	glEnable(GL_DEPTH_TEST);

	// Do not draw back-facing polygons
	glDisable(GL_CULL_FACE);
}

static void main_loop() {
	SDL_Event event;

	while (1) {
		// Process pending events
		while (SDL_PollEvent( &event ) ) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					exit(0);
				break;
				case SDLK_KP_PLUS:
					level++;
					if (level > 5) level = 5;
					break;
				case SDLK_KP_MINUS:
					level--;
					if (level < 0) level = 0;
					break;
				default:
					// no default action
					break;
				}
			break;

			case SDL_MOUSEMOTION:
				pitch += event.motion.yrel;
				if (pitch < -70) pitch = -70;
				if (pitch > 70) pitch = 70;
				break;
			case SDL_QUIT:
				exit(0);
				break;
			}
		}

		// Update the screen
		repaint();

		// Wait 50ms to avoid using al the CPU time
		SDL_Delay(50);
	}
}

int main(int argc, char ** argv) {
	setup_sdl();
	setup_opengl();

	yaw = 45;
	pitch = 0;
	level = 2;

	main_loop();

	return 0;
}
