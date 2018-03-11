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
	if (options.play_sound())
		this->single_play(QUrl(QStringLiteral("qrc:/Resources/StartSound.aif")));
	ui.statusBar->set_status(QStringLiteral("Recording"), QStringLiteral("red"));

	recordStartHotkey.setRegistered(false);
	replayStartHotkey.setRegistered(false);
	recordStopHotkey.setRegistered(true);
	replayStopHotkey.setRegistered(false);

	ui.replayTabWidget->setEnabled(false);
}

void InputRepeater::record_stopped(RecordTabWidget::RecordResult result) {
	if (options.play_sound())
		this->single_play(QUrl(QStringLiteral("qrc:/Resources/StopSound.aif")));
	ui.statusBar->set_status(QStringLiteral("Recording finished"), QStringLiteral("green"));
	
	recordStartHotkey.setRegistered(true);
	replayStartHotkey.setRegistered(true);
	recordStopHotkey.setRegistered(false);
	replayStopHotkey.setRegistered(false);

	ui.replayTabWidget->setEnabled(true);

	this->record_updated();
	if (result == RecordTabWidget::RecordResult::SaveFailed)
		QMessageBox::warning(
			this,
			QStringLiteral("Saving error"),
			QStringLiteral("Could not save record to specified file")
		);
	else if (result == RecordTabWidget::RecordResult::NoActionRecorded)
		QMessageBox::information(
			this,
			QStringLiteral("Recording failed"),
			QStringLiteral("No action was recorded. No record created")
		);
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
	if (options.play_sound())
		this->single_play(QUrl(QStringLiteral("qrc:/Resources/StartSound.aif")));
	ui.statusBar->set_status(QStringLiteral("Replaying"), QStringLiteral("red"));
	recordStartHotkey.setRegistered(false);
	replayStartHotkey.setRegistered(false);
	recordStopHotkey.setRegistered(false);
	replayStopHotkey.setRegistered(true);
}

void InputRepeater::replay_stopped() {
	if (options.play_sound())
		this->single_play(QUrl(QStringLiteral("qrc:/Resources/StopSound.aif")));
	ui.statusBar->set_status(QStringLiteral("Replaying finished"), QStringLiteral("green"));
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
		recordStopHotkey.setShortcut(
			options.record_stop_hotkey() - VK_F1 + Qt::Key_F1,
			false
		);
	else
		recordStopHotkey.setShortcut(
			options.record_stop_hotkey() - VK_F1 + Qt::Key_F1,
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

void InputRepeater::single_play(const QUrl & media) {
	QMediaPlayer *player = new QMediaPlayer(this);
	player->setAudioRole(QAudio::MusicRole);
	player->setMedia(QMediaContent(media));

	QObject::connect(
		player, &QMediaPlayer::stateChanged, player,
		[player](QMediaPlayer::State state) {
			if (state == QMediaPlayer::StoppedState) player->deleteLater();
	});
	player->play();
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

	QObject::connect(&recordStartHotkey, &QHotkey::activated, this, &InputRepeater::auto_disable_hotkey);
	QObject::connect(&recordStopHotkey, &QHotkey::activated, this, &InputRepeater::auto_disable_hotkey);
	QObject::connect(&replayStartHotkey, &QHotkey::activated, this, &InputRepeater::auto_disable_hotkey);
	QObject::connect(&replayStopHotkey, &QHotkey::activated, this, &InputRepeater::auto_disable_hotkey);

	QObject::connect(&recordStartHotkey, &QHotkey::activated, ui.recordTabWidget, [tab = ui.recordTabWidget]{ QTimer::singleShot(QApplication::keyboardInputInterval(), tab, &RecordTabWidget::start_recording); });
	QObject::connect(&recordStopHotkey, &QHotkey::activated, ui.recordTabWidget, &RecordTabWidget::stop_recording);
	QObject::connect(&replayStartHotkey, &QHotkey::activated, ui.replayTabWidget, &ReplayTabWidget::start_playing);
	QObject::connect(&replayStopHotkey, &QHotkey::activated, ui.replayTabWidget, &ReplayTabWidget::stop_playing);
}


const QString InputRepeater::OPTIONS_FILE = QStringLiteral("Config.ini");