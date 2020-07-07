#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include <QtWidgets/QLabel>
#include <QThreadPool>
#include <QDebug>
#include <unistd.h>

class Animation : public QObject, public QRunnable{
    Q_OBJECT

public:
    explicit Animation(double min, double max, int steps, double delay);
    void setRunning(bool running);
    void setDelay(double delay);
    void setEnding();
    bool getRunning();

signals:
    void Tfinished();
    void updateGraphic(double d);

private:
    bool running;
    bool ending;
    double delay;
    double min;
    double max;
    int steps;
    double d;

protected:
    void run();
};

#endif // ANIMATION_H
