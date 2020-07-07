#include "animation.h"
#include <iostream>

Animation::Animation(double min, double max, int steps, double delay){
    this->min = min;
    this->max = max;
    this->steps = steps;
    this->delay = delay;
    running = true;
    ending = false;

    QRunnable::setAutoDelete(false);
}

void Animation::run(){
    double step = (max - min)/(double)steps;

    for(d = min; d < max; d += step){
        while(!running);
        if(ending){
            ending = false;
            break;
        }

        emit updateGraphic(d);

        usleep(delay);

    }

    emit Tfinished();
}

bool Animation::getRunning(){
    return running;
}

void Animation::setRunning(bool running){
    this->running = running;
}

void Animation::setDelay(double delay){
    this->delay = delay;
}

void Animation::setEnding(){
    this->ending = true;
}
