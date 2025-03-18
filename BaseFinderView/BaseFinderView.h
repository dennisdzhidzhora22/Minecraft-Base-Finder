#pragma once

#include <QtWidgets/QWidget>
#include "ui_BaseFinderView.h"
#include <QImage>
#include <utility>

class BaseFinderView : public QWidget
{
    Q_OBJECT

public:
    BaseFinderView(QWidget *parent = nullptr);
    ~BaseFinderView();

private:
    Ui::BaseFinderViewClass ui;

private slots:
    void on_startButton_clicked();
};
