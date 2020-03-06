
/*


WahadloSprez.c 

Rozwi�zanie numeryczne wahad�a matematycznego metod� Eulera.
Napisane na potrzeby ksi��ki "Symulacje Komputerowe w Fizyce"

autor: Maciej Matyka, 2001 r.

 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>

#include <math.h>

#define PI 3.14

GLenum rgb, doubleBuffer=1, windType;
GLint windW, windH;

//#include "tkmap.c"

GLenum mode=0;

float point[2] = { 1.0, 1.0 };

double omega,teta,dt,g,r,t;
double pr,pvr,k,pr0;

static void Init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

    mode = GL_FALSE;


/* Sta�e */

	dt=0.01f;
	g=9.8f;
	r=0.9;

	k=0.9;

/* Warunki pocz�tkowe rozwi�zania numerycznego */

	pvr=0.4;

	t=0;
	omega=0.0f;
	teta=4.1f;

	pr0 = r;
	pr=pr0;


}

static void Reshape(int width, int height)
{

    windW = (GLint)width;
    windH = (GLint)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}


#define MOVEY 0.9





void RysujWektor(double x1, double y1, double x2, double y2, double scale, double px, double py)
{
		double kx,ky,kprimx,kprimy,vsize,ksize;

// Wylicz d�ugo�� wektora v i ustal d�ugo�� wektora k

	vsize=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	ksize=vsize*scale/4.f;


// Rysuj podstaw� strza�ki


    glBegin(GL_LINE_STRIP);

//	glVertex2f(x1,y1);
//	glVertex2f(((x2-x1)/vecsize),((y2-y1)/vecsize));

 	glVertex2f(x1+px,y1+py);
 	glVertex2f(x1+(x2-x1)*scale+px,y1+(y2-y1)*scale+py);

    glEnd();


// wyznacz wektor jednostkowy skierowany naprzeciwko v

	kx=-(x2-x1)/vsize;
	ky=-(y2-y1)/vsize;

// obr�� k o 30 stopni

   	kprimx=kx*cos(35*PI/180.f)+ky*sin(35*PI/180.f);
   	kprimy=-kx*sin(35*PI/180.f)+ky*cos(35*PI/180.f);

// wymn� przez jego d�ugo��

	kprimx*=ksize;
	kprimy*=ksize;

// rysuj kprim

    glBegin(GL_LINE_STRIP);

	glVertex2f(x1+(x2-x1)*scale+px,y1+(y2-y1)*scale+py);
	glVertex2f(x1+(x2-x1)*scale+kprimx+px,y1+(y2-y1)*scale+kprimy+py);

    glEnd();

// obr�� k o -30 stopni

   	kprimx=kx*cos(-35*PI/180.f)+ky*sin(-35*PI/180.f);
   	kprimy=-kx*sin(-35*PI/180.f)+ky*cos(-35*PI/180.f);

// wymn� przez jego d�ugo��

	kprimx*=ksize;
	kprimy*=ksize;

// rysuj kprim

    glBegin(GL_LINE_STRIP);

	glVertex2f(x1+(x2-x1)*scale+px,y1+(y2-y1)*scale+py);
	glVertex2f(x1+(x2-x1)*scale+kprimx+px,y1+(y2-y1)*scale+kprimy+py);

    glEnd();

}


#define frac (double)26


void RysujSprezyne(double x1, double y1, double x2, double y2)
{
	double vsize;
	double wx1,wy1,wx2,wy2;
	double _wx1,_wy1,_wx2,_wy2;
	int i;


	
	vsize=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));

	x2=(x2-x1);
	y2=(y2-y1);

	wx1=x1;
	wy1=y1;
	wx2=x1+(x2/vsize)*vsize*4/frac;
	wy2=y1+(y2/vsize)*vsize*4/frac;

	glBegin(GL_LINE_STRIP);
	glVertex2f(wx1,wy1);
	glVertex2f(wx2,wy2);
	glEnd();


	wx1 += (x2/vsize)*vsize*4/frac;
	wy1 += (y2/vsize)*vsize*4/frac;
	wx2 += (x2/vsize)*vsize*4/frac;
	wy2 += (y2/vsize)*vsize*4/frac;


	for(i=0;i<frac-(frac/2);i++)
	{

// wektor prostopadly do w
   	_wx1=wy1;
   	_wy1=-wx1;
   	_wx2=wy2;
   	_wy2=-wx2;

	vsize=sqrt((_wx2-_wx1)*(_wx2-_wx1)+(_wy2-_wy1)*(_wy2-_wy1));

	_wx1/=vsize;
	_wx2/=vsize;
	_wy1/=vsize;
	_wy2/=vsize;

	_wx2=_wx2-_wx1;
	_wy2=_wy2-_wy1;

//	_wx2/=2.f;
//	_wy2/=2.f;

	glBegin(GL_LINE_STRIP);
	glVertex2f(wx1-_wx2/frac,wy1-_wy2/frac);
	glVertex2f(wx1+_wx2/frac+(x2/vsize)*vsize/frac,wy1+_wy2/frac+(y2/vsize)*vsize/frac);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(wx1+_wx2/frac+(x2/vsize)*vsize/frac,wy1+_wy2/frac+(y2/vsize)*vsize/frac);
	glVertex2f(wx1+_wx2/frac+(x2/vsize)*vsize*1.5/frac,wy1+_wy2/frac+(y2/vsize)*vsize*1.5/frac);
	glEnd();


	vsize=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));


		wx1 += (x2/vsize)*vsize/frac;
		wy1 += (y2/vsize)*vsize/frac;
		wx2 += (x2/vsize)*vsize/frac;
		wy2 += (y2/vsize)*vsize/frac;


/*		glBegin(GL_LINE_STRIP);
 		glVertex2f(wx1,wy1);
 		glVertex2f(wx2,wy2);
		glEnd();
*/

	}


	glBegin(GL_LINE_STRIP);
	glVertex2f(wx1,wy1);
	glVertex2f(wx2+ (x2/vsize)*vsize*4/frac,wy2+ (y2/vsize)*vsize*4/frac);
	glEnd();


}



static void Draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);


 	glColor3f(1.0,1.0,1.0);
	glLineWidth(1);


    glPointSize(24);

    glBegin(GL_POINTS);
	glVertex2f(point[0], point[1]+MOVEY);
    glEnd();

/*    glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0 + MOVEY);
	glVertex2f(point[0], point[1]+MOVEY);
    glEnd();
*/

	glLineWidth(2);
	RysujSprezyne(0,0+MOVEY,point[0],point[1]+MOVEY);



	glLineWidth(3);
	glColor3f(0.6,0.6,0.5);
	RysujWektor(point[0],point[1]+MOVEY,pr*sin(teta+omega*dt),pr*cos(teta+omega*dt)+MOVEY,15.5f,0,0);

	glColor3f(0.6,0.7,0.8);
	RysujWektor(0,0+MOVEY,point[0],point[1]+MOVEY,3.5*(-k*(pr-pr0)),point[0],point[1]);



    glFlush();

    if (doubleBuffer) {
	glutSwapBuffers();
    }
}

static GLenum Args(int argc, char **argv)
{
    rgb = GL_TRUE;
    doubleBuffer = GL_TRUE;

    return GL_TRUE;
}



void idle(void)
{
	double Fs;
	

	t=t+dt;

// m=1

 //   printf("%f     %f       %f      %f\n",t,((omega*omega)*(r*r)/2.f)*10,(9.8 + g*point[1])*10,( (omega*omega)*(r*r)/2.f + 9.8 + g*point[1])*10);     


/*	if(point[0]<-1)
	{
		point[0]=-1;
		omega=-omega;
	}

	if(point[0]>1)
	{
		point[0]=1;
		omega=-omega;
	}
*/

	Fs=-k*(pr-pr0);

	omega = omega + (g+fabs(Fs)*cos(teta)/pr)*sin(teta)*dt;
 	teta  = teta + omega * dt;

	point[0] = pr*sin(teta);
    point[1] = pr*cos(teta);


 //	printf("%f\n",teta);



	// F=-k(x0-x)
	//

	pvr=pvr+Fs*dt;
	pr=pr+pvr*dt;


    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    if (Args(argc, argv) == GL_FALSE) {
	exit(1);
    }

    windW = 300;
    windH = 300;
    glutInitWindowPosition(0, 0); glutInitWindowSize( windW, windH);

    windType = (rgb) ? GLUT_RGB : GLUT_INDEX;
    windType |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(windType);

    if (glutCreateWindow("Wahad�o") == GL_FALSE) {
	exit(1);
    }

    //InitMap();

    Init();

    glutReshapeFunc(Reshape);
    glutDisplayFunc(Draw);
    glutIdleFunc(idle);
    glutMainLoop();
    
	return 0;
}
