#pragma once

#include <InputRecord.hpp>
#include "RecordManager.hpp"

namespace record_serialization {
	QDataStream &operator<<(QDataStream &stream, const InputRecord &record);
	QDataStream &operator>>(QDataStream &stream, InputRecord &record);
	bool save_to_file(const InputRecord &record, const QString &fileName);
	std::optional<QFileInfo> save_to_dialog(QWidget *parent, const InputRecord &record, const RecordManager & manager);
}

using record_serialization::operator<<;
using record_serialization::operator>>;