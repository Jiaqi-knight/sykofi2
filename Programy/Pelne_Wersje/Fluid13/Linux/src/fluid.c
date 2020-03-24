
   /*____________________________________________________________________
   """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
	 The MAC method for incompresible, viscous fluid flow.
	   
	   Main file: fluid.c



		          Maciej Matyka 
	      mailto:maq@panoramix.ift.uni.wroc.pl

		   Uniwersity of Wroclaw in Poland
             Wydzial Fizyki i Astronomii
   
   ______________________________________________________________________
   """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""*/




/*
 changes:

 define DTMAX number, bufor for every datas copy before timestep,
 dt increment doubled every timestep, halved when
 D[i][j] test fail, then copy old datas and run timestep with dt/2.

*/

/*

	h i L calculate maximum of xm and ym particle positions from setup


	Region #55 - done in ReflagCells in tscells.c

	skipped:
					 1. create and destroy particles (need for B_IN cells)

					 2.
							void	CalculateBNDConditions(int **F, double **U, double **V, int nx, int ny);
							in bndinit.c

           3. #55 For each SUR cell, calculate the velocity for the side next to an EMP
F              cell (equation).





@todo:     !!!  reading type of boundary in parse.c

@todo:     U*V corners ????????????


*/

/*
#include <proto/dos.h>
#include <proto/exec.h>
#include <exec/exec.h>
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bndflags.h"
#include "parse.h"
#include "dimalloc.h"
#include "bndinit.h"
#include "view.h"
#include "particle.h"
#include "visual.h"
#include "tscells.h"
#include "pressure.h"
#include "velocity.h"
#include "minmax.h"

#define DTSTART 1.0/8
#define DTMAX DTSTART

/*struct DosBase *DosBase;
*/

struct particle *particle1;
struct particle *_particle1;

int timestep(double dx, double dy, double gx, double gy, double vis,
	     double t, double dt, int nx, int ny, int imax, int jmax,
	     int width, int height, int **Flag, double **Un1, double **Vn1,
	     double **U, double **V, double **P, double **D, double **R, double sta);
void CopyTable(double **T1, double **T2, int nx, int ny);
void CopyTableI(int **T1, int **T2, int nx, int ny);
void CopyParticles(struct particle *par1, struct particle *par2);
 
int teststability(double **U, double **V, int nx, int ny, double dx,
		  double dy, double dt, double vis, double sta);



//#define fontanna




int main(int argc, char *argv[])
{
//    char blah[100];

    int i, j;

    double dx, dy;
    double gx, gy;
    double vis;
    double savet;
    double t,dt,dtmax,dtstart;
    int nx, ny, imax, jmax, width, height, BNum;


    int frame;
    int stability;

    int **Flag, **_Flag;
    double **Un1, **Vn1, **U, **V, **P, **D, **R;

    double **_Un1, **_Vn1, **_U, **_V, **_P, **_D, **_R;

    int *BPoints, *BCells;
    double sta;
    int gmode;

/*
	DosBase=(struct DosBase*) OpenLibrary("dos.library",37);
*/


	ParseFile(argc,argv,&nx,&ny,&width,&height,&gx,&gy,&dt,&vis,&sta,&gmode,&BNum,&BCells,&BPoints);


	dtstart=dtmax=dt;

    imax = nx + 2;
    jmax = ny + 2;
    dx = width / nx;
    dy = height / ny;


    printf("tables...\n");

    if
	(
	 !(Flag = Alloc2TableI(imax, jmax)) |
	 !(Un1 = Alloc2TableD(imax, jmax)) |
!(Vn1 = Alloc2TableD(imax, jmax)) |
	 !(U = Alloc2TableD(imax, jmax)) |
	 !(V = Alloc2TableD(imax, jmax)) |
	 !(P = Alloc2TableD(imax, jmax)) |
	 !(D = Alloc2TableD(imax, jmax)) |
	 !(R = Alloc2TableD(imax, jmax)) |
	 !(_Flag = Alloc2TableI(imax, jmax)) |
	 !(_Un1 = Alloc2TableD(imax, jmax)) |
	 !(_Vn1 = Alloc2TableD(imax, jmax)) |
	 !(_U = Alloc2TableD(imax, jmax)) |
	 !(_V = Alloc2TableD(imax, jmax)) |
	 !(_P = Alloc2TableD(imax, jmax)) |
	 !(_D = Alloc2TableD(imax, jmax)) | !(_R = Alloc2TableD(imax, jmax))) {	/* then ... */
	printf("No memory to alloc tables.\n");
	exit(1);
    }


    printf("...done\n");

    printf("\n");		/* for progress */


    BundaryCellsToPoints(BCells, BPoints, BNum, width, height, dx, dy);
    CellSetup15(Flag, BPoints, BNum, width, height, nx, ny, dx, dy);






#ifdef fontanna

    
	for(i=3;i<nx-2;i++)
{
	Flag[i][ny-1] &= ~C_EMPBND;
	Flag[i][ny-1] &= ~C_BND;
	Flag[i][ny-1] |= C_OB;
	Flag[i][ny-1] |= C_URON;
	Flag[i][ny] |= C_BND;

}

	
	
    Flag[17][10] &= ~C_BND;
    Flag[18][10] &= ~C_BND;
	Flag[15][10] &= ~C_BND;
	Flag[16][10] &= ~C_BND;
	Flag[15][10] |= C_OB;
	Flag[18][10] |= C_OB;
	
	Flag[15][10] &= ~C_URON;
	Flag[16][10] &= ~C_URON;
	Flag[17][10] &= ~C_URON;
	Flag[18][10] &= ~C_URON;
	Flag[15][9] &= ~C_URON;
	Flag[16][9] &= ~C_URON;
	Flag[17][9] &= ~C_URON;
 	Flag[18][9] &= ~C_URON;
	

	Flag[32][10] &= ~C_URON;
 	Flag[32][11] &= ~C_URON;
	Flag[31][10] &= ~C_OB;
	Flag[32][10] &= ~C_OB;
	Flag[32][11] &= ~C_OB;
	Flag[32][11] &= ~C_COR;


#endif



	particle1=ParticleFile(argc,argv,dx,dy,nx,ny,width,height,Flag,U,V,1);
	_particle1=ParticleFile(argc,argv,dx,dy,nx,ny,width,height,Flag,U,V,1);


		for(j=1;j<=ny;j++)
                  {
		for(i=1;i<=nx;i++)
		{
			if(!( (Flag[i][j] & C_BND) || (Flag[i][j] & C_EMPBND) ))
					ReflagTo(Flag,i,j,C_EMP);
		}
		}

    /*particle1 = ParticleSetup16(dx, dy, nx, ny, width, height, Flag, U, V);
    _particle1 = ParticleSetup16(dx, dy, nx, ny, width, height, Flag, U, V);
      */

/*
    printf("...particles\n");
*/
/*	particle1=ParticleFile(argc,argv,dx,dy,nx,ny,width,height,Flag,U,V,1);
	_particle1=ParticleFile(argc,argv,dx,dy,nx,ny,width,height,Flag,U,V,1);
*/
/*
    printf("...done\n");
*/	


    CalculateBNDConditions(Un1, Vn1, Flag, U, P, V, D, nx, ny, gx, gy, vis,
			   dx, dy);

  V_OpenView(width,height);
 /*	V_ClearRect(0,0,width-1,height-1);
	DrawMesh(width,height,nx,ny,dx,dy);
	DrawBndCells(width,height,nx,ny,dx,dy,Flag);
	PlotParticles(particle1,width,height,P,nx,ny);
	V_Write(0);
*/




/*	for(j=1;j<=ny;j++)*/
/*  {*/
/*		for(i=1;i<=nx;i++)*/
/*		{*/
/*			U[i][j] =  0;*/
/*			V[i][j] =  0;*/
/*    }*/
/*	}*/



    ShowBoundary(Flag, nx, ny);

    t = 0.0;
    frame = 0;
    savet=0;


    /* Time step */
    for (j = 0; j < 10000; j++) {
	frame++;
	t += dtmax;

/*
    printf("write iff image.\n");


		if(frame<10)
			sprintf(blah,"rx grab.pprx dane:drop/frame.000%d",frame);
		else
			if(frame<100)
				sprintf(blah,"rx grab.pprx dane:drop/frame.00%d",frame);
	    else
			if(frame<1000)
				sprintf(blah,"rx grab.pprx dane:drop/frame.0%d",frame);
			else
				sprintf(blah,"rx grab.pprx dane:drop/frame.%d",frame);
		Execute(blah,0,0);
*/


/*    printf("gx=%f,gy=%f\n",gx,gy);*/



/*	printf("dt timeloop.\n");
*/

/*			CopyTable(New,Old);*/

	do {

/*				printf("frame %d.",frame);
	    printf("frame=%d, t=%f, dt=%f\n", frame, t, dt);
*/
	    for (i = 0; i < ((int) ((dtmax / dt))); i++) {

		printf("i=%d(%d)\n",i,(int)(dtmax/dt)-1);

		stability=0;

		stability +=
		    timestep(dx, dy, gx, gy, vis, t, dt, nx, ny, imax,
			     jmax, width, height, Flag, Un1, Vn1, U, V, P,
			     D, R, sta);


		stability += teststability(U, V, nx, ny, dx, dy, dt, vis, sta);

#define breakloop_sta

#ifdef breakloop_sta

		if (stability != 0) 
		{
/*		    printf("repeat timestep.\n");
*/
/*					CopyTable(Old,New);*/



			
			if (frame > 1) 
		    {


			CopyParticles(_particle1,particle1);

			CopyTableI(_Flag, Flag, nx, ny);
			CopyTable(_U, U, nx, ny);
			CopyTable(_V, V, nx, ny);
			CopyTable(_Vn1, Vn1, nx, ny);
			CopyTable(_Un1, Un1, nx, ny);
			CopyTable(_P, P, nx, ny);
			CopyTable(_D, D, nx, ny);
			CopyTable(_R, R, nx, ny);

		    }

		    dt /= 2.0;
		    break;
		}
		/*      if (V_CheckExitTrue())
		   break; */
	    

#else
		if (stability != 0) 
		    dt /= 2.0;
#endif		
		}

	} while (stability != 0);



	dt *= 2.0;
	if (dt > dtmax)
	    dt = dtmax;


	printf("[%d] \n ", frame);

	if (frame > 1) 

	CopyParticles(particle1,_particle1);

/*	printf("po partiklach\n");
*/
	CopyTableI(Flag, _Flag, nx, ny);
	CopyTable(U, _U, nx, ny);
	CopyTable(V, _V, nx, ny);
	CopyTable(Vn1, _Vn1, nx, ny);
	CopyTable(Un1, _Un1, nx, ny);
	CopyTable(P, _P, nx, ny);
	CopyTable(D, _D, nx, ny);
	CopyTable(R, _R, nx, ny);

/*			dt*=2.0;*/

/*			t+=fi;*/

/*			if(resdij > 0)*/
/*				dt/=2;*/
/*			else*/
/*				{*/
/*  			}*/

/*
		if(V_CheckExitTrue())
			break;
*/
	 	V_ClearRect(0,0,width-1,height-1);



		DrawMesh(width,height,nx,ny,dx,dy);


/* Graphics mode

0 - particles with pressure field color
1 - particles one color set
2 - velocity vectors
3 - particles & vectors

*/


		if(gmode==4)
			PlotParticlesColor(particle1,width,height,nx,ny,95,95,255);

		if(gmode==3)
		{
			PlotParticles(particle1,width,height,P,nx,ny);
			PlotVelocities(width,height,nx,ny,dx,dy,U,V,Flag);
		}
		if(gmode==1)
{

/*    for (j = 1; j <= ny; j++)
	for (i = 1; i <= nx; i++) 
	    Vn1[i][j] = 200.0;

    Vn1[1][1] = 0.0;
*/
		PlotParticles(particle1,width,height,P,nx,ny);
}
		if(gmode==0)
		PlotParticles(particle1,width,height,P,nx,ny);

		if(gmode == 2)
		PlotVelocities(width,height,nx,ny,dx,dy,U,V,Flag);

		DrawBndCells(width,height,nx,ny,dx,dy,Flag);

/*		savet+=dtmax;
*/
/*
		if(savet > dtmax*9)
		{

			printf("\nWRITE %d.\n",frame);
			V_Write(frame);
			savet=0;

		}
*/
		


		printf("write frame %d to disk...\n",frame);
		V_Write(frame);
		printf("...done\n");

/*
		if(V_CheckExitTrue())
			break;
*/

    }  while (1);


/*		j=2;*/
/*		for(i=0;i<nx;i++)*/
/*		{*/
/*				printf("P(%d,%d) = %f\n",i,j,P[i][j]);*/
/*				printf("-P(%d,%d) = %f\n",i,j,-P[i][j]);*/
/*				printf("(-1.0)*P(%d,%d) = %f\n",i,j,(-1.0)*P[i][j]);*/
     /**/
/*    }*/
/*	while(!V_CheckExitTrue());
*/

/*	V_CloseView();
*/

/*  printf("\n");*/
/*	for(j=1;j<=ny;j++)*/
/*  {*/
/*		for(i=1;i<=nx;i++)*/
/*		{*/
/*			printf("U[%d][%d] = %f    ",i,j,U[i][j]);*/
/*			printf("V[%d][%d] = %f\n",i,j,V[i][j]);*/
/*  			printf("R[%d][%d] = %f\n",i,j,R[i][j]);*/
/*    }*/
/*	}*/
/*	ShowBoundary(Flag,nx,ny);*/



    FreeAllParticles(particle1);
    FreeAllParticles(_particle1);

    Free2TableI(_Flag, imax, jmax);
    Free2TableD(_R, imax, jmax);
    Free2TableD(_D, imax, jmax);
    Free2TableD(_P, imax, jmax);
    Free2TableD(_U, imax, jmax);
    Free2TableD(_V, imax, jmax);
    Free2TableD(_Un1, imax, jmax);
    Free2TableD(_Vn1, imax, jmax);

    Free2TableD(R, imax, jmax);
    Free2TableD(D, imax, jmax);
    Free2TableD(P, imax, jmax);
    Free2TableD(U, imax, jmax);
    Free2TableD(V, imax, jmax);
    Free2TableD(Un1, imax, jmax);
    Free2TableD(Vn1, imax, jmax);
    Free2TableI(Flag, imax, jmax);
    Free1TableI(BPoints);
    Free1TableI(BCells);

    return (0);
}


int teststability(double **U, double **V, int nx, int ny, double dx,
		  double dy, double dt, double vis, double sta)
{
    double UMAX, VMAX, UREADED, VREADED;
    int i, j;
    double A, d;

    UMAX = 0.0;
    VMAX = 0.0;

    for (j = 1; j <= ny; j++)
	for (i = 1; i <= nx; i++) {
	    UREADED = fabs(U[i][j]);
	    VREADED = fabs(V[i][j]);

	    UMAX = max(UREADED, UMAX);
	    VMAX = max(VREADED, VMAX);
	}

    A = max(UMAX, VMAX);
    d = max(dx, dy);


    if (((A * dt) / d) > sta) {
/*	printf("vmax=%f, dmax=%f, dt=%f, ", A, d, dt);
	printf(" vmax*dt/d=%f\n", (A * dt) / d);
	printf
	    (" (1)                             stability test fail !\n");
*/
	return (1);
    }



/* second stability condition */

/*
    UMAX = 0;
    VMAX = 0;

    for (j = 1; j <= ny; j++)
	for (i = 1; i <= nx; i++) {
	    UREADED = fabs(U[i][j] - U[i - 1][j]);
	    VREADED = fabs(V[i][j] - V[i][j - 1]);

	    UMAX = max(UREADED, UMAX);
	    VMAX = max(VREADED, VMAX);
	}

    A = max(UMAX, VMAX);
    d = max(1/dx, 1/dy);


    if (dt > ((2 * vis ) / (A * d))) {
	printf("dvmax=%f, dt=%f, right[(2*vis)/(A*d)]=%f\n", A, dt,
	       (2 * vis) / (A * d));
	printf
	    (" (2)                             stability test fail !\n");
	return (1);
    }
*/

    return (0);

}

void CopyParticles(struct particle *par1, struct particle *par2)
{
	struct particle *wrkpart1;
	struct particle *wrkpart2;

	wrkpart2 = par2;
	
	
        for (wrkpart1 = par1; wrkpart1 != NULL; wrkpart1 = wrkpart1->next)
        {
         	
         	wrkpart2->x = wrkpart1->x;
         	wrkpart2->y = wrkpart1->y;
         	wrkpart2->uk = wrkpart1->uk;
         	wrkpart2->vk = wrkpart1->vk;
         	wrkpart2->flag = wrkpart1->flag;
         	wrkpart2->i = wrkpart1->i;
         	wrkpart2->j = wrkpart1->j;

             	wrkpart2 = wrkpart2->next;
        }
	


}


void CopyTable(double **T1, double **T2, int nx, int ny)
{
    int i, j;

    for (j = 1; j <= ny; j++)
	for (i = 1; i <= nx; i++)
	    T2[i][j] = T1[i][j];
}


void CopyTableI(int **T1, int **T2, int nx, int ny)
{
    int i, j;

    for (j = 1; j <= ny; j++)
	for (i = 1; i <= nx; i++)
	    T2[i][j] = T1[i][j];
}



int timestep(double dx, double dy, double gx, double gy, double vis,
	     double t, double dt, int nx, int ny, int imax, int jmax,
	     int width, int height, int **Flag, double **Un1, double **Vn1,
	     double **U, double **V, double **P, double **D, double **R, double sta)
{
//    int i,j;






    ReflagCells(Vn1, Flag, P, U, V, nx, ny, particle1);

/*		if (V_CheckExitTrue())
			return(0);
*/

    CalculateDij(Flag, D, U, V, dx, dy, nx, ny);


/*
		if(V_CheckExitTrue())
			return(0);
*/
    PressureSource(Flag, U, V, D, R, nx, ny, dx, dy, vis, dt);

/*
		if (V_CheckExitTrue())
			return(0);

		if (V_CheckExitTrue())
			return(0);

*/

    /* fuckin NaN */




    if (CalculatePressure(Flag, R, P, nx, ny, dx, dy, gx, gy, vis, U, V) !=
	0) return (1);


    CalculateVelocity(Un1, Vn1, Flag, R, P, nx, ny, dx, dy, gx, gy, vis, U,
		      V, dt);

/*
		if (V_CheckExitTrue())
			return(0);
*/


    CalculateSurface(Un1, Vn1, Flag, P, D, U, V, nx, ny, gx, gy, dt, dx,
		     dy);


/*
    for (j = 1; j <= ny; j++)
	for (i = 1; i <= nx; i++)
	{
	    if(Flag[i][j] & C_EMP)
	    {

		if(P[i][j] != 0.0)
		printf("P[%d][%d] = %f\n",i,j,P[i][j]);
	    
		P[i][j] = 0.0; 
	    }

	}
*/


/*
		if (V_CheckExitTrue())
			return(0);
*/
    CalculateBNDConditions(Un1, Vn1, Flag, U, P, V, D, nx, ny, gx, gy, vis,
			   dx, dy);

/*
		if (V_CheckExitTrue())
			return(0);
*/
    if (teststability(U, V, nx, ny, dx, dy, dt, vis, sta) != 0)
	return (1);


/*    printf("movement \n");
*/
    ParticleMovement(particle1, dx, dy, nx, ny, width, height, Flag, U, V,
		     dt);

/*
		if (V_CheckExitTrue())
			return(0);
*/
    return (0);
}
