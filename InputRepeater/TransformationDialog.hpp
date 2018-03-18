#pragma once

#include "ui_TransformationDialog.h"
#include <InputRecord.hpp>
#include "RecordManager.hpp"

class TransformationDialog : public QDialog {
	Q_OBJECT
public:
	TransformationDialog(InputRecord &record, QWidget *parent = nullptr);
	virtual ~TransformationDialog();

	void set_file_name(QString fileName);
private slots:
	void user_transform_dialog();
private:
	bool check_for_any_transformation();
	void make_message_box(QMessageBox &questionMessage, QPushButton *&buttonOverwrite, QPushButton *&buttonCopyOld);

	void set_time(std::chrono::microseconds time);
	void set_record_type(InputRecord::RecordType recordType);
	void transform();
	Ui::TransformationDialog ui;
	InputRecord &record;
	RecordManager manager;
	QString fileName;
};
