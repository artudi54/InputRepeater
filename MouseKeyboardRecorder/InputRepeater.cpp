#include "stdafx.h"
#include "InputRepeater.hpp"
namespace chrono = std::chrono;

InputRepeater::InputRepeater(QWidget *parent)
	: QMainWindow(parent){
	ui.setupUi(this);
	this->load_options();
	this->connect_signals();
}

void InputRepeater::closeEvent(QCloseEvent * event) {
	if (!options.save_to(OPTIONS_FILE)) {
		int button = QMessageBox::warning(
			this,
			QStringLiteral("Config Error"),
			QStringLiteral(
				"Could not save options to config file. "
				"Do you want to close the application?"
			),
			QMessageBox::Yes | QMessageBox::No
		);
		if (button != QMessageBox::Yes) {
			event->ignore();
			return;
		}
	}
	QMainWindow::closeEvent(event);
}

void InputRepeater::record_started() {
	qDebug() << "record";
	ui.statusBar->set_status(QStringLiteral("Recording"), QStringLiteral("red"));

	recordStartHotkey.setRegistered(false);
	replayStartHotkey.setRegistered(false);
	recordStopHotkey.setRegistered(true);
	replayStopHotkey.setRegistered(false);

	ui.replayTabWidget->setEnabled(false);
}

void InputRepeater::record_stopped() {
	qDebug() << "stop record";
	ui.statusBar->set_status(QStringLiteral("Recording finished"), QStringLiteral("green"));
	
	recordStartHotkey.setRegistered(true);
	replayStartHotkey.setRegistered(true);
	recordStopHotkey.setRegistered(false);
	replayStopHotkey.setRegistered(false);

	ui.replayTabWidget->setEnabled(true);

	this->record_updated();
}

void InputRepeater::record_updated() {
	ui.replayTabWidget->set_record(ui.recordTabWidget->get_record());
	ui.replayTabWidget->set_title(ui.recordTabWidget->get_title());
}

void InputRepeater::record_unloaded() {
	ui.statusBar->set_status(QStringLiteral("Unloaded"));
	ui.statusBar->reset_time();
	this->record_updated();
}

void InputRepeater::replay_started() {
	qDebug() << "replay";
	recordStartHotkey.setRegistered(false);
	replayStartHotkey.setRegistered(false);
	recordStopHotkey.setRegistered(false);
	replayStopHotkey.setRegistered(true);
}

void InputRepeater::replay_stopped() {
	qDebug() << "stop replay";
	recordStartHotkey.setRegistered(true);
	replayStartHotkey.setRegistered(true);
	recordStopHotkey.setRegistered(false);
	replayStopHotkey.setRegistered(false);
}

void InputRepeater::auto_disable_hotkey() {
	QHotkey *hotkey = dynamic_cast<QHotkey*>(this->sender());
	if (hotkey != nullptr)
		hotkey->setRegistered(false);
}

void InputRepeater::hotkey_record_start() {
	auto_disable_hotkey();
	QTimer::singleShot(QApplication::keyboardInputInterval(), [this] {ui.recordTabWidget->start_recording(); });
	//ui.recordTabWidget->start_recording();
}

void InputRepeater::hotkey_record_stop() {
	auto_disable_hotkey();
	ui.recordTabWidget->stop_recording();

}

void InputRepeater::hothey_replay_start() {
	auto_disable_hotkey();
	ui.replayTabWidget->start_playing();
}

void InputRepeater::hotkey_replay_stop() {
	auto_disable_hotkey();
	ui.replayTabWidget->stop_playing();
}

void InputRepeater::set_hotkeys() {
	if (recordStartHotkey.shortcut().isEmpty())
		recordStartHotkey.setShortcut(
			options.record_start_hotkey(), true
		);
	else
		recordStartHotkey.setShortcut(
			options.record_start_hotkey(),
			recordStartHotkey.isRegistered()
		);


	if (recordStopHotkey.shortcut().isEmpty())
		recordStopHotkey.setNativeShortcut(
			QHotkey::NativeShortcut(options.record_stop_hotkey()),
			false
		);
	else
		recordStopHotkey.setNativeShortcut(
			QHotkey::NativeShortcut(options.record_stop_hotkey()),
			recordStopHotkey.isRegistered()
		);


	if (replayStartHotkey.shortcut().isEmpty())
		replayStartHotkey.setShortcut(
			options.replay_start_hotkey(), true
		);
	else
		replayStartHotkey.setShortcut(
			options.replay_start_hotkey(), replayStartHotkey.isRegistered()
		);


	if (replayStopHotkey.shortcut().isEmpty())
		replayStopHotkey.setShortcut(
			options.replay_stop_hotkey(), false
		);
	else
		replayStopHotkey.setShortcut(
			options.replay_stop_hotkey(),
			replayStopHotkey.isRegistered()
		);
}

void InputRepeater::load_options() {
	std::optional<Options> optOptions = Options::load_from(OPTIONS_FILE);
	if (optOptions.has_value())
		options = *optOptions;
	else
		QMessageBox::warning(
			this,
			QStringLiteral("Config Error"),
			QStringLiteral("Could not load options from config file. Defaults will be used")
		);
	ui.recordTabWidget->set_options(options);
	ui.replayTabWidget->set_options(options);
	this->set_hotkeys();
	InputRecorder::get_instance().set_stop_key(options.record_stop_hotkey());
}

void InputRepeater::connect_signals() {
	QObject::connect(ui.recordTabWidget, &RecordTabWidget::options_updated, this, &InputRepeater::set_hotkeys);

	QObject::connect(ui.recordTabWidget, &RecordTabWidget::record_started, this, &InputRepeater::record_started);
	QObject::connect(ui.recordTabWidget, &RecordTabWidget::record_stopped, this, &InputRepeater::record_stopped);
	QObject::connect(ui.recordTabWidget, &RecordTabWidget::record_updated, this, &InputRepeater::record_updated);
	QObject::connect(ui.recordTabWidget, &RecordTabWidget::record_unloaded, this, &InputRepeater::record_unloaded);
	
	QObject::connect(ui.replayTabWidget, &ReplayTabWidget::replay_started, this, &InputRepeater::replay_started);
	QObject::connect(ui.replayTabWidget, &ReplayTabWidget::replay_stopped, this, &InputRepeater::replay_stopped);

	QObject::connect(ui.recordTabWidget, &RecordTabWidget::record_time_changed, ui.statusBar, QOverload<chrono::microseconds>::of(&InputRepeaterStatusBar::set_time));
	QObject::connect(ui.replayTabWidget, &ReplayTabWidget::replay_time_changed, ui.statusBar, QOverload<chrono::microseconds, chrono::microseconds>::of(&InputRepeaterStatusBar::set_time));

	QObject::connect(&recordStartHotkey, &QHotkey::activated, this, &InputRepeater::hotkey_record_start);
	QObject::connect(&recordStopHotkey, &QHotkey::activated, this, &InputRepeater::hotkey_record_stop);
	QObject::connect(&replayStartHotkey, &QHotkey::activated, this, &InputRepeater::hothey_replay_start);
	QObject::connect(&replayStopHotkey, &QHotkey::activated, this, &InputRepeater::hotkey_replay_stop);
}


const QString InputRepeater::OPTIONS_FILE = QStringLiteral("Config.ini");