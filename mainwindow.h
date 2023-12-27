#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFlag>
#include <QMainWindow>
#include <QPushButton>
#include <QScreen>
#include <QVector>
#include <QWidget>

#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    void openBinaryFileAndPlot();

  private:
    Ui::MainWindow* ui;
    QPushButton* fileButton;
    QCustomPlot* customPlot;
};
#endif  // MAINWINDOW_H
