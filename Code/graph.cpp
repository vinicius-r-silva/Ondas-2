
//g++ mycode.cpp -o main `pkg-config --cflags --libs opencv`

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <time.h>     
#include <stdlib.h>  
#include <math.h>

#include "function.h"
#include "graph.h"

// #define _Z0 0
// #define _ZF 100

#define _X_LABEL_SIZE 30
#define _Y_LABEL_SIZE 120
#define _X_LABEL_DIST 100
#define _Y_LABEL_DIST 40

#define QTD_PER_PX 10

#define SCALE 3

double Graph::TFixed_vo(double z){
    // if(z >= max_z || z < 0)
    //      return 0.0;

    return getVoltage(fdata, fixedT, z, dt, dz);
    // return (fixedT - 2)*cos(0.39*z) + (fixedT + 5)*sin(0.5*z) + (fixedT - 0.5)*cos(0.1*z) + (fixedT + 10)*sin(0.7*z) + (fixedT - 5)*sin(1*z) + (fixedT + 5)*sin(0.35*z);
}

double Graph::TFixed_ic(double z){
    // if(z >= max_z || z < 0)
    //      return 0.0;

    return getCurrent(fdata, fixedT, z, dt, dz);
    // return 0.0;
    // return (fixedT - 2)*sin(0.19*z) + (fixedT + 5)*sin(0.25*z) + (fixedT - 0.5)*cos(1*z) + (fixedT + 10)*sin(1.4*z) + (fixedT - 5)*cos(1*z) + (fixedT + 5)*cos(0.35*z);
}

double Graph::ZFixed_vo(double t){
    // if(t >= max_t || t < 0)
    //      return 0.0;
         
    return getVoltage(fdata, t, fixedZ, dt, dz);
    // return (fixedZ - 2)*cos(1.39*t) + (fixedZ + 5)*sin(1.5*t) + (fixedZ - 0.5)*cos(0.52*t) + (fixedZ + 10)*sin(0.74*t) + (fixedZ - 5)*sin(0.99*t) + (fixedZ + 5)*sin(0.82*t);
}

double Graph::ZFixed_ic(double t){
    // if(t >= max_t || t < 0)
    //      return 0.0;
         
    return getCurrent(fdata, t, fixedZ, dt, dz);
    // return 0.0;
    // return (fixedZ - 2)*sin(1.09*t) + (fixedZ + 5)*sin(0.39*t) + (fixedZ - 0.5)*cos(0.59*t) + (fixedZ + 10)*sin(1.2*t) + (fixedZ - 5)*cos(1.29*t) + (fixedZ + 5)*cos(1.45*t);
}

cv::Mat Graph::TFixed_Graph(double t){
    fixedT = t;

    PrintParameters p;
    p.xLabel[0] = 'Z';
    p.xLabel[1] = '\0';
    p.max_x = max_z;
    p.voltage = &Graph::TFixed_vo;
    p.current = &Graph::TFixed_ic;


    return print_img(p);
}

cv::Mat Graph::ZFixed_Graph(double z){
    fixedZ = z;

    PrintParameters p;
    p.xLabel[0] = 'T';
    p.xLabel[1] = '\0';
    p.max_x = max_t;
    p.voltage = &Graph::ZFixed_vo;
    p.current = &Graph::ZFixed_ic;

    return print_img(p);
}

void Graph::updateParameters(functionData_t *fdata, double nt, double nz, double dt, double dz){
    // double max = (fdata->maxVoltage > fdata->maxCurrent) ? fdata->maxVoltage : fdata->maxCurrent;
    // double min = (fdata->minVoltage < fdata->minCurrent) ? fdata->minVoltage : fdata->minCurrent;

    // max += 0.5; min -= 0.5;
    double maxV = fdata->maxVoltage + 0.5;      double minV = fdata->minVoltage - 0.5;
    double maxC = fdata->maxCurrent + 0.05;     double minC = fdata->minCurrent - 0.05;
    max_vo = maxV;  min_vo = minV;
    max_ic = maxC;  min_ic = minV*maxC/maxV;
    max_t = nt;
    max_z = nz;

    std::cout << "nt: " << nt << ",  dt: " << dt << ",  nz: " << nz << ",  dz: " << dz << ",  max_vo: " << max_vo << ",  min_vo: " << min_vo << ",  max_ic: " << max_ic << ",  min_ic: " << min_ic << std::endl;
    std::cout << "maxVoltage: " << fdata->maxVoltage << ",  minVoltage: " << fdata->minVoltage << ",  maxCurrent: " << fdata->maxCurrent << ",  minCurrent: " << fdata->minCurrent << std::endl;
    std::cout << std::endl;

    this->dt = dt;
    this->dz = dz;
    this->fdata = fdata;
}

cv::Mat Graph::print_img(PrintParameters p){
    int i = 0, j = 0;
    int px = 0;
    double vo,ic;

    int s_width  = width  * SCALE;
    int s_height = height * SCALE;

    int s_y_label_size = _Y_LABEL_SIZE * SCALE;
    int s_x_label_size = _X_LABEL_SIZE * SCALE;

    int s_y_label_dist = _Y_LABEL_DIST * SCALE;
    int s_x_label_dist = _X_LABEL_DIST * SCALE;

    int printable_width = s_width - s_y_label_size;
    int printable_height = s_height - s_x_label_size; 

    double range_vo = max_vo - min_vo;
    double range_ic = max_ic - min_ic;

    cv::Mat image(s_height, s_width, CV_8UC3, white);
    cv::line(image, cv::Point(0, printable_height), cv::Point(s_width, printable_height), black, SCALE);
    cv::line(image, cv::Point(s_y_label_size, 0), cv::Point(s_y_label_size, s_height), black, SCALE);

    cv::Vec3b ic_color = green;
    cv::Vec3b vo_color = orange;

    double vo2px = printable_height / range_vo;
    double ic2px = printable_height / range_ic;

    double dx = double(p.max_x) / double(printable_width * QTD_PER_PX);
    double dvo = range_vo / printable_height;
    double dic = range_ic / printable_height;

    double prev_vo, prev_ic;
    prev_vo = (this->*(p.voltage))(dx) * vo2px;
    prev_ic = (this->*(p.current))(dx) * ic2px;
    prev_vo = (printable_height / 2) - prev_vo;
    prev_ic = (printable_height / 2) - prev_ic;

    double vo0px = (printable_height * max_vo) / range_vo;
    double ic0px = (printable_height * max_ic) / range_ic;

    for(i = 1; i < printable_width * QTD_PER_PX; i ++){
        // if(dx * i < 1)
            // std::cout << "voltage: " << (this->*(p.voltage))(dx * i) << ",  current: " << (this->*(p.current))(dx * i) << std::endl;

        vo = (this->*(p.voltage))(dx * i) * vo2px;
        ic = (this->*(p.current))(dx * i) * ic2px;

        vo = vo0px - vo;
        ic = ic0px - ic;
        
        px = i / QTD_PER_PX + s_y_label_size; 
        if(vo < 0) vo = 0;
        if(vo > printable_height) vo = printable_height;
        
        if(ic < 0) ic = 0;
        if(ic > printable_height) ic = printable_height;

        cv::line(image, cv::Point(px, prev_vo), cv::Point(px, vo), vo_color, SCALE);
        cv::line(image, cv::Point(px, prev_ic), cv::Point(px, ic), ic_color, SCALE);

        prev_vo = vo;
        prev_ic = ic;
    }

    if(SCALE >= 3)
        cv::GaussianBlur(image, image, cv::Size(3,3), 0);

    //Axis values
    char value[20];
    cv::Size textSize;
    memset(value, '\0', 20 * sizeof(char));

    i = s_x_label_dist;
    for(; i < s_width - s_x_label_dist; i += s_x_label_dist){
        if(p.xLabel[0] == 'T')
            sprintf(value, "%.1e", dx * i * QTD_PER_PX);
        else
            sprintf(value, "%.2f", dx * i * QTD_PER_PX);

        textSize = getTextSize(value, cv::FONT_HERSHEY_PLAIN, 1 * SCALE, 1 * SCALE, 0);
        cv::putText(image, value, cv::Point(i + s_y_label_size - textSize.width, s_height - 5), cv::FONT_HERSHEY_PLAIN, 1 * SCALE, black, 1 * SCALE);
    }
    




    double dist = 0;
    for(dist = 0; dist < 10; dist += 0.125){
        if(dist * vo2px >= s_y_label_dist)
            break;
    }
    dist *= vo2px;

    i = printable_height - vo0px - 10*dist;
    for(; i < printable_height - s_y_label_dist; i += dist){
        if(i < s_y_label_dist)
            continue;

        sprintf(value, "%6.1f", dvo * i + min_vo);
        textSize = getTextSize(value, cv::FONT_HERSHEY_PLAIN, 1 * SCALE, 1 * SCALE, 0);
        cv::putText(image, value, cv::Point(0, printable_height - i + textSize.height), cv::FONT_HERSHEY_PLAIN, 1 * SCALE, vo_color, 1 * SCALE);

        sprintf(value, "%6.2f", dic * i + min_ic);
        textSize = getTextSize(value, cv::FONT_HERSHEY_PLAIN, 1 * SCALE, 1 * SCALE, 0);
        cv::putText(image, value, cv::Point(60 * SCALE, printable_height - i + textSize.height), cv::FONT_HERSHEY_PLAIN, 1 * SCALE, ic_color, 1 * SCALE);
    }
    


    //Subtitle

    cv::line(image, cv::Point(s_width - 90 * SCALE, 25 * SCALE), cv::Point(s_width - 40 * SCALE, 25 * SCALE), vo_color, 2 * SCALE);
    cv::line(image, cv::Point(s_width - 90 * SCALE, 55 * SCALE), cv::Point(s_width - 40 * SCALE, 55 * SCALE), ic_color, 2 * SCALE);
    cv::putText(image, "v", cv::Point(s_width - 25 * SCALE, 30 * SCALE), cv::FONT_HERSHEY_SIMPLEX, 1.0 * SCALE, vo_color, 2 * SCALE);
    cv::putText(image, "i", cv::Point(s_width - 20 * SCALE, 60 * SCALE), cv::FONT_HERSHEY_SIMPLEX, 0.8 * SCALE, ic_color, 2 * SCALE);

    cv::putText(image, "v", cv::Point(30 * SCALE, 20 * SCALE), cv::FONT_HERSHEY_SIMPLEX, 1.0 * SCALE, vo_color, 2 * SCALE);
    cv::putText(image, "i", cv::Point(95 * SCALE, 20 * SCALE), cv::FONT_HERSHEY_SIMPLEX, 0.8 * SCALE, ic_color, 2 * SCALE);

    cv::putText(image, p.xLabel, cv::Point(s_width - 25 * SCALE, s_height - 5 * SCALE), cv::FONT_HERSHEY_SIMPLEX, 0.8 * SCALE, black, 2 * SCALE);
    
    cv::resize(image, image, cv::Size(), 1.0 / double(SCALE), 1.0 / double(SCALE), cv::INTER_CUBIC);
    return image;
}


Graph::Graph(functionData_t *fdata, int width, int height, double TMax, double ZMax, double dt, double dz){
    this->width = width;
    this->height = height;

    min_t = 0;
    min_z = 0;

    updateParameters(fdata, TMax, ZMax, dt, dz);

    blue        = cv::Vec3b( 57, 106, 177);
    orange      = cv::Vec3b(218, 124,  48);
    green       = cv::Vec3b( 62, 150,  81);
    red         = cv::Vec3b(204,  37,  41);
    dark_grey   = cv::Vec3b( 83,  81,  84);
    purple      = cv::Vec3b(107,  76, 154);
    dark_red    = cv::Vec3b(146,  36,  40);
    sycamore    = cv::Vec3b(148, 139,  61);

    white       = cv::Vec3b(255, 255, 255);
    black       = cv::Vec3b(  0,   0,   0);

//    blue        = cv::Vec3b(177, 106,  57);
//    orange      = cv::Vec3b( 48, 124, 218);
//    green       = cv::Vec3b( 81, 150,  62);
//    red         = cv::Vec3b( 41,  37, 204);
//    dark_grey   = cv::Vec3b( 84,  81,  83);
//    purple      = cv::Vec3b(154,  76, 107);
//    dark_red    = cv::Vec3b( 40,  36, 146);
//    sycamore    = cv::Vec3b( 61, 139, 148);
}  
