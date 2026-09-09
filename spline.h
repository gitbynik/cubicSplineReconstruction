#ifndef SPLINE_H
#define SPLINE_H

typedef struct
{
	int size;
	int capacity;
	double* data;
} DoubleVector;

typedef struct
{
	int size;
	int capacity;
	struct Cubic* data;
} CubicVector;

typedef struct
{
	int num;
	DoubleVector x;
	DoubleVector y;
	DoubleVector h;
} DataPoints;

struct Cubic
{
	double a,b,c,d; //aX3 + bX2 + cX + d, X= x-xi
};

typedef struct Cubic Cubic;

typedef struct
{
	int size;
	double* upperDiag; //size-1 of these
	double* lowerDiag; //size-1 of these
	double* mainDiag; //size of these
	double* rhsx; //size of these
	double* rhsy; //size of these
} LinearSystem;

typedef struct
{
	DataPoints* dp;
	CubicVector CubicArrayX;
	CubicVector CubicArrayY;
} plotData;

void initDoubleVector(DoubleVector* v);
void pushDoubleVector(DoubleVector* v, double value);
void destroyDoubleVector(DoubleVector* v);

void initCubicVector(CubicVector* v);
void pushCubicVector(CubicVector* v, Cubic value);
void destroyCubicVector(CubicVector* v);

void createData(int num, DataPoints* dp);
void initData(DataPoints* dp);
void addDataPoint(DataPoints* dp, double x, double y);
void destroyData(DataPoints* dp);

void initLinearSystem(LinearSystem* ls,int size);
void fillLinearSystem(LinearSystem* ls, DataPoints* dp);
void destroyLinearSystem(LinearSystem* ls);
void thomas(LinearSystem* ls, double* x, double* y);
void constructCubics(LinearSystem* ls,DataPoints* dp, Cubic* cX, Cubic* cY);

void createPlotData(plotData* pd, int num);
void addPlotDataPoint(plotData* pd, double x, double y);
void destroyPlotData(plotData* pd);

#endif