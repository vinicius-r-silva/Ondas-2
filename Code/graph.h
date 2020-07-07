#ifndef GRAPH_CLASS
#define GRAPH_CLASS

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "function.h"

class Graph{

private:
    // int debug_count;

    double TFixed_vo(double z);
    double TFixed_ic(double z);
    double TFixed_tw(double z);

    double ZFixed_vo(double t);
    double ZFixed_ic(double t);
    double ZFixed_tw(double t);

    int width;
    int height;

    double min_t;
    double min_z;
    double max_t;
    double max_z;
    double dt;
    double dz;

    double fixedT;
    double fixedZ;

    double max_vo, min_vo;
    double max_ic, min_ic;

    struct PrintParameters{
        char xLabel[2];
        double (Graph::*voltage)(double);
        double (Graph::*current)(double);
        double (Graph::*thWave)(double);

        double max_x;
    };

    functionData_t *fdata;

    cv::Vec3b white;
    cv::Vec3b black;
    cv::Vec3b blue;
    cv::Vec3b orange;
    cv::Vec3b green;
    cv::Vec3b red;
    cv::Vec3b dark_grey;
    cv::Vec3b purple;
    cv::Vec3b dark_red;
    cv::Vec3b sycamore;

public:
    Graph(functionData_t *fdata, int width, int height, double TMax, double ZMax, double dt, double dz);
    cv::Mat print_img(PrintParameters p);
    void updateParameters(functionData_t *fdata, double nt, double nz, double dt, double dz);

    cv::Mat TFixed_Graph(double t);
    cv::Mat ZFixed_Graph(double z);
};



#endif //GRAPH_CLASS
