#ifndef TVIEW_H
#define TVIEW_H

#include <QMainWindow>
#include <QtWidgets>
#include "graph.h"

namespace Ui {
class TView;
}

class TView : public QMainWindow {
    Q_OBJECT

public:
    explicit TView(QWidget *parent = nullptr);
    ~TView();

private slots:
    void on_BtRecalcular_clicked();

    bool parametersValid();

    void updateGraphic();

    void on_Slider_valueChanged();

    void on_rb_square_clicked();
    void on_rb_gauss_clicked();

    void on_rb_oneC_clicked();
    void on_rb_twoC_clicked();
    void on_rb_threeC_clicked();

    void on_pb_zApply_clicked();
    void on_pb_zRestore_clicked();

    void on_le_minX_textChanged(const QString &arg1);
    void on_le_maxX_textChanged(const QString &arg1);

    void on_le_n1_textChanged(const QString &arg1);
    void on_le_n2_textChanged(const QString &arg1);
    void on_le_n3_textChanged(const QString &arg1);
    void on_le_s1_textChanged(const QString &arg1);
    void on_le_s2_textChanged(const QString &arg1);

private:
    QThreadPool *th;
    functionData_t *datas;
    Ui::TView *ui;
    Graph *graphs;

    bool changed;
    bool startingUp;
    int pulse;
    int lineNum;

    double _minX, _maxX, _fix;
    double _n1, _n2, _n3;
    double _s1, _s2;

    double _prMinX, _prMaxX, _prFix;
    double _prn1, _prn2, _prn3;
    double _prs1, _prs2;
};

#endif // TVIEW_H
