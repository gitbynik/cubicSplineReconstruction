#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "spline.h"
#include "sign.h"

void initDoubleVector(DoubleVector* v)
{
	v->size = 0;
	v->capacity = 0;
	v->data = NULL;
}

void pushDoubleVector(DoubleVector* v, double value)
{
	if(v->size >= v->capacity)
	{
		int newCapacity = (v->capacity == 0) ? 4 : v->capacity * 2;
		double* newData = realloc(v->data,newCapacity * sizeof(double));

		if(newData == NULL)	return;
		v->data = newData;
		v->capacity = newCapacity;
	}
	v->data[v->size] = value;
	v->size++;
}

void destroyDoubleVector(DoubleVector* v)
{
	free(v->data);
	v->data = NULL;
	v->size = 0;
	v->capacity = 0;
}

void initCubicVector(CubicVector* v)
{
	v->size = 0;
	v->capacity = 0;
	v->data = NULL;
}

void pushCubicVector(CubicVector* v, Cubic value)
{
	if(v->size >= v->capacity)
	{
		int newCapacity = (v->capacity == 0) ? 4 : v->capacity * 2;
		Cubic* newData = realloc(v->data,newCapacity * sizeof(Cubic));

		if(newData == NULL) return;
		v->data = newData;
		v->capacity = newCapacity;
	}
	v->data[v->size] = value;
	v->size++;
}

void destroyCubicVector(CubicVector* v)
{
	free(v->data);
	v->data = NULL;
	v->size = 0;
	v->capacity = 0;
}

void destroyData(DataPoints* dp)
{
	destroyDoubleVector(&dp->h);
	destroyDoubleVector(&dp->y);
	destroyDoubleVector(&dp->x);
	dp->num = 0;
}

void initData(DataPoints* dp)
{
	for(int i=0;i<dp->num;i++) 
	{
		pushDoubleVector(&dp->x,-10.0+20.0*(i)/(dp->num-1));
		pushDoubleVector(&dp->y,4*(i%2)-2);
	}
	for(int i=0;i<dp->num-1;i++) 
	{
		double tempx = (dp->x.data[i+1] - dp->x.data[i]) * (dp->x.data[i+1] - dp->x.data[i]);
		double tempy = (dp->y.data[i+1] - dp->y.data[i]) * (dp->y.data[i+1] - dp->y.data[i]);
		pushDoubleVector(&dp->h,sqrt(tempx+tempy));
	}
}

void addDataPoint(DataPoints* dp, double x, double y)
{
	pushDoubleVector(&dp->x, x);
	pushDoubleVector(&dp->y, y);
	dp->num++;

	if(dp->num >= 2)
	{
		int i = dp->num - 1;
		double dx = dp->x.data[i] - dp->x.data[i-1];
		double dy = dp->y.data[i] - dp->y.data[i-1];
		pushDoubleVector(&dp->h,sqrt(dx*dx + dy*dy));
	}
}

void createData(int num, DataPoints* dp)
{
	dp->num = 0;

	initDoubleVector(&dp->x);
	initDoubleVector(&dp->y);
	initDoubleVector(&dp->h);
	
	for(int i=0;i<num;i++)
	{
		double x = -10.0+20.0*(i)/(num-1);
		double y = 4*(i%2)-2;
		addDataPoint(dp,x,y);
	}
}

void initLinearSystem(LinearSystem* ls,int size)
{
	ls->size = size;

	ls->lowerDiag = (double*)malloc((size-1)*sizeof(double));
	ls->upperDiag = (double*)malloc((size-1)*sizeof(double));
	ls->mainDiag = (double*)malloc(size*sizeof(double));
	ls->rhsx = (double*)malloc(size*sizeof(double));
	ls->rhsy = (double*)malloc(size*sizeof(double));
}

void fillLinearSystem(LinearSystem* ls, DataPoints* dp)
{
	ls->mainDiag[0] = ls->mainDiag[ls->size-1] = 1;
	ls->lowerDiag[ls->size-2] = ls->upperDiag[0] = 0;
	ls->rhsx[0] = ls->rhsx[ls->size-1] = 0;
	ls->rhsy[0] = ls->rhsy[ls->size-1] = 0;

	for(int i=1;i<ls->size-1;i++) //SIMD VECTOR???
	{
		ls->lowerDiag[i-1] = dp->h.data[i-1];
		ls->upperDiag[i] = dp->h.data[i];
		ls->mainDiag[i] = 2*(dp->h.data[i-1]+dp->h.data[i]);
		
		ls->rhsx[i] = 3*((dp->x.data[i+1] - dp->x.data[i])/dp->h.data[i] - (dp->x.data[i] - dp->x.data[i-1])/dp->h.data[i-1]);
		ls->rhsy[i] = 3*((dp->y.data[i+1] - dp->y.data[i])/dp->h.data[i] - (dp->y.data[i] - dp->y.data[i-1])/dp->h.data[i-1]);
	}
}

void destroyLinearSystem(LinearSystem* ls)
{
	free(ls->rhsx);
	free(ls->rhsy);
	free(ls->mainDiag);
	free(ls->upperDiag);
	free(ls->lowerDiag);
}

void thomas(LinearSystem* ls, double* x, double* y)
{
	for(int i=1;i<ls->size;i++)
	{
		double tempfactor = ls->lowerDiag[i-1]/ls->mainDiag[i-1];
		ls->mainDiag[i] -= tempfactor*ls->upperDiag[i-1];
		ls->rhsx[i] -= tempfactor*ls->rhsx[i-1];
		ls->rhsy[i] -= tempfactor*ls->rhsy[i-1];
	}

	x[ls->size-1] =	ls->rhsx[ls->size-1] / ls->mainDiag[ls->size-1];
	y[ls->size-1] = ls->rhsy[ls->size-1] / ls->mainDiag[ls->size-1];

	for(int i=ls->size-2;i>=0;i--)	
	{
		x[i] = (ls->rhsx[i] - ls->upperDiag[i]*x[i+1]) / ls->mainDiag[i];
		y[i] = (ls->rhsy[i] - ls->upperDiag[i]*y[i+1]) / ls->mainDiag[i];
	}
}

void constructCubics(LinearSystem* ls,DataPoints* dp,Cubic* cX,Cubic* cY)
{
	double* CoeffCX =(double*)malloc(ls->size*sizeof(double));
	double* CoeffCY =(double*)malloc(ls->size*sizeof(double));
	thomas(ls,CoeffCX,CoeffCY);

	for(int i=0;i<ls->size-1;i++) //SIMD VECTOR???
	{
		cX[i].d = dp->x.data[i];
		cX[i].b = CoeffCX[i];
		cX[i].c = (dp->x.data[i+1]-dp->x.data[i])/dp->h.data[i] - (2*CoeffCX[i]+CoeffCX[i+1])*dp->h.data[i]/3.0;
		cX[i].a = (CoeffCX[i+1]-CoeffCX[i])/(3*dp->h.data[i]);

		cY[i].d = dp->y.data[i];
		cY[i].b = CoeffCY[i];
		cY[i].c = (dp->y.data[i+1]-dp->y.data[i])/dp->h.data[i]	- (2*CoeffCY[i]+CoeffCY[i+1])*dp->h.data[i]/3.0;
		cY[i].a = (CoeffCY[i+1]-CoeffCY[i])/(3*dp->h.data[i]);
	}

	free(CoeffCY);
	free(CoeffCX);
}

void createPlotData(plotData* pd, int num)
{
	pd->dp = (DataPoints*)malloc(sizeof(DataPoints));
	createData(num, pd->dp);
	initCubicVector(&pd->CubicArrayX);
	initCubicVector(&pd->CubicArrayY);

	for(int i=0;i<pd->dp->num-1;i++)
	{
		Cubic c1 = {0};
		pushCubicVector(&pd->CubicArrayX, c1);
		pushCubicVector(&pd->CubicArrayY, c1);
	}

	LinearSystem ls;
	initLinearSystem(&ls,pd->dp->num);
	fillLinearSystem(&ls,pd->dp);
	constructCubics(&ls,pd->dp,pd->CubicArrayX.data,pd->CubicArrayY.data);
	destroyLinearSystem(&ls);
}

void addPlotDataPoint(plotData* pd, double x, double y)
{
	addDataPoint(pd->dp,x,y);
	Cubic newcubic = {0};
	pushCubicVector(&pd->CubicArrayX, newcubic);
	pushCubicVector(&pd->CubicArrayY, newcubic);

	LinearSystem ls;
	initLinearSystem(&ls,pd->dp->num);
	fillLinearSystem(&ls,pd->dp);
	constructCubics(&ls,pd->dp,pd->CubicArrayX.data,pd->CubicArrayY.data);
	destroyLinearSystem(&ls);
}

void destroyPlotData(plotData *pd)
{
	destroyDoubleVector(&pd->dp->h);
	destroyDoubleVector(&pd->dp->x);
	destroyDoubleVector(&pd->dp->y);
	destroyCubicVector(&pd->CubicArrayX);
	destroyCubicVector(&pd->CubicArrayY);
}