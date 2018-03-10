#pragma once
class RecordManager {
public:
	RecordManager(QString directory, QString nameStart);
	bool is_valid() const noexcept;
	QFileInfo get_new_save_path() const;
	unsigned item_count() const;
private:
	QDir directory;
	QString nameStart;
};

