#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    planFile(":/text/files/suivre.txt"),
    nameFile(":/text/files/list_plan.txt")
{
    ui->setupUi(this);
    rs = new RealSense();
    ui->confirmation->setVisible(false);
    ui->demarre->setVisible(false);

    planFile.open(QIODevice::ReadOnly);
    planReader = new QTextStream(&planFile);
    planReader->setCodec("UTF-8");

    nameFile.open(QIODevice::ReadOnly);
    nameReader = new QTextStream(&nameFile);
    nameReader->setCodec("UTF-8");
    gif = new QMovie(":/images/gif/waiting.gif");
    ui->onGif->setMovie(gif);
    name = 0;
    this->advanceText();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete gif;
    delete rs;
    delete planReader;
    delete nameReader;
}

/**
 * @brief MainWindow::closeEvent
 * @param event
  * Closing event. The function is used to catch the closing event and assure the program will call it's destructor functoin correctly.
  * DO NOT REMOVE THIS. Otherwise the realsense camera will not be correctly freed.
*/
void MainWindow::closeEvent(QCloseEvent *event)
{
    play = false;
    event->accept();
}


void MainWindow::on_demarre_clicked()
{
    char t[32];

    sprintf(t, "%05d", name);
    std::string vidName = t;
    if (gif->state() != 2) {
        gif->start();
        ui->demarre->setText("Arreter l'enregistrement");
        ui->confirmation->setVisible(false);
        rs->startRecording(it->first);

    }
    else {
        gif->stop();
        ui->demarre->setText("Démarrer l'enregistrement");
        ui->confirmation->setVisible(true);
        rs->stopRecording(it->first,false);

    }

}

void MainWindow::on_actionStart_triggered()
{
    play = true;
    ui->demarre->setVisible(true);
    this->udpFrame();

}

void MainWindow::on_confirmation_clicked()
{
    ui->confirmation->setVisible(false);
    name++;
    ++it;
    ui->textBrowser->clear();
    ui->textBrowser->append(it->second);
}

void MainWindow::getNextPlan(){
    for (auto it : rs->findLastName()){
       if (plans.count(it) > 0)
           plans.erase(it);
    }
    this->it = plans.begin();
    ui->textBrowser->clear();
    ui->textBrowser->append(it->second);
}

void MainWindow::advanceText(){
    while (!(nameReader->atEnd() || planReader->atEnd())){
        std::string name = nameReader->readLine().toUtf8().constData();
        QString plan = planReader->readLine();
        plans[name] = plan;
    }

    this->getNextPlan();
}


/**
 * @brief MainWindow::udpFrame
 * udpFrame process the user events of the main frame while also updating the camera viewpoint.
 */
void MainWindow::udpFrame(){

    while(play) {
        qApp->processEvents();
        this->rs->update();
        cv::Mat img = this->rs->getColorFeed();
        cv::cvtColor(img,img,cv::COLOR_BGR2RGB);
        ui->image->setPixmap(QPixmap::fromImage(QImage((unsigned char*) img.data,img.cols,img.rows,QImage::Format_RGB888)));
        ui->image->repaint();
        if (!play)
            break;

    }
}

void MainWindow::on_actionStart_3_triggered()
{
    rs->extractVideos();
}
