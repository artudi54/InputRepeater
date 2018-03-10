#include "stdafx.h"
#include "TransformationDialog.hpp"
#include "record_serialization.hpp"
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
	this->setFixedSize(this->size());
}

TransformationDialog::~TransformationDialog() {}

void TransformationDialog::set_file_name(QString fileName) {
	this->fileName = std::move(fileName);
	this->setWindowTitle("Transformation - " + this->fileName);
}




void TransformationDialog::user_transform_dialog() {
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
		return;
	}

	QMessageBox questionMessage;
	questionMessage.setIcon(QMessageBox::Question);
	questionMessage.setWindowTitle(QStringLiteral("Record overwrite"));
	questionMessage.setText(QStringLiteral(
		"Warning - this will overwrite the last record. "
		"What do you want to do?"
	));
	QPushButton *buttonOverwrite = questionMessage.addButton(
		QStringLiteral("Overwrite"),
		QMessageBox::ButtonRole::AcceptRole
	);
	QPushButton *buttonCopyOld = questionMessage.addButton(
		QStringLiteral("Copy old and overwrite"),
		QMessageBox::ButtonRole::AcceptRole
	);
	questionMessage.addButton(QMessageBox::Cancel);

	questionMessage.exec();
	 
	if (questionMessage.clickedButton() == buttonOverwrite) {
		this->transform();
		this->accept();
		this->close();
	} else if (questionMessage.clickedButton() == buttonCopyOld) {
		std::optional<QFileInfo> saveResult = record_serialization::save_to_dialog(this, record, manager);
		if (saveResult.has_value()) {
			if (!saveResult->exists()) {
				QMessageBox::warning(
					this,
					QStringLiteral("Saving error"),
					QStringLiteral(
						"Could not save record to specified file. Transformation aborted, "
						"record left unchanged"
					)
				);
				return;
			}
			this->transform();
			this->accept();
			this->close();
		}
	}
}







void TransformationDialog::set_time(std::chrono::microseconds time) {
	auto minutes = chrono::duration_cast<chrono::minutes>(time);
	auto seconds = chrono::duration_cast<chrono::seconds>(time - minutes);
	QString strTime = QString(QStringLiteral("%1:%2")).arg(minutes.count(), 2, 10, QChar('0'))
													  .arg(seconds.count(), 2, 10, QChar('0'));
	ui.labelTime->setText("Time: <i>" + strTime + "</i>");
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
}





void TransformationDialog::transform() {
	if (ui.checkMouseOnly->isChecked())
		record.make_mouse_only();
	else if (ui.checkKeyboardOnly->isChecked())
		record.make_keyboard_only();
	if (ui.checkRemoveWaitTime->isChecked())
		record.remove_wait_time_before_first_action();
	if (ui.checkChangeSpeed->isChecked())
		record.change_speed(ui.spinBoxSpeed->value());
}
