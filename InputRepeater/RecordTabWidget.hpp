#pragma once

#include "ui_RecordTabWidget.h"
#include "OptionsDialog.hpp"
#include "TransformationDialog.hpp"
#include <InputRecorder.hpp>


class RecordTabWidget : public QWidget {
	Q_OBJECT
public:
	enum class RecordResult {
		Ok,
		SaveFailed,
		NoActionRecorded
	};
	explicit RecordTabWidget(QWidget *parent = nullptr);
	virtual ~RecordTabWidget();

	void set_options(Options &options);
	const InputRecord& get_record() const;
	QString get_title() const;
	bool is_saved() const;
	bool validate_save();
signals:
	void record_started(bool success);
	void record_stopped(RecordResult result);
	void record_updated();
	void record_time_changed(std::chrono::microseconds totalTime);
	void record_unloaded();
	void options_updated();
public slots:
	void start_recording();
	void stop_recording();
private slots:
	void unload_record();
	//dialogs
	void transformation_dialog();
	bool save_as_dialog();
	void load_from_dialog();
private:
	void notifier_procedure();
	void load_from(const QString &filePath);
	RecordResult retrieve_record(InputRecord &&record);

	void connect_signals();
	void clear_record_info();
	void set_record_info(const QFileInfo &fileInfo);
	QString to_string(InputRecord::RecordType recordType);
	void set_input_type();

	Ui::RecordTabWidget ui;
	Options *options;
	InputRecord record;
	RecordManager manager;

	QTimer notifierTimer;
	std::chrono::steady_clock::time_point start;
};
