#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2

float px, py; // player position
float pdx, pdy, pa; // player angle and dx, dy

void drawPlayer()
{
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px, py); 
	glEnd();
	
	// draw direction we are looking
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px, py);
	glVertex2i(px + pdx*5, py + pdy*5);
	glEnd();
}

int mapX=8,mapY=8,mapS=64;
int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

void drawMap2D()
{
	int x,y,xo,yo;
	for (y = 0; y< mapY; y++)
	{
		for (x = 0; x < mapX; x++) 
		{
			if (map[y*mapX + x]==1)
			{
				glColor3f(1,1,1); 
			}
			else
			{
				glColor3f(0,0,0);
			}
			
			xo=x*mapS; yo=y*mapS;
			// define vertices of each box 
			glBegin(GL_QUADS);
			glVertex2i(xo        +1, yo        +1);
			glVertex2i(xo        +1, yo + mapS -1); // bottom left
			glVertex2i(xo + mapS -1, yo + mapS -1);
			glVertex2i(xo + mapS -1, yo        +1);
			glEnd();
		}
	}
}

float dist(float ax, float ay, float bx, float by, float ang)
{
	return ( sqrt((bx-ax)* (bx-ax) + (by-ay)*(by-ay)) ); // pytahgorean
}

void drawRays2D()
{
	int r,mx,my,mp,dof;
	float rx, ry, ra, xo, yo;
	ra = pa; //ray's angle to player's angle
	for (r =0; r < 1; r++)
	{
		// Find where the ray intersects on the horizontal
		dof = 0;
		float disH = 100000000, hx = px, hy = py;
		float arcTan = -1/tan(ra);
		if (ra > PI) // ray's angle is down
		{
			ry = (((int)py>>6)<<6)-0.0001;
			rx = (py-ry)*arcTan+px;
			
			// these are the offsets, i.e. how far away is the next intersection
			yo = -64; 
			xo = -yo * arcTan;
		}
		if (ra < PI) // ray's angle is up
		{
			ry = (((int)py>>6)<<6)+64;
			rx = (py-ry)*arcTan+px;
			
			// these are the offsets, i.e. how far away is the next intersection
			yo = 64; 
			xo = -yo * arcTan;
		}
		if (ra == 0 || ra == PI) // if straight left or right, never hit horizontal wall
		{
			rx = px;
			ry = py;
			dof = 8;
		}
		while (dof < 8)
		{
			// find in map array
			mx = (int)(rx) >> 6; // divide by 64
			my = (int)(ry) >> 6;
			mp = my*mapX + mx;
			if (mp > 0 && mp < mapX*mapY && map[mp] == 1) // hit wall
			{ 
				hx = rx;
				hy = ry;
				disH = dist(px, py, hx, hy, ra);
				dof=8; 
			} 
			else 
			{
				rx += xo;
				ry += yo;
				dof += 1; 
			}
			
		}
		
		// Find where the ray intersects on the vertical
		dof = 0;
		float disV = 10000000, vx = px, vy = py;
		float nTan = -tan(ra);
		if (ra > P2 && ra < P3) // ray's angle is left
		{
			rx = (((int)px>>6)<<6)-0.0001;
			ry = (px-rx)*nTan+py;
			
			// these are the offsets, i.e. how far away is the next intersection
			xo = -64; 
			yo = -xo * nTan;
		}
		if (ra < P2 || ra > P3) // ray's angle is right
		{
			rx = (((int)px>>6)<<6)+64;
			ry = (px-rx)*nTan+py;
			
			// these are the offsets, i.e. how far away is the next intersection
			xo = 64; 
			yo = -xo * nTan;
		}
		if (ra == 0 || ra == PI) // if straight up or down, never hit horizontal wall
		{
			rx = px;
			ry = py;
			dof = 8;
		}
		while (dof < 8)
		{
			// find in map array
			mx = (int)(rx) >> 6; // divide by 64
			my = (int)(ry) >> 6;
			mp = my*mapX + mx;
			if (mp > 0 && mp < mapX*mapY && map[mp] == 1) // hit wall
			{ 
				vx = rx;
				vy = ry; 
				disV = dist(px, py, vx, vy, ra);
				dof=8; 
			} 
			else 
			{
				rx += xo;
				ry += yo;
				dof += 1; 
			}
			
		}
		
		if (disV<disH){rx = vx; ry = vy;}
		else {rx = hx; ry = hy;}
		
		glColor3f(1,0,0);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(px,py);
		glVertex2i(rx,ry);
		glEnd();
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	drawRays2D();
	glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
	if (key == 'a') // change what angle we are looking
	{ 
		pa -= 0.1; 
		if (pa<0) 
		{
			pa += 2*PI;
		}
		pdx = cos(pa) * 5;
		pdy = sin(pa) * 5;
	}
	if (key == 'd') // change angle counterclockwise
	{
		pa += 0.1; 
		if (pa>2*PI) 
		{
			pa = 0;
		}
		pdx = cos(pa) * 5;
		pdy = sin(pa) * 5;
	} 
	if (key == 'w') {px += pdx; py += pdy;} // move in direction
	if (key == 's') {px -= pdx; py -= pdy;} // move counter direction
	glutPostRedisplay();
}

void init()
{
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, 1024, 512, 0);
	px=300; py=300;
	pdx = cos(pa) * 5; pdy = sin(pa) * 5;
}

int main(int argc, char** argv)
{ 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1024,512);
	glutCreateWindow("Kevin Raycasting");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
}
