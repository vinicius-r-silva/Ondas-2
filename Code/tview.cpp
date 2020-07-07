#include "tview.h"
#include "ui_tview.h"

#include <iostream>

#define MAXMEMORY 2000000000 //2GB
#define DELAY 1 // 10000*n -> n seconds
#define NDELAY 100000         // n times to animation
#define MAX_X 400.0
#define _DX 0.5
#define MAX_Y 5
#define MIN_Y -5

TView::TView(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::TView){

    //////////////////////////////////////////////////////////////////////////////////////////
    // SET THE UI PARAMETERS AND SOME ICONS TO INTERFACE
    ui->setupUi(this);
    this->setWindowTitle("Scalar Waves");

    startingUp = true;

    //////////////////////////////////////////////////////////////////////////////////////////
    // SET INITIAL PARAMETERS INTO THE INTERFACE

    anim = nullptr;

    lineNum = 0;
    _fix = 0;

    ui->le_maxX->setText(QString::number(MAX_X));
    ui->le_maxX->setText(QString::number(0));

    ui->le_Line->setText(QString::number(0));
    ui->SlAnimation->setValue(100);

    ui->rb_square->setChecked(true);
    ui->rb_oneC->setChecked(true);

    ui->pb_zRestore->setEnabled(false);
    ui->le_n2->setEnabled(false);
    ui->le_n3->setEnabled(false);
    ui->le_s2->setEnabled(false);

    changed = false;

    //////////////////////////////////////////////////////////////////////////////////////////
    // MAKE THE OBJECTS AND THREADS

    datas = NULL;
    datas = allocMemory(pulse, lineNum, _DX, MAX_X);
    graphs = new Graph(datas, ui->l_Graphic->width(), ui->l_Graphic->height(), MAX_X, _DX);

    th = new QThreadPool();
    th->setExpiryTimeout(-1);

    //////////////////////////////////////////////////////////////////////////////////////////
    // UPDATE THE GRAPHICS

    //updateGraphic();
    startingUp = false;
}

TView::~TView() {
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//

bool TView::parametersValid(){
    int n;
}

void TView::updateGraphic(){
  //  cv::Mat img = graphs->TFixed_Graph(fix);
  //  ui->l_Graphic->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
}

void TView::on_BtRecalcular_clicked(){
    if(changed && !startingUp){
        if(parametersValid()){
            if(th->activeThreadCount() > 0){
                anim->setEnding();
                anim->setRunning(true);
            }

            changed = false;
            //freeMemory(datas);
            //datas = allocMemory(vol, res, dt, nt, dz, nz);
            //graphs->updateParameters(datas, nt, nz, dt, dz);
            //updateGraphic();
        }else{
            QMessageBox::warning(this, tr("ERROR BOX"), tr("Parametros ultrapassam o limite da memória"));
            changed = false;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////
//

void TView::on_rb_square_clicked(){
    if(pulse != SQUARE){
        pulse = SQUARE;
        changed = true;
    }
}

void TView::on_rb_gauss_clicked(){
    if(pulse != GAUSS){
        pulse = GAUSS;
        changed = true;
    }

}

void TView::on_rb_oneC_clicked(){
    if(lineNum != 1){
        lineNum = 1;
        changed = true;
        ui->le_n2->setEnabled(false);
        ui->le_n3->setEnabled(false);
        ui->le_s2->setEnabled(false);
    }
}

void TView::on_rb_twoC_clicked(){
    if(lineNum != 2){
        lineNum = 2;
        changed = true;
        ui->le_n2->setEnabled(false);
        ui->le_n3->setEnabled(false);
        ui->le_s2->setEnabled(true);
    }
}

void TView::on_rb_threeC_clicked(){
    if(lineNum != 3){
        lineNum = 3;
        changed = true;
        ui->le_n2->setEnabled(true);
        ui->le_n3->setEnabled(true);
        ui->le_s2->setEnabled(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//

void TView::on_Slider_valueChanged(){
    ui->l_Line->setText(QString::number(double(ui->Slider->value())));
}

void TView::on_pb_zApply_clicked(){
    ui->pb_zRestore->setEnabled(true);
}

void TView::on_pb_zRestore_clicked(){
    ui->pb_zRestore->setEnabled(false);
    ui->le_maxX->setText(QString::number(MAX_X));
    ui->le_maxX->setText(QString::number(0));
}

void TView::on_le_minX_textChanged(const QString &arg1){
        std::string edit = arg1.toStdString();

        if(arg1.contains('.')){
            int index = arg1.indexOf('.');
            edit.replace(uint32_t(index), 1, ",");
        }

        double minX;
        try{
            minX = std::stod(edit);

            if(minX != _minX);
                _minX = minX;

        }catch(std::invalid_argument e){
        }catch(std::out_of_range e2){
        }
}

void TView::on_le_maxX_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double maxX;
    try{
        maxX = std::stod(edit);

        if(maxX != _maxX);
            _maxX = maxX;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void TView::on_le_n1_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double n1;
    try{
        n1 = std::stod(edit);
        if(n1 != _n1);
            _n1 = n1;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void TView::on_le_n2_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double n2;
    try{
        n2 = std::stod(edit);
        if(n2 != _n2);
            _n2 = n2;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void TView::on_le_n3_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double n3;
    try{
        n3 = std::stod(edit);
        if(n3 != _n3);
            _n3 = n3;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void TView::on_le_s1_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double s1;
    try{
        s1 = std::stod(edit);
        if(s1 != _s1);
            _s1 = s1;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void TView::on_le_s2_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double s2;
    try{
        s2 = std::stod(edit);

        if(s2 != _s2);
            _s2 = s2;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}
