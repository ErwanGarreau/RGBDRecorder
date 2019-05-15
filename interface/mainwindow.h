#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <ctime>
#include <QMovie>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include <QString>
#include "../recorder/VideoSource.h"
#include "../recorder/RealSense.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_demarre_clicked();
    void on_actionStart_triggered();
    void on_confirmation_clicked();

private:
    Ui::MainWindow *ui;
    VideoSource* rs;
    QMovie * gif;
    QMovie * saving;
    QFile inputFile;
    QTextStream* filereader;

    bool play = false;
    void gestionEnregistrement();
    void enregistrerVideo();
    void gestionExplications();
    void recVideo(std::string nom);
    void udpFrame();
    QString loadText();
    int name;


};

#endif // MAINWINDOW_H
