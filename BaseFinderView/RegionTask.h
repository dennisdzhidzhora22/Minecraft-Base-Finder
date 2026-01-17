#pragma once

#include <QObject>
#include <QRunnable>
#include <QImage>
#include <filesystem>
#include "BlockFilter.h"

namespace fs = std::filesystem;

class RegionTask : public QObject, public QRunnable {
	Q_OBJECT

public:
	RegionTask(const fs::path path, std::shared_ptr<BlockFilter> filter);

	int colorMap(long long score, long long highestScore);
	void run() override;

signals:
	void scanFinished(QImage image, int x, int z);

private:
	fs::path m_path;
	std::shared_ptr<BlockFilter> m_filter;

};