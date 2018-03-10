#include "stdafx.h"
#include "RecordTabWidget.hpp"
namespace chrono = std::chrono;
using namespace std::literals;

RecordTabWidget::RecordTabWidget(QWidget *parent)
	: QWidget(parent)
	, options(nullptr)
	, record()
	, manager(QStringLiteral("Records"), QStringLiteral("record_"))
	, notifierTimer(this)
	, start()
	, optionsDialog(nullptr)
	, autoClickerDialog(nullptr) {
	ui.setupUi(this);
	this->connect_signals();
	if (!manager.is_valid())
		QMessageBox::warning(
			this,
			QStringLiteral("Record directory error"),
			QStringLiteral(
				"Record directory does not exist and could not have been created. "
				"Records can not be saved automatically"
			)
		);
}

RecordTabWidget::~RecordTabWidget() {}






void RecordTabWidget::set_options(Options & options) {
	this->options = &options;
}

const InputRecord & RecordTabWidget::get_record() const {
	return record;
}

QString RecordTabWidget::get_title() const {
	return ui.editFilename->text();
}





void RecordTabWidget::start_recording() {
	InputRecorder &recorder = InputRecorder::get_instance();
	this->clear_record_info();
	record.clear();
	ui.buttonRecord->setEnabled(false);
	ui.buttonStop->setEnabled(true);
	ui.buttonUnload->setEnabled(false);
	ui.buttonSaveAs->setEnabled(false);
	ui.buttonLoadFrom->setEnabled(false);
	ui.buttonTransform->setEnabled(false);
	ui.buttonAutoClicker->setEnabled(false);
	
	this->set_input_type();
	ui.labelRecordStatus->setText(QStringLiteral("Recording"));
	ui.labelType->setText(this->to_string(InputRecord::RecordType::None));
	recorder.start_recording();

	start = chrono::steady_clock::now();
	notifierTimer.start((20ms).count());

	emit record_started();
	emit record_time_changed(0us);
}

void RecordTabWidget::stop_recording() {
	InputRecorder &recorder = InputRecorder::get_instance();
	recorder.stop_recording();

	if (!recorder.get_record().empty()) {
		record = std::move(recorder.retrieve_record());
		if (options->auto_save_new_records() && manager.is_valid()) {
			QFileInfo newPathInfo = manager.get_new_save_path();
			if (record_serialization::save_to_file(record, newPathInfo.filePath())) {
				this->set_record_info(newPathInfo);
				ui.labelRecordStatus->setText(QStringLiteral("Saved to file"));
			} else {
				QMessageBox::warning(
					this,
					QStringLiteral("Saving error"),
					QStringLiteral("Could not save record to specified file")
				);
				ui.labelRecordStatus->setText(QStringLiteral("In memory"));
			}
		} else
			ui.labelRecordStatus->setText(QStringLiteral("In memory"));
		ui.labelType->setText(this->to_string(record.get_record_type()));
		ui.buttonUnload->setEnabled(true);
		ui.buttonSaveAs->setEnabled(true);
		ui.buttonTransform->setEnabled(true);
		ui.buttonAutoClicker->setEnabled(true);
	} 
	
	
	else {
		ui.labelRecordStatus->setText(QStringLiteral("Unloaded"));
		QMessageBox::information(
			this,
			QStringLiteral("Recording failed"),
			QStringLiteral("No action was recorded. No record created")
		);
	}


	ui.buttonRecord->setEnabled(true);
	ui.buttonStop->setEnabled(false);
	ui.buttonLoadFrom->setEnabled(true);

	notifierTimer.stop();
	emit record_time_changed(record.total_time());
	emit record_stopped();
}






void RecordTabWidget::unload_record() {
	ui.buttonUnload->setEnabled(false);
	ui.buttonSaveAs->setEnabled(false);
	ui.buttonTransform->setEnabled(false);
	ui.buttonAutoClicker->setEnabled(true);
	this->clear_record_info();
	ui.labelType->setText(this->to_string(InputRecord::RecordType::None));
	ui.labelRecordStatus->setText(QStringLiteral("Unloaded"));
	record.clear();
	emit record_unloaded();
}









void RecordTabWidget::options_dialog() {
	if (optionsDialog != nullptr) {
		optionsDialog->show();
		optionsDialog->raise();
		optionsDialog->activateWindow();
		return;
	}
	optionsDialog = new OptionsDialog(*options, this);
	optionsDialog->setAttribute(Qt::WA_DeleteOnClose);

	QObject::connect(
		optionsDialog, &QDialog::accepted, this,
		[this] { 
			*options = optionsDialog->get_options();
			InputRecorder::get_instance().set_stop_key(options->record_stop_hotkey());
			emit options_updated();
		}
	);

	QObject::connect(
		optionsDialog, &QObject::destroyed, this,
		[this] { optionsDialog = nullptr; }
	);

	optionsDialog->show();
}

void RecordTabWidget::auto_clicker_dialog() {
	//todo: autoclicker dialog
	qDebug() << "pls implement me";
}

void RecordTabWidget::transformation_dialog() {
	TransformationDialog transformationDialog(record, this);
	QString fileName = ui.editFilename->text();
	if (!fileName.isEmpty())
		transformationDialog.set_file_name(fileName);
	if (transformationDialog.exec() == QDialog::Accepted) {
		emit record_updated();
		emit record_time_changed(record.total_time());
	}
}

void RecordTabWidget::save_as_dialog() {
	std::optional<QFileInfo> saveFileInfo = record_serialization::save_to_dialog(this, record, manager);
	if (saveFileInfo) {
		if (saveFileInfo->exists())
			this->set_record_info(*saveFileInfo);
		else
			QMessageBox::warning(
				this,
				QStringLiteral("Saving error"),
				QStringLiteral("Could not save record to specified file")
			);
	}
}

void RecordTabWidget::notifier_procedure() {
	emit record_time_changed(chrono::duration_cast<chrono::microseconds>(
		chrono::steady_clock::now() - start
	));
}








void RecordTabWidget::connect_signals() {
	QObject::connect(ui.buttonRecord, &QPushButton::clicked, this, &RecordTabWidget::start_recording);
	QObject::connect(ui.buttonStop, &QPushButton::clicked, this, &RecordTabWidget::stop_recording);
	QObject::connect(ui.buttonUnload, &QPushButton::clicked, this, &RecordTabWidget::unload_record);

	QObject::connect(ui.buttonOptions, &QPushButton::clicked, this, &RecordTabWidget::options_dialog);
	QObject::connect(ui.buttonAutoClicker, &QPushButton::clicked, this, &RecordTabWidget::auto_clicker_dialog);
	QObject::connect(ui.buttonTransform, &QPushButton::clicked, this, &RecordTabWidget::transformation_dialog);
	QObject::connect(ui.buttonSaveAs, &QPushButton::clicked, this, &RecordTabWidget::save_as_dialog);

	QObject::connect(&notifierTimer, &QTimer::timeout, this, &RecordTabWidget::notifier_procedure);
}





void RecordTabWidget::clear_record_info() {
	ui.editFilename->clear();
	ui.editFullPath->clear();
}

void RecordTabWidget::set_record_info(const QFileInfo & savePath) {
	ui.editFilename->setText(savePath.fileName());
	ui.editFullPath->setText(savePath.absoluteFilePath());
}





QString RecordTabWidget::to_string(InputRecord::RecordType recordType) {
	switch (recordType) {
	case InputRecord::RecordType::None:
		return QStringLiteral("None");
	case InputRecord::RecordType::Mouse:
		return QStringLiteral("Mouse");
	case InputRecord::RecordType::Keyboard:
		return QStringLiteral("Keyboard");
	case InputRecord::RecordType::MouseAndKeyboard:
		return QStringLiteral("Mouse and keyboard");
	default:
		return QString();
	}
}




void RecordTabWidget::set_input_type() {
	InputRecorder &recorder = InputRecorder::get_instance();
	if (ui.radioKeyboard->isChecked())
		recorder.set_record_type(InputRecord::RecordType::Keyboard);
	else if (ui.radioMouse->isChecked())
		recorder.set_record_type(InputRecord::RecordType::Mouse);
	else
		recorder.set_record_type(InputRecord::RecordType::MouseAndKeyboard);
}
