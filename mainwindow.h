#pragma once

#include "circle.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <vector>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    SpiralFun::Circle* addCircle(qreal radius);
    void advanceCircles(qreal angle);

private:
    void advanceCircle(unsigned index, qreal angle);

    Ui::MainWindow *ui;
    QGraphicsScene mScene;
    std::vector<std::unique_ptr<SpiralFun::Circle>> mCircles;
};
