#include "stdafx.h"
#include "RecordTabWidget.hpp"
#include "record_serialization.hpp"
namespace chrono = std::chrono;
using namespace std::literals;

RecordTabWidget::RecordTabWidget(QWidget *parent)
	: QWidget(parent)
	, options(nullptr)
	, record()
	, manager(QStringLiteral("Records"), QStringLiteral("record_"))
	, notifierTimer(this)
	, start() {

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
	const QString &filePath = options.last_loaded_path();
	if (options.open_last_loaded() && !filePath.isEmpty() && QFileInfo::exists(filePath))
		this->load_from(options.last_loaded_path());
}

const InputRecord & RecordTabWidget::get_record() const {
	return record;
}

QString RecordTabWidget::get_title() const {
	return ui.editFilename->text();
}

bool RecordTabWidget::is_saved() const {
	return !ui.editFilename->text().isEmpty();
}




bool RecordTabWidget::validate_save() {
	if (!record.empty() && !this->is_saved()) {
		int button = QMessageBox::question(
			this, QStringLiteral("Unsaved Record"),
			QStringLiteral("You have unsaved record. Do you want to save it?"),
			QMessageBox::Yes | QMessageBox::No
		);
		if (button == QMessageBox::Yes)
			return this->save_as_dialog();
		if (button == QMessageBox::No)
			return true;
		return false;
	}
	return true;
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
	
	this->set_input_type();
	ui.labelRecordStatus->setText(QStringLiteral("Recording"));
	ui.labelType->setText(this->to_string(InputRecord::RecordType::None));
	if (recorder.start_recording()) {
		start = chrono::steady_clock::now();
		notifierTimer.start((20ms).count());
		emit record_started(true);
		emit record_time_changed(0us);
	} else {
		ui.labelRecordStatus->setText(QStringLiteral("Recording failed"));
		emit record_started(false);
	}
}



void RecordTabWidget::stop_recording() {
	RecordResult result;
	InputRecorder &recorder = InputRecorder::get_instance();
	recorder.stop_recording();

	if (!recorder.get_record().empty()) {
		result = this->retrieve_record(recorder.retrieve_record());
		ui.buttonUnload->setEnabled(true);
		ui.buttonSaveAs->setEnabled(true);
		ui.buttonTransform->setEnabled(true);
	} else {
		result = RecordResult::NoActionRecorded;
		ui.labelRecordStatus->setText(QStringLiteral("Unloaded"));
		options->set_last_loaded_path(QString());
	}


	ui.buttonRecord->setEnabled(true);
	ui.buttonStop->setEnabled(false);
	ui.buttonLoadFrom->setEnabled(true);

	notifierTimer.stop();
	emit record_time_changed(record.total_time());
	emit record_stopped(result);
}










void RecordTabWidget::unload_record() {
	ui.buttonUnload->setEnabled(false);
	ui.buttonSaveAs->setEnabled(false);
	ui.buttonTransform->setEnabled(false);
	this->clear_record_info();
	ui.labelType->setText(this->to_string(InputRecord::RecordType::None));
	ui.labelRecordStatus->setText(QStringLiteral("Unloaded"));
	record.clear();
	emit record_unloaded();
}










void RecordTabWidget::transformation_dialog() {
	TransformationDialog transformationDialog(record, this);
	if (!this->is_saved())
		transformationDialog.set_file_name(this->get_title());
	if (transformationDialog.exec() == QDialog::Accepted) {
		this->clear_record_info();
		ui.labelRecordStatus->setText(QStringLiteral("In memory"));
		ui.labelType->setText(this->to_string(record.get_record_type()));
		emit record_updated();
		emit record_time_changed(record.total_time());
	}
}

bool RecordTabWidget::save_as_dialog() {
	std::optional<QFileInfo> saveFileInfo = record_serialization::save_to_dialog(this, record, manager);
	if (saveFileInfo) {
		if (saveFileInfo->exists()) {
			this->set_record_info(*saveFileInfo);
			options->set_last_loaded_path(saveFileInfo->absoluteFilePath());
			ui.labelRecordStatus->setText(QStringLiteral("Saved to file"));
			emit record_updated();
			emit record_time_changed(record.total_time());
			return true;
		}
		QMessageBox::warning(
			this,
			QStringLiteral("Saving error"),
			QStringLiteral("Could not save record to specified file")
		);
	}
	return false;
}

void RecordTabWidget::load_from_dialog() {
	QFileDialog dialog;
	dialog.setDirectory(QStringLiteral("Records"));
	dialog.setNameFilters({
		QStringLiteral("Record file (*.rec)"),
		QStringLiteral("All files (*)")
	});
	if (dialog.exec() == QDialog::Accepted)
		this->load_from(dialog.selectedFiles().front());
}











void RecordTabWidget::notifier_procedure() {
	emit record_time_changed(chrono::duration_cast<chrono::microseconds>(
		chrono::steady_clock::now() - start
	));
}




void RecordTabWidget::load_from(const QString & filePath) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(
			this, QStringLiteral("Loading error"),
			"Could not open \"" + file.fileName() + "\" for reading"
		);
		return;
	}
	QDataStream stream(&file);
	InputRecord record;
	stream >> record;
	if (stream.status() != QDataStream::Ok) {
		QMessageBox::warning(
			this, QStringLiteral("Loading error"),
			"File \"" + file.fileName() + "\" does not contain valid input record"
		);
		return;
	}
	this->record = record;
	this->set_record_info(QFileInfo(filePath));
	ui.labelRecordStatus->setText(QStringLiteral("Saved to file"));
	ui.labelType->setText(this->to_string(record.get_record_type()));
	options->set_last_loaded_path(filePath);

	ui.buttonUnload->setEnabled(true);
	ui.buttonSaveAs->setEnabled(true);
	ui.buttonTransform->setEnabled(true);

	emit record_updated();
	emit record_time_changed(record.total_time());
}




RecordTabWidget::RecordResult RecordTabWidget::retrieve_record(InputRecord && _record) {
	RecordResult result = RecordResult::Ok;

	record = std::move(_record);
	if (options->remove_first_wait_time())
		record.remove_first_wait_time();

	if (options->auto_save_new_records() && manager.is_valid()) {
		QFileInfo newPathInfo = manager.get_new_save_path();

		if (record_serialization::save_to_file(record, newPathInfo.filePath())) {
			this->set_record_info(newPathInfo);
			options->set_last_loaded_path(newPathInfo.absoluteFilePath());
			ui.labelRecordStatus->setText(QStringLiteral("Saved to file"));
		} else {
			result = RecordResult::SaveFailed;
			ui.labelRecordStatus->setText(QStringLiteral("In memory"));
			options->set_last_loaded_path(QString());
		}

	} else {
		ui.labelRecordStatus->setText(QStringLiteral("In memory"));
		options->set_last_loaded_path(QString());
	}

	ui.labelType->setText(this->to_string(record.get_record_type()));

	return result;
}








void RecordTabWidget::connect_signals() {
	QObject::connect(ui.buttonRecord, &QPushButton::clicked, this, &RecordTabWidget::start_recording);
	QObject::connect(ui.buttonStop, &QPushButton::clicked, this, &RecordTabWidget::stop_recording);
	QObject::connect(ui.buttonUnload, &QPushButton::clicked, this, &RecordTabWidget::unload_record);

	QObject::connect(ui.buttonTransform, &QPushButton::clicked, this, &RecordTabWidget::transformation_dialog);
	QObject::connect(ui.buttonSaveAs, &QPushButton::clicked, this, &RecordTabWidget::save_as_dialog);
	QObject::connect(ui.buttonLoadFrom, &QPushButton::clicked, this, &RecordTabWidget::load_from_dialog);

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
