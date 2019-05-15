#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    inputFile(":/text/files/suivre.txt")
{
    ui->setupUi(this);
    rs = new RealSense();
    ui->confirmation->setVisible(false);
    ui->demarre->setVisible(false);

    inputFile.open(QIODevice::ReadOnly);
    filereader = new QTextStream(&inputFile);
    filereader->setCodec("UTF-8");

    QString text = this->loadText();
    ui->textBrowser->append(text);
    gif = new QMovie(":/images/gif/waiting.gif");
    ui->onGif->setMovie(gif);
    name = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete gif;
    delete rs;
    delete filereader;
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
        rs->startRecording(vidName);

    }
    else {
        gif->stop();
        ui->demarre->setText("Démarrer l'enregistrement");
        ui->confirmation->setVisible(true);
        rs->stopRecording(vidName);

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
    QString text = this->loadText();
    ui->textBrowser->clear();
    ui->textBrowser->append(text);

}

/**
 * @brief MainWindow::loadText
 * @return The next action to do in the plan library
 */
QString MainWindow::loadText(){
    return filereader->readLine();
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
