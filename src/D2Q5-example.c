/*Index Legend for D2Q5
*
*    y
*    4
*    |
* 1--0--2x
*    |
*    3
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mygraph.h>

//Maximum Simulation Size
#define XMAX (128)
#define YMAX (128)
//Can make simulation size smaller than maximums during runtime, but not larger
//Dynamically allocate arrays with malloc to avoid this limitation
int xdim = XMAX;
int ydim = YMAX;

//Simulation Parameters:
//these change the physics of the simulation
double a = -1.0, b = 1.0, kappa = 1.0;
double tau = 1.0;
double phi_init = 0.0;
double noise_amplitude = 1.0e-6;

//distributions
double f0[XMAX][YMAX];
double f1[XMAX][YMAX];
double f2[XMAX][YMAX];
double f3[XMAX][YMAX];
double f4[XMAX][YMAX];

//physical quantities
double phi[XMAX][YMAX];
double mu[XMAX][YMAX];
double current[XMAX][YMAX];

//Scaling Parameter for Time:
//this effects the stability of the simulation
//higher is faster but less stable
double s = 0.050;
// for kappa = 2.0, use s = 0.027;

//simulation controls
int iteration = 0; //current iteration count
int batch_iterations = 100; //iterations between calls to the GUI
int gui = 1;
int paused = 1;
int done = 0;
int request_current=0;
int rand_seed = 0;


void Initialize(void);
void DefineGUI(void);
void IterateOnce(void);
void IterateBatch(void);
void Collision(void);
void Streaming(void);
void SanityCheck(void);
void GetGraphics(void);

int main (void) {
	int newdata = 0;
	
	Initialize();

	if (gui) DefineGUI();

	while (!done)
	{
		if (gui)
		{
			Events(newdata);
			SanityCheck();
			GetGraphics();
			DrawGraphs();
		} else if (paused) {
			done = 1;
		}

		if (!paused)
		{
			newdata = 1;
			IterateBatch();
		} else sleep(1);
	}
	return 0;
}



void Initialize(void)
{
	iteration = 0;

	if (rand_seed) srand(rand_seed);

	for (int i=0;i<xdim;i++)
	{
		for (int j=0;j<ydim;j++)
		{
			phi[i][j] = phi_init + noise_amplitude*(2.0*rand()/RAND_MAX - 1.0);
		}
	}
	
	for (int i=0;i<xdim;i++)
	{
		int im = (i+xdim-1)%xdim; //periodic boundary
		int ip = (i+1)%xdim; //periodic boundary
		for (int j=0;j<ydim;j++)
		{
			int jm = (j+ydim-1)%ydim; //periodic boundary
			int jp = (j+1)%ydim; //periodic boundary
			double p = phi[i][j];
			
			mu[i][j] = a*p + b*p*p*p - kappa*(phi[im][j] + phi[i][jm] - 4.0*p + phi[i][jp] + phi[ip][j]);
			current[i][j] = 0.0;
			
			f0[i][j] = p - 2.0*s*mu[i][j];
			f1[i][j] = f2[i][j] = f3[i][j] = f4[i][j] = 0.5*s*mu[i][j];
		}
	}
}


void DefineGUI(void)
{
	DefineGraphNxN_R("f0", &f0[0][0], &xdim, &ydim, NULL);
	DefineGraphNxN_R("f1", &f1[0][0], &xdim, &ydim, NULL);
	DefineGraphNxN_R("f2", &f2[0][0], &xdim, &ydim, NULL);
	DefineGraphNxN_R("f3", &f3[0][0], &xdim, &ydim, NULL);
	DefineGraphNxN_R("f4", &f4[0][0], &xdim, &ydim, NULL);
	NewGraph();

	DefineGraphNxN_R("phi", &phi[0][0], &xdim, &ydim, NULL);
	DefineGraphNxN_R("mu", &mu[0][0], &xdim, &ydim, NULL);
	DefineGraphNxN_R("current", &current[0][0], &xdim, &ydim, &request_current);
	NewGraph();
	
	StartMenu("D2Q5 Example", 1);
	
		StartMenu("Inputs", 0);
			DefineInt("xdim", &xdim);
			DefineInt("ydim", &ydim);
			DefineDouble("a", &a);
			DefineDouble("b", &b);
			DefineDouble("kappa", &kappa);
			DefineDouble("tau", &tau);
			DefineDouble("phi_init", &phi_init);
			DefineDouble("noise_amplitude", &noise_amplitude);
			DefineDouble("s", &s);
			DefineInt("rand_seed", &rand_seed);
		EndMenu();

	DefineFunction("Initialize",&Initialize);
	
	DefineInt("iteration", &iteration);
	DefineInt("batch_iterations", &batch_iterations);
	
	SetActiveGraph(0);
	DefineGraph(contour2d_,"Distribution Plots");
	SetActiveGraph(1);
	DefineGraph(contour2d_,"Physical Plots");
	
	DefineFunction("IterateOnce",&IterateOnce);
	DefineFunction("IterateBatch",&IterateBatch);
	DefineBool("paused", &paused);
	DefineBool("done",&done);	
	EndMenu();
}


void IterateBatch(void)
{
	for (int n = 0;n<batch_iterations;n++)
	{
		IterateOnce();
	}
}


void IterateOnce(void){
	Collision();
	Streaming();
	iteration++;
}


void Collision(void) {
	double omega = 1.0/tau;

	for (int i=0;i<xdim;i++)
	{
		for (int j=0;j<ydim;j++)
		{
			phi[i][j] = f0[i][j] + f1[i][j] + f2[i][j] + f3[i][j] + f4[i][j];
		}
	}

	for (int i=0;i<xdim;i++)
	{
		int im = (i+xdim-1)%xdim; //periodic boundary
		int ip = (i+1)%xdim; //periodic boundary
		for (int j=0;j<ydim;j++)
		{
			int jm = (j+ydim-1)%ydim; //periodic boundary
			int jp = (j+1)%ydim; //periodic boundary
			double p = phi[i][j];
			
			mu[i][j] = a*p + b*p*p*p - kappa*(phi[im][j] + phi[i][jm] - 4.0*p + phi[i][jp] + phi[ip][j]);
			
			f0[i][j] += omega*(p - 2.0*s*mu[i][j] - f0[i][j]);
			f1[i][j] += omega*(0.5*s*mu[i][j] - f1[i][j]);
			f2[i][j] += omega*(0.5*s*mu[i][j] - f2[i][j]);
			f3[i][j] += omega*(0.5*s*mu[i][j] - f3[i][j]);
			f4[i][j] += omega*(0.5*s*mu[i][j] - f4[i][j]);
		}
	}
}


void Streaming(void)
{
	double f1_out[YMAX];
	double f2_out[YMAX];
	double f3_out[XMAX];
	double f4_out[XMAX];
	
	//save outflowing edges
	//left and right can be done with a memmove as the memory is contiguous
	memmove(&f1_out[0], &f1[0][0], ydim*sizeof(double));
	memmove(&f2_out[0], &f2[xdim-1][0], ydim*sizeof(double));
	//top and bottom must be done cell by cell
	for (int i=0;i<xdim;i++)
	{
		f3_out[i] = f3[i][0];
		f4_out[i] = f4[i][ydim-1];
	}
	
	//use memmove for bulk
	memmove(&f1[0][0], &f1[1][0], (xdim-1)*ydim*sizeof(double));
	memmove(&f2[1][0], &f2[0][0], (xdim-1)*ydim*sizeof(double));
	memmove(&f3[0][0], &f3[0][1], (ydim*xdim-1)*sizeof(double));
	memmove(&f4[0][1], &f4[0][0], (ydim*xdim-1)*sizeof(double));
	
	//bring in from periodic boundary
	memmove(&f1[xdim-1][0], &f1_out[0], ydim*sizeof(double));
	memmove(&f2[0][0], &f2_out[0], ydim*sizeof(double));
	for (int i=0;i<xdim;i++)
	{
		f3[i][ydim-1] = f3_out[i];
		f4[i][0] = f4_out[i];
	}
}


void SanityCheck(void)
{
	if (xdim>XMAX) xdim = XMAX;
	if (ydim>YMAX) ydim = YMAX;
}


void GetGraphics(void){

	if (request_current)
	{
		request_current = 0;
		for (int i=0;i<xdim;i++)
		{
			for (int j=0;j<ydim;j++)
			{
				current[i][j] = 0.0;  //Fill this in with the proper code to compute the current vec{j}=m \nabla \mu
                //current[i][j] = (tau - .5)* //grad(mu[i][j]) = grad(a*p + b*p*p*p
            }
		}
	}	
}

