#include "stdafx.h"
#include "BaseFinderView.h"
#include "Region.h"
#include "BlockFilter.h"
#include <utility>
#include "ZoomableGraphicsView.h"

BaseFinderView::BaseFinderView(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

// Maps score to value from 0-255 based on highest score
int colorMap(long long score, long long highestScore) {
    int v = ((static_cast<double>(score) / static_cast<double>(highestScore)) * 256.0);
    if (v < 0)
        return 0;
    else if (v > 255)
        return 255;
    else
        return v;
}

void BaseFinderView::on_startButton_clicked() {
    BlockFilter* defaultFilter = new BlockFilter;
    defaultFilter->loadDefault();

    Region* reg1 = new Region("r.0.0.mca", *defaultFilter);
    reg1->startTask();

    long long highestScore = -1;

    for (int i = 0; i < 1024; i++) {
        //std::pair<int, int> score = reg1.getChunkScore(i);
        //long long mult = score.first * score.second;
        long long mult = reg1->getChunkScoreCount(i) * reg1->getChunkScoreVariety(i);

        if (mult > highestScore)
            highestScore = mult;
    }

    QImage* map = new QImage(32, 32, QImage::Format_RGB32);

    for (int i = 0; i < 1024; i++) {
        //std::pair<int, int> preScore = reg1.getChunkScore(i);
        //long long mult = preScore.first * preScore.second;
        long long mult = reg1->getChunkScoreCount(i) * reg1->getChunkScoreVariety(i);

        int score = colorMap(mult, highestScore);

        //std::pair<int, int> pos = reg1.getChunkPos(i);

        map->setPixel(reg1->getChunkX(i), reg1->getChunkZ(i), qRgb(score, 0, 0));
    }

    QGraphicsScene* scene = new QGraphicsScene;
    //scene->addText("Hello, world!");
    scene->addPixmap(QPixmap::fromImage(*map));

    ui.graphicsView->setScene(scene);
    ui.graphicsView->show();
}

BaseFinderView::~BaseFinderView()
{}
