#pragma once
#include <cmath>
#include <bits/stdc++.h> 
//v(t) = 2u(t)
//v(t) = u(t) - u(t-l*/(10*uf)
//uf -> 0.9c

//#define l 100        //meters
#define MAX_TIME 10    //seconds
//#define _c 300000000     //light speed

//sem perdas
#define _R 0 //resistance
#define _G 0 //conductance

#define Z0 50
#define Rs 75

#define DEGRAU 0
#define CONTINUA 1

#define ZERO 0
#define CEM 1
#define INFINITA 2

const double C = 7.40741 * powf64x(10, -11);
const double L = 1.85185 * powf64x(10, -7);

const double Vph = 1/sqrt(C*L);

typedef double (*voltageFunction)(int t, double limit);

typedef struct functionData_t{

    double **voltage;
    double **current;

    double minCurrent;
    double maxCurrent;

    double minVoltage;
    double maxVoltage;

}functionData_t;

double stepFunction(int nt);
double pulseFunction(int nt);

void freeMemory(functionData_t* functionData);
functionData_t* allocMemory(int vol, int res, double dt, double nt, double dz, double nz);
double initialFunction(double t);
functionData_t* calculateAllValues(functionData_t* functionData, int vol, int res, double dt, double nt, double dz, double nz);

double getCurrent(functionData_t* functionData, double t, double z, double dt, double dz);
double getVoltage(functionData_t* functionData, double t, double z, double dt, double dz);
