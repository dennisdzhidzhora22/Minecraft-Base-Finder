#pragma once

#include <QtWidgets/QWidget>
#include <QImage>
#include "ui_BaseFinderView.h"

class BaseFinderView : public QWidget
{
    Q_OBJECT

public:
    BaseFinderView(QWidget *parent = nullptr);
    ~BaseFinderView();

private:
    std::u16string regionsDirectory;
    int numReg = 0;
    int activeTasks = 0;
    bool alreadyScanned = false;

    QLabel* coordLabel;
    QGraphicsScene* scene;

    Ui::BaseFinderViewClass ui;

private slots:
    void startScan();
    void openRegionFileDialog();
    void updateCoords(int x, int z);
    void addRegion(QImage image, int x, int z);
};
