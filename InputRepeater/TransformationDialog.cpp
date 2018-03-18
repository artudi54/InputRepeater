#include "stdafx.h"
#include "TransformationDialog.hpp"
#include "record_serialization.hpp"
#include "time_util.hpp"
namespace chrono = std::chrono;


TransformationDialog::TransformationDialog(InputRecord &record, QWidget *parent)
	: QDialog(parent)
	, record(record)
	, manager(QStringLiteral("Records"), QStringLiteral("old_record_"))
	, fileName() {
	ui.setupUi(this);

	this->set_record_type(record.get_record_type());
	this->set_time(record.total_time());

	QObject::connect(ui.checkChangeSpeed, &QCheckBox::toggled, ui.spinBoxSpeed, &QSpinBox::setEnabled);
	QObject::connect(ui.checkMouseOnly, &QCheckBox::toggled, ui.checkKeyboardOnly, &QSpinBox::setDisabled);
	QObject::connect(ui.checkKeyboardOnly, &QCheckBox::toggled, ui.checkMouseOnly, &QSpinBox::setDisabled);
	QObject::connect(ui.buttonOK, &QPushButton::clicked, this, &TransformationDialog::user_transform_dialog);
	QObject::connect(ui.buttonCancel, &QPushButton::clicked, this, &TransformationDialog::close);
	
	this->setFixedSize(this->sizeHint());
}

TransformationDialog::~TransformationDialog() {}






void TransformationDialog::set_file_name(QString fileName) {
	this->fileName = std::move(fileName);
	this->setWindowTitle("Transformation - " + this->fileName);
}




void TransformationDialog::user_transform_dialog() {
	if (!this->check_for_any_transformation())
		return;

	QMessageBox questionMessage;
	QPushButton *buttonOverwrite, *buttonCopyOld;
	this->make_message_box(questionMessage, buttonOverwrite, buttonCopyOld);

	questionMessage.exec();
	 
	if (questionMessage.clickedButton() == buttonCopyOld) {
		std::optional<QFileInfo> saveResult = record_serialization::save_to_dialog(this, record, manager);
		if (saveResult.has_value()) {
			if (!saveResult->exists()) {
				QMessageBox::warning(
					this, QStringLiteral("Saving error"),
					QStringLiteral(
						"Could not save record to specified file. Transformation aborted, "
						"record left unchanged")
				);
				return;
			}
		}
	} else if (questionMessage.clickedButton() != buttonOverwrite)
		return;

	this->transform();
	this->accept();
	this->close();
}







bool TransformationDialog::check_for_any_transformation() {
	if (!ui.checkMouseOnly->isChecked()
		&& !ui.checkKeyboardOnly->isChecked()
		&& !ui.checkRemoveWaitTime->isChecked()
		&& (!ui.checkChangeSpeed->isChecked()
			|| (ui.checkChangeSpeed->isChecked()
				&& std::abs(ui.spinBoxSpeed->value() - 1.0) <= std::numeric_limits<double>::epsilon()
				)
			)
		) {
		QMessageBox::information(
			this,
			QStringLiteral("Input Error"),
			QStringLiteral("No transformation has been selected")
		);
		return false;
	}
	return true;
}





void TransformationDialog::make_message_box(QMessageBox &questionMessage, QPushButton *&buttonOverwrite, QPushButton *&buttonCopyOld) {
	questionMessage.setIcon(QMessageBox::Question);
	questionMessage.setWindowTitle(QStringLiteral("Record overwrite"));
	questionMessage.setText(QStringLiteral(
		"Warning - this will overwrite the last record. "
		"What do you want to do?"
	));
	buttonOverwrite = questionMessage.addButton(
		QStringLiteral("Overwrite"),
		QMessageBox::ButtonRole::AcceptRole
	);
	buttonCopyOld = questionMessage.addButton(
		QStringLiteral("Copy old and overwrite"),
		QMessageBox::ButtonRole::AcceptRole
	);
	questionMessage.addButton(QMessageBox::Cancel);
}





void TransformationDialog::set_time(std::chrono::microseconds time) {
	ui.labelTime->setText("Time: <i>" + time_util::single_printable_time(time) + "</i>");
}




void TransformationDialog::set_record_type(InputRecord::RecordType recordType) {
	QString recordTypeStr;
	bool typeConversionEnabled = false;

	switch (recordType) {
	case InputRecord::RecordType::Mouse:
		recordTypeStr = QStringLiteral("Mouse");
		break;
	case InputRecord::RecordType::Keyboard:
		recordTypeStr = QStringLiteral("Keyboard");
		break;
	case InputRecord::RecordType::MouseAndKeyboard:
		typeConversionEnabled = true;
		recordTypeStr = QStringLiteral("Mouse and keyboard");
		break;
	default:
		recordTypeStr = QStringLiteral("None");
	}

	ui.checkMouseOnly->setEnabled(typeConversionEnabled);
	ui.checkKeyboardOnly->setEnabled(typeConversionEnabled);

	ui.labelRecordType->setText("Record type: <i>" + recordTypeStr + "</i>");
}





void TransformationDialog::transform() {
	if (ui.checkMouseOnly->isChecked())
		record.make_mouse_only();
	else if (ui.checkKeyboardOnly->isChecked())
		record.make_keyboard_only();
	if (ui.checkRemoveWaitTime->isChecked())
		record.remove_first_wait_time();
	if (ui.checkChangeSpeed->isChecked())
		record.change_speed(ui.spinBoxSpeed->value());
}
