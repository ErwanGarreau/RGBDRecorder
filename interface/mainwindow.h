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
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

#if __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#endif
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

    void on_actionStart_3_triggered();

private:
    Ui::MainWindow *ui;
    VideoSource* rs;
    QMovie * gif;
    QMovie * saving;
    QFile planFile;
    QTextStream* planReader;
    QFile nameFile;
    QTextStream* nameReader;
    std::map<std::string,QString> plans;
    std::map<std::string,QString>::iterator it;
    std::set<std::string> folders;
    int name;

    bool play = false;
    void gestionEnregistrement();
    void enregistrerVideo();
    void gestionExplications();
    void recVideo(std::string nom);
    void udpFrame();
    void advanceText();
    void getNextPlan();


};

#endif // MAINWINDOW_H
