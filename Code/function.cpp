#include "function.h"
#include <iostream>

void freeMemory(functionData_t* FData){
    int t;

    double **voltage = FData->voltage;
    double **current = FData->current;

    for (t = 0; voltage[t] != nullptr; t++)
    {
        delete[] voltage[t];
        delete[] current[t];
    }

    delete[] voltage;
    delete[] current;

    delete FData;

    std::cout << "ALL CLEAR\n";
}

double continueFunction(int nt, double limit){
    return 2;
}

double stepFunction(int nt, double limit){
    // return (nt >= limit) ? 0 : 1;
    return exp(-(4.0*log(2.0*nt/50.0)*log(2.0*nt/50.0))/1.2);
}

functionData_t* allocMemory(int vol, int res, double dt, double nt, double dx, double nz){

    int K = nz/dx;
    int N = nt/dt;

    int t;

    double **voltage;
    double **current;

    functionData_t* FData = new functionData_t;

    voltage = new double *[N + 1];
    current = new double *[N + 1];

    std::cout << "iniciando alocacao" << std::endl;
    for (t = 0; t < N + 1; t++)
    {
        voltage[t] = new double[K + 1];
        current[t] = new double[K + 1];

        if (voltage[t] == nullptr || current[t] == nullptr)
        {
            std::cout << "sem memoria" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    FData->voltage = voltage;
    FData->current = current;

    return calculateAllValues(FData, vol, res, dt, nt, dx, nz);

}


functionData_t* calculateAllValues(functionData_t* FData, int vol, int res, double dt, double nt, double dx, double nz){
    int Nz = nz/dx;
    int Nt = nt/dt;

    std::cout << "K: "<< Nz << std::endl;
    std::cout << "N: "<< Nt << std::endl;

    int t;
    int z;
    // float S = 1;
    // float S = 0.5;
    float S = 1.0005;

    double **v = FData->voltage;
    double **i = FData->current;

    double minVoltage = 0;
    double maxVoltage = 0;

    double minCurrent = 0;
    double maxCurrent = 2;


    std::cout << "S: " << S << std::endl;
    std::cout << "iniciando calculo" << std::endl;

    for (t = 0; t < Nt + 1; t++)
    {
        for (z = 0; z < Nz + 1; z++){
            i[t][z] = 0;
            v[t][z] = 0;
        }
    }
    
    for (t = 0; t < Nt + 1; t++){
        v[t][0] = stepFunction(t, 40);
    }
    
    //u_n+1_i = u_n_i+1 + u_n_i-1 - u_n-1_i
    for (t = 1; t < Nt - 1; t++)
    {
        // v[t+1][1] = v[t][2] + v[t][0] - v[t-1][1];
        v[t+1][1] = S*S*(v[t][2] -2*v[t][1] + v[t][0]) + 2*v[t][1] - v[t-1][1];
        for (z = 1; z < Nz - 1; z++)
        {
            // if(z > 3*Nz/4.0)
            //     S = 0.25;
            // else
            //     S = 1;
            
            // v[t+1][z] = v[t][z+1] + v[t][z-1] - v[t-1][z];
            v[t+1][z] = S*S*(v[t][z+1] -2*v[t][z] + v[t][z-1]) + 2*v[t][z] - v[t-1][z];
    
            if(v[t + 1][z] > maxVoltage)
                maxVoltage = v[t + 1][z];
            else if(v[t + 1][z] < minVoltage)
                minVoltage = v[t + 1][z];
        }        
    }
    
    FData->minVoltage = minVoltage;
    FData->maxVoltage = maxVoltage;

    FData->minCurrent = minCurrent;
    FData->maxCurrent = maxCurrent;    

    std::cout << "calculo terminado" << std::endl;

    return FData;
}

double getVoltage(functionData_t* FData, double t, double z, double dt, double dx){

    int nt = t/dt;
    int nz = z/dx;
    
    return FData->voltage[nt][nz];
}

double getCurrent(functionData_t* FData, double t, double z, double dt, double dx){
    
    int nt = t/dt;
    int nz = z/dx;
    
    return 0;//FData->current[nt][nz];
}
