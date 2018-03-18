#include "stdafx.h"
#include "RecordManager.hpp"


RecordManager::RecordManager(QString directory, QString nameStart)
	: directory(directory)
	, nameStart(nameStart) {
	if (!this->directory.exists())
		this->directory.mkdir(QStringLiteral("."));
}





bool RecordManager::is_valid() const noexcept {
	return directory.exists();
}




QFileInfo RecordManager::get_new_save_path() const {
	bool ok;
	QFileInfoList entries = directory.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
	std::vector<unsigned> entriesIndices;
	entriesIndices.reserve(entries.size());

	for (QFileInfo &info : entries) {
		QString fileName = info.completeBaseName();
		if (fileName.startsWith(nameStart)) {

			unsigned index = fileName.mid(nameStart.size()).toUInt(&ok);
			if (ok)
				entriesIndices.push_back(index);
		}
	}

	std::sort(entriesIndices.begin(), entriesIndices.end());

	if (entriesIndices.empty() || entriesIndices.front() != 1)
		return directory.filePath(nameStart + "001.rec");
	for (std::size_t i = 1; i < entriesIndices.size(); ++i)
		if (entriesIndices[i] - entriesIndices[i - 1] > 1)
			return directory.filePath(
				nameStart + QString("%1").arg(entriesIndices[i - 1] + 1, 3, 10, QChar('0')) + ".rec"
			);
	return directory.filePath(
		nameStart + QString("%1").arg(entriesIndices.back() + 1, 3, 10, QChar('0')) + ".rec"
	);

}




unsigned RecordManager::item_count() const {
	return directory.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot).size();
}