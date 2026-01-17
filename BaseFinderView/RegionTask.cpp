#include "RegionTask.h"
#include "Region.h"

RegionTask::RegionTask(const fs::path path, std::shared_ptr<BlockFilter> filter) : m_path(path), m_filter(filter) {
	setAutoDelete(true);
}

int RegionTask::colorMap(long long score, long long highestScore) {
    int v = ((static_cast<double>(score) / static_cast<double>(highestScore)) * 256.0);
    if (v < 0)
        return 0;
    else if (v > 255)
        return 255;
    else
        return v;
}

void RegionTask::run() {
	Region* reg = new Region(m_path.string(), *m_filter);
	reg->startTask();

    long long highestScore = -1;

    for (int i = 0; i < 1024; i++) {
        long long mult = reg->getChunkScoreCount(i) * reg->getChunkScoreVariety(i);

        if (mult > highestScore)
            highestScore = mult;
    }

    QImage tileImage(32, 32, QImage::Format_RGB32);
    tileImage.fill(Qt::transparent);

    for (int i = 0; i < 1024; i++) {
        long long mult = reg->getChunkScoreCount(i) * reg->getChunkScoreVariety(i);

        int score = colorMap(mult, highestScore);

        int localX = reg->getChunkX(i) & 31;
        int localZ = reg->getChunkZ(i) & 31;

        tileImage.setPixel(localX, localZ, qRgb(score, 0, 0));
    }

    int rX, rZ;
    sscanf_s(m_path.filename().string().c_str(), "r.%d.%d.mca", &rX, &rZ);

    delete reg;

    emit scanFinished(tileImage, rX, rZ);
}
