#include "stdafx.h"
#include "BaseFinderView.h"
#include "Region.h"
#include "BlockFilter.h"
#include <QFileDialog>
#include <QDebug>
#include <QThreadPool>

#include <utility>
#include <filesystem>
#include <set>
#include <memory>
#include "ZoomableGraphicsView.h"
#include "RegionTask.h"

namespace fs = std::filesystem;

BaseFinderView::BaseFinderView(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    coordLabel = new QLabel(ui.graphicsView);
    coordLabel->setStyleSheet("background-color: rgba(0, 0, 0, 150); color: white;");
    coordLabel->setText("X: N/A (N/A)\nZ: N/A (N/A)");
    coordLabel->move(10, 10);
    coordLabel->adjustSize();
    coordLabel->show();

    scene = new QGraphicsScene;
    ui.graphicsView->setScene(scene);
    ui.graphicsView->show();

    connect(ui.startButton, &QPushButton::clicked, this, &BaseFinderView::startScan);
    connect(ui.regionsPathDialogButton, &QPushButton::clicked, this, &BaseFinderView::openRegionFileDialog);
    connect(ui.graphicsView, &ZoomableGraphicsView::mouseMoved, this, &BaseFinderView::updateCoords);
    
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

void BaseFinderView::startScan() {
    if (regionsDirectory.size() != 0 and numReg > 0) {
        //if (ui.graphicsView->scene()) {
        /*if (alreadyScanned) {
            delete ui.graphicsView->scene();
            scene = new QGraphicsScene;
            ui.graphicsView->setScene(scene);
            ui.graphicsView->show();
        }*/
        scene->clear();

        ui.startButton->setEnabled(false);

        /*BlockFilter* defaultFilter = new BlockFilter;
        defaultFilter->loadDefault();*/

        auto defaultFilter = std::make_shared<BlockFilter>();
        defaultFilter->loadDefault();

        std::set<fs::path> sorted_paths;
        for (const auto& entry : fs::directory_iterator(regionsDirectory)) {
            if (fs::is_regular_file(entry) && (entry.path().extension() == ".mca"))
                sorted_paths.insert(entry.path());
        }

        activeTasks = sorted_paths.size();

        //int count = 0;
        for (const fs::path& file : sorted_paths) {
            RegionTask* task = new RegionTask(file.string(), defaultFilter);

            connect(task, &RegionTask::scanFinished, this, &BaseFinderView::addRegion, Qt::QueuedConnection);

            QThreadPool::globalInstance()->start(task);

            ////if (count == 500) break;

            //Region* reg1 = new Region(file.string(), *defaultFilter);
            //reg1->startTask();

            //long long highestScore = -1;

            //for (int i = 0; i < 1024; i++) {
            //    //std::pair<int, int> score = reg1.getChunkScore(i);
            //    //long long mult = score.first * score.second;
            //    long long mult = reg1->getChunkScoreCount(i) * reg1->getChunkScoreVariety(i);

            //    if (mult > highestScore)
            //        highestScore = mult;
            //}

            //QImage tileImage(32, 32, QImage::Format_RGB32);
            //tileImage.fill(Qt::transparent);

            //for (int i = 0; i < 1024; i++) {
            //    //std::pair<int, int> preScore = reg1.getChunkScore(i);
            //    //long long mult = preScore.first * preScore.second;
            //    long long mult = reg1->getChunkScoreCount(i) * reg1->getChunkScoreVariety(i);

            //    int score = colorMap(mult, highestScore);

            //    int localX = reg1->getChunkX(i) & 31;
            //    int localZ = reg1->getChunkZ(i) & 31;

            //    tileImage.setPixel(localX, localZ, qRgb(score, 0, 0));
            //}

            //int rX, rZ;
            //if (sscanf_s(file.filename().string().c_str(), "r.%d.%d.mca", &rX, &rZ) == 2) {
            //    //QGraphicsPixmapItem* item = scene->addPixmap(QPixmap::fromImage(tileImage));
            //    //item->setPos(rX * 32, rZ * 32);
            //    QCoreApplication::processEvents();
            //}

            ////scene->addPixmap(QPixmap::fromImage(tileImage));

            //delete reg1;
            ////count++;
        }

        //ui.startButton->setEnabled(true);
        //delete defaultFilter;
    }

    //alreadyScanned = true;
}

void BaseFinderView::openRegionFileDialog() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);

    QStringList regionsPath;
    if (dialog.exec()) {
        regionsPath = dialog.selectedFiles();
    }

    if (!regionsPath.empty()) {
        regionsDirectory = regionsPath[0].toStdU16String();
        int numRegions = 0;
        //fs::path fsPath(regionsDirectory);

        for (const auto& entry : fs::directory_iterator(regionsDirectory)) {
            if (fs::is_regular_file(entry) && (entry.path().extension() == ".mca")) {
                numRegions++;
            }
        }

        ui.regionsPath->setReadOnly(false);
        ui.regionsPath->setText(regionsPath[0]);
        ui.regionsPath->setReadOnly(true);

        numReg = numRegions;
        ui.numRegionsLabel->setText(QString::number(numRegions) + " Regions");
    }

}

void BaseFinderView::updateCoords(int chunkX, int chunkZ) {
    int blockX = chunkX * 16;
    int blockZ = chunkZ * 16;

    QString text = QString("X: %1 (%2)\nZ: %3 (%4)")
                    .arg(chunkX)
                    .arg(blockX)
                    .arg(chunkZ)
                    .arg(blockZ);

    coordLabel->setText(text);
    coordLabel->adjustSize();
}

void BaseFinderView::addRegion(QImage image, int rX, int rZ) {
    QGraphicsPixmapItem* item = scene->addPixmap(QPixmap::fromImage(image));
    item->setPos(rX * 32, rZ * 32);

    activeTasks--;
    if (activeTasks <= 0) {
        ui.startButton->setEnabled(true);
    }
}

BaseFinderView::~BaseFinderView()
{}
