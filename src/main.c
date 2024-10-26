// VS Code: Use the Build task (Ctrl+Shift+B) to compile your project, 
// and then use the Run and Debug view (Ctrl+Shift+D) to run and debug your program.

// in MYSYS2, build and run with:
// $ gcc -v -g main.c -o main.exe -L/ucrt64/lib -lfreeglut -lopengl32 -lglu32 && ./main.exe

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535 // player rotation is based on Radians and not degrees, so pi, pi/2, 2pi, 3pi/2.
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 // one degree in radians converted to a float

float px,py,pdx,pdy,pa; // player position, delta x, delta y, angle of player

void drawPlayer()
{
  // player
  glColor3f(1,1,0); // yellow
  glPointSize(8); // pixel representing player
  glBegin(GL_POINTS);
  glVertex2i(px,py); 
  glEnd();

  // player direction indicator 
  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(px,py); 
  glVertex2i(px + pdx*5, py + pdy*5); 
  glEnd();
}

int mapX=8,mapY=8,mapS=64; 
int map[]=
{
  1,1,1,1,1,1,1,1,  
  1,0,1,0,0,0,0,1,  
  1,0,0,0,0,1,0,1,  
  1,0,0,1,0,0,0,1,  
  1,0,0,0,0,0,0,1,  
  1,0,0,0,1,1,0,1,  
  1,0,1,0,1,0,0,1,  
  1,1,1,1,1,1,1,1,  
};

void drawMap2D()
{
  int x,y,xo,yo;
  for(y=0; y<mapY; y++)
  {
    for(x=0; x<mapX; x++)
    {
      if(map[y * mapX + x] == 1) { glColor3f(1,1,1); } else { glColor3f(0,0,0); }
      xo=x*mapS; yo=y*mapS;
      glBegin(GL_QUADS);
      glVertex2i(xo        + 1, yo        + 1);
      glVertex2i(xo        + 1, yo + mapS - 1);
      glVertex2i(xo + mapS - 1, yo + mapS - 1);
      glVertex2i(xo + mapS - 1, yo        + 1);
      glEnd();
    }
  }
}

// return the distance between the player and the rays end point using pythagorean theorem
float dist(float ax, float ay, float bx, float by, float ang)
{
  return ( sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)) );
}

void drawRays2D()
{
  int r,mx,my,mp,dof; float rx,ry,ra,xo,yo,disT;
  ra=pa-DR*30; if(ra<0) { ra+=2*PI;} if(ra>2*PI) { ra-=2*PI;} // init ray angle starting in a 30 degrees to each side
  for(r=0; r<60; r++) // number of rays to draw from player's perspective
  {
    // --- Check Horizontal Lines ---
    dof=0;
    // set the disH distance really high by default as the "dist" algo above tries to find the SHORTEST distance
    // also save the horizontal's x position
    float disH=1000000,hx=px,hy=py;
    float aTan=-1/tan(ra);
    // bit shifting magic, round ray's y position to nearest 64 value, 
    // divide the value by 64 by bitshifting it six down, and multiplying
    // it by 64 or bitshifting it 6 up. We subtract 0.0001 for accuracy
    if(ra>PI) { ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry)*aTan+px; yo=-64; xo=-yo*aTan; } // looking up
    if(ra<PI) { ry=(((int)py>>6)<<6)+64;     rx=(py-ry)*aTan+px; yo= 64; xo=-yo*aTan; } // looking down
    if(ra==0 || ra==PI) { rx=px; ry=py; dof=8;} // looking straight left or right

    // you don't want to check for angles collisions forever, let's stop the ray casting beyond 8 blocks
    // we are checking for the angle where the ray hits the lines
    while(dof<8)
    {
      mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
      // HIT GRID LINE, we save the ray's x and y pos, and calculate the ray's distance from the player
      if(mp>0 && mp<mapX*mapY && map[mp]>0){ hx=rx; hy=ry; disH=dist(px,py,hx,hy,ra); dof=8; }
      else{ rx+=xo; ry+=yo; dof+=1; } // if we didn't hit a wall
    }

    // --- Check Vertical Lines ---
    dof=0;
    // same as the floats for horizontal but VERTICAL
    float disV=1000000,vx=px,vy=py;
    float nTan=-tan(ra);
    // same as above but different part of the radian circle for vertical lines
    if(ra>P2 && ra<P3) { rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx)*nTan+py; xo=-64; yo=-xo*nTan; } // looking left
    if(ra<P2 || ra>P3) { rx=(((int)px>>6)<<6)+64;     ry=(px-rx)*nTan+py; xo= 64; yo=-xo*nTan; } // looking right
    if(ra==0 || ra==PI) { rx=px; ry=py; dof=8;} // looking straight UP or DOWN

    // you don't want to check for angles collisions forever, let's stop the ray casting beyond 8 blocks
    // we are checking for the angle where the ray hits the lines
    while(dof<8)
    {
      mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
      if(mp>0 && mp<mapX*mapY && map[mp]>0){ vx=rx; vy=ry; disV=dist(px,py,vx,vy,ra); dof=8; } // hit grid line
      else{ rx+=xo; ry+=yo; dof+=1; } // if we didn't hit a wall
    }
    // we want the shortest distance always, so update that
    if(disV<disH){ rx=vx; ry=vy; disT=disV; glColor3f(0.9,0,0); } // vertical wall hit
    if(disH<disV){ rx=hx; ry=hy; disT=disH; glColor3f(0.7,0,0); }  // horizontal wall hit
    // RENDERING the actual ray in the program as an one liner, this time in RED
    glLineWidth(3); glBegin(GL_LINES); glVertex2i(px,py); glVertex2i(rx,ry); glEnd();

    //---Draw 3D Walls---
    // find distance between player angle and ray angle to prevent FISHEYE effect
    float ca=pa-ra; if(ca<0) { ca+=2*PI;} if(ca>2*PI) { ca-=2*PI;} disT=disT*cos(ca); // fix fisheye with COSINE
    // Formula: Constant (object) / Ray Distance = Wall Height
    // More distance == smaller!
    // window will be 320x160 pixels
    float lineH=(mapS*320)/disT; if(lineH>320) {lineH=320;} // line height
    float lineO=160-lineH/2;                                // offset to the center of the screen
    // draw the line every 8 pixel
    glLineWidth(8);glBegin(GL_LINES);glVertex2i(r*8+530,0);glVertex2i(r*8+530,lineH+lineO);glEnd();

    // after you draw the ray, increase the ray by one radian
    ra+=DR; if(ra<0) { ra+=2*PI;} if(ra>2*PI) { ra-=2*PI;} // set the limit again
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
  if(key=='a'){ pa-=0.1; if(pa <    0) { pa += 2*PI; } pdx=cos(pa)*5; pdy=sin(pa)*5; };
  if(key=='d'){ pa+=0.1; if(pa > 2*PI) { pa -= 2*PI; } pdx=cos(pa)*5; pdy=sin(pa)*5; };
  if(key=='w'){ px += pdx; py += pdy; };
  if(key=='s'){ px -= pdx; py -= pdy; };
  glutPostRedisplay();
}

void init()
{
  glClearColor(0.3,0.3,0.3,0);
  gluOrtho2D(0,1024,512,0);
  px=300; py=300;
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1024, 512);
  glutCreateWindow("3DSage - Raycaster");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(buttons);
  glutMainLoop();
}
