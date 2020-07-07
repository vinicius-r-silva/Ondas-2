#include "tview.h"
#include "ui_tview.h"

#include <iostream>

#define MAXMEMORY 2000000000 //2GB
#define ZDELAY 1
#define TDELAY 1 // 10000*n -> n seconds
#define NDELAY 100000         // n times to animation

TView::TView(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::TView){
    //////////////////////////////////////////////////////////////////////////////////////////
    // SET THE UI PARAMETERS AND SOME ICONS TO INTERFACE

    startingUp = true;
    ui->setupUi(this);

    this->setStyleSheet("background-color: white;");

    QSize iconsSize(46, 46);

    ui->firstV->setIcon(QIcon(":/icons/firstV.png"));
    ui->secondV->setIcon(QIcon(":/icons/secondV.png"));
    ui->firstR->setIcon(QIcon(":/icons/firstR.png"));
    ui->secondR->setIcon(QIcon(":/icons/secondR.png"));
    ui->thirdR->setIcon(QIcon(":/icons/thirdR.png"));

    ui->BtPauseT->setIcon(QIcon(":/icons/small_pause.png"));
    ui->BtPauseZ->setIcon(QIcon(":/icons/small_pause.png"));
    ui->BtPlayT->setIcon(QIcon(":/icons/small_play.png"));
    ui->BtPlayZ->setIcon(QIcon(":/icons/small_play.png"));
    ui->BtStopT->setIcon(QIcon(":/icons/small_stop.png"));
    ui->BtStopZ->setIcon(QIcon(":/icons/small_stop.png"));

    ui->firstV->setIconSize(iconsSize);
    ui->secondV->setIconSize(QSize(120, 180));
    ui->firstR->setIconSize(iconsSize);
    ui->secondR->setIconSize(iconsSize);
    ui->thirdR->setIconSize(iconsSize);

    this->setWindowTitle("Transmission Lines");

    //////////////////////////////////////////////////////////////////////////////////////////
    // SET INITIAL PARAMETERS INTO THE INTERFACE

    animT = nullptr;
    animZ = nullptr;

    vol = CONTINUA;
    res = CEM;

    zFix = 0;
    tFix = 0;

    ui->zLine->setText(QString::number(zFix));
    ui->tLine->setText(QString::number(tFix));

    ui->dT->setText(QString::number(1));
    //ui->nT->setText(QString::number(0.000005));
    ui->dZ->setText(QString::number(1));
    ui->nZ->setText(QString::number(200));
    ui->nT->setText(QString::number(200));
    
    dtPrev = dt;
    ntPrev = nt;
    dzPrev = dz;
    nzPrev = nz;

    ui->SlAnimationT->setValue(100);
    ui->SlAnimationZ->setValue(100);

    ui->firstV->setChecked(true);
    ui->thirdR->setChecked(true);

    ui->SliderT->setMaximum(nt);
    ui->SliderZ->setMaximum(nz);

    changed = false;

    //////////////////////////////////////////////////////////////////////////////////////////
    // MAKE THE OBJECTS AND THREADS

    datas = NULL;
    datas = allocMemory(vol, res, dt, nt, dz, nz);
    graphs = new Graph(datas, ui->zGraphic->width(), ui->zGraphic->height(), nt, nz, dt, dz);

    thZ = new QThreadPool();
    thT = new QThreadPool();

    thZ->setExpiryTimeout(-1);
    thT->setExpiryTimeout(-1);

    animT = new Animation(0.0, nz, NDELAY, (double)(ui->SlAnimationT->value() * TDELAY));
    animZ = new Animation(0.0, nt, NDELAY, (double)(ui->SlAnimationZ->value() * ZDELAY));

    QObject::connect(animZ, SIGNAL(Tfinished()), this, SLOT(animationZFinished()));
    QObject::connect(animZ, SIGNAL(updateGraphic(double)), this, SLOT(updateZ(double)));
    QObject::connect(animT, SIGNAL(Tfinished()), this, SLOT(animationTFinished()));
    QObject::connect(animT, SIGNAL(updateGraphic(double)), this, SLOT(updateT(double)));

    //////////////////////////////////////////////////////////////////////////////////////////
    // UPDATE THE GRAPHICS

    updateTGraphic();
    updateZGraphic();

    startingUp = false;
}

TView::~TView() {
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//ANIMATIONS FUNCTIONS

void TView::updateZ(double d){
    ui->SliderT->setValue(d);
}

void TView::updateT(double d){
    ui->SliderZ->setValue(d);
}

void TView::animationZFinished(){
    ui->SliderT->setValue(0);
}

void TView::animationTFinished(){
    ui->SliderZ->setValue(0);
}

void TView::on_SlAnimationT_valueChanged(int value){
    if(animT != nullptr)
        animT->setDelay((double)(value * TDELAY));
}

void TView::on_SlAnimationZ_valueChanged(int value){
    if(animZ != nullptr)
        animZ->setDelay((double)(value * ZDELAY));
}

void TView::on_BtPlayT_clicked(){
    if(thT->activeThreadCount() == 0){
        std::cout << "Iniciando a thread do T\n";
        thT->tryStart(animT);
    }else if(!animT->getRunning()){
        animT->setRunning(true);
    }
}

void TView::on_BtPlayZ_clicked(){
    if(thZ->activeThreadCount() == 0){
        std::cout << "Iniciando a thread do Z\n";
        thZ->tryStart(animZ);
    }else if(!animZ->getRunning()){
        animZ->setRunning(true);
    }
}

void TView::on_BtPauseT_clicked(){
    if(thT->activeThreadCount() > 0){
        animT->setRunning(false);
    }
}

void TView::on_BtPauseZ_clicked(){
    if(thZ->activeThreadCount() > 0){
        animZ->setRunning(false);
    }
}

void TView::on_BtStopT_clicked(){
    if(thT->activeThreadCount() > 0){
        animT->setEnding();
        animT->setRunning(true);
    }
}

void TView::on_BtStopZ_clicked(){
    if(thZ->activeThreadCount() > 0){
        animZ->setEnding();
        animZ->setRunning(true);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// UPDATES FUNCTIONS

bool TView::parametersValid(){
    int n = (int)(nt/dt) * (int)(nz/dz);

    if((long int)8 * n > MAXMEMORY){
        return false;
    }

    return true;
}

void TView::updateTGraphic(){
    cv::Mat img = graphs->TFixed_Graph(tFix);
    ui->tGraphic->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
}

void TView::updateZGraphic(){
    cv::Mat img = graphs->ZFixed_Graph(zFix);
    ui->zGraphic->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
}

void TView::on_BtRecalcular_clicked(){
    if(changed && !startingUp){
        if(parametersValid()){
            if(thT->activeThreadCount() > 0){
                animT->setEnding();
                animT->setRunning(true);
            }

            if(thZ->activeThreadCount() > 0){
                animZ->setEnding();
                animZ->setRunning(true);
            }

            changed = false;
            freeMemory(datas);
            datas = allocMemory(vol, res, dt, nt, dz, nz);
            graphs->updateParameters(datas, nt, nz, dt, dz);
            updateTGraphic();
            updateZGraphic();
            dtPrev = dt;
            ntPrev = nt;
            dzPrev = dz;
            nzPrev = nz;
        }else{
            QMessageBox::warning(this, tr("ERROR BOX"), tr("Parametros ultrapassam o limite da memória"));
            ui->dT->setText(QString::number(dtPrev));
            ui->nT->setText(QString::number(ntPrev));
            ui->dZ->setText(QString::number(dzPrev));
            ui->nZ->setText(QString::number(nzPrev));
            changed = false;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////
// PARAMETERS FUNCTIONS

void TView::on_firstV_clicked(){
    if(ui->firstV->isChecked() && vol != CONTINUA){
        vol = CONTINUA;
        changed = true;
        on_BtRecalcular_clicked();
    }
}

void TView::on_secondV_clicked(){
    if(ui->secondV->isChecked() && vol != DEGRAU){
        vol = DEGRAU;
        changed = true;
        on_BtRecalcular_clicked();
    }
}

void TView::on_firstR_clicked(){
    if(ui->firstR->isChecked() && res != INFINITA){
       res = INFINITA;
        changed = true;
        on_BtRecalcular_clicked();
    }
}

void TView::on_secondR_clicked(){
    if(ui->secondR->isChecked() && res != ZERO){
        res = ZERO;
        changed = true;
        on_BtRecalcular_clicked();
    }
}

void TView::on_thirdR_clicked(){
    if(ui->thirdR->isChecked() && res != CEM){
        res = CEM;
        changed = true;
        on_BtRecalcular_clicked();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GRAPHICS Z AND T FIX PARAMETERS FUNCTIONS

void TView::on_SliderT_valueChanged(){
    ui->tLine->setText(QString::number(double(ui->SliderT->value())));
}

void TView::on_SliderZ_valueChanged(){
    ui->zLine->setText(QString::number(double(ui->SliderZ->value())));
}

void TView::on_zLine_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double fixZ;
    try{
        fixZ = std::stod(edit);

        if(fixZ != zFix){
            zFix = fixZ;
            updateZGraphic();
        }
    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void TView::on_tLine_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double fixT;
    try{
        fixT = std::stod(edit);

        if(fixT != tFix){
            tFix = fixT;
            updateTGraphic();
        }
    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// ADVANCED FUNCTIONS

void TView::on_dT_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double dT;
    try{
        dT = std::stod(edit);

        if(dT != dt && dT != 0){
            dt = dT;
            changed = true;
            on_BtRecalcular_clicked();
        }

    }catch(std::invalid_argument e){

    }catch(std::out_of_range e2){

    }
}

void TView::on_nT_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double nT;
    try{
        nT = std::stod(edit);

        if(nT != nt && nT != 0){
            nt = nT;
            changed = true;
            on_BtRecalcular_clicked();
        }
    }catch(std::invalid_argument e){

    }catch(std::out_of_range e2){

    }
}

void TView::on_dZ_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double dZ;
    try{
        dZ = std::stod(edit);

        if(dZ != dz && dZ != 0){
            dz = dZ;
            changed = true;
            on_BtRecalcular_clicked();
        }
    }catch(std::invalid_argument e){

    }catch(std::out_of_range e2){

    }

}

void TView::on_nZ_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    if(arg1.contains('.')){
        int index = arg1.indexOf('.');
        edit.replace(uint32_t(index), 1, ",");
    }

    double nZ;
    try{
        nZ = std::stod(edit);
        if(nZ != nz && nZ != 0){
            nz = nZ;
            changed = true;
            on_BtRecalcular_clicked();
        }

    }catch(std::invalid_argument e){

    }catch(std::out_of_range e2){

    }
}


