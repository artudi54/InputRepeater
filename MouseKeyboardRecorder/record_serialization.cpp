#include "stdafx.h"
#include "record_serialization.hpp"

namespace chrono = std::chrono;

QDataStream & record_serialization::operator<<(QDataStream & stream, const InputRecord & record) {
	const auto& actions = record.get_actions();
	std::uint8_t recordType = static_cast<std::uint8_t>(record.get_record_type());
	std::uint64_t recordSize = record.size();
	stream << recordType << recordSize;
	for (const auto & action : actions) {
		std::int64_t count = action.time.count();
		stream << count;
		stream.writeRawData(reinterpret_cast<const char*>(&action.input), sizeof(INPUT));
	}
	return stream;
}

QDataStream & record_serialization::operator>>(QDataStream & stream, InputRecord & record) {
	record.clear();

	INPUT input;
	QByteArray::fromRawData(reinterpret_cast<const char*>(&input), sizeof(INPUT));
	std::uint8_t recordType;
	std::uint64_t recordSize;
	stream >> recordType >> recordSize;
	record.set_record_type(static_cast<InputRecord::RecordType>(recordType));
	record.reserve(static_cast<std::size_t>(recordSize));
	while (recordSize--) {
		std::int64_t time;
		stream >> time;
		stream.readRawData(reinterpret_cast<char*>(&input), sizeof(INPUT));
		record.add(chrono::microseconds(static_cast<chrono::microseconds::rep>(time)), input);
	}

	return stream;
}

bool record_serialization::save_to_file(const InputRecord & record, const QString & fileName) {
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QDataStream stream(&file);
	stream << record;
	return stream.status() == QDataStream::Ok;
}

std::optional<QFileInfo> record_serialization::save_to_dialog(QWidget *parent, const InputRecord &record, const RecordManager & manager) {
	QFileDialog dialog(parent);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setNameFilters({
		QStringLiteral("Record file (*.rec)"),
		QStringLiteral("All files (*)")
	});
	if (manager.is_valid()) {
		QFileInfo info = manager.get_new_save_path();
		dialog.setDirectory(info.absoluteDir());
		dialog.selectFile(info.fileName());
	} else {
		dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
		dialog.selectFile(QStringLiteral("record.rec"));
	}
	if (dialog.exec() == QDialog::Accepted) { 
		QFileInfo selectedFileInfo = QFileInfo(dialog.selectedFiles().front());
		if (record_serialization::save_to_file(record, selectedFileInfo.filePath()))
			return selectedFileInfo; //valid
		return QFileInfo(); //invalid
	}
	return std::optional<QFileInfo>(); //no info
}
