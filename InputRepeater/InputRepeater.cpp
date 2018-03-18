#include "stdafx.h"
#include "InputRepeater.hpp"
namespace chrono = std::chrono;

InputRepeater::InputRepeater(QWidget *parent)
	: QMainWindow(parent)
	, trayIcon(QApplication::windowIcon(), this)
	, options()
	, optionsDialog(nullptr)
	, recordStartHotkey(this)
	, recordStopHotkey(this)
	, replayStartHotkey(this)
	, replayStopHotkey(this) {
	ui.setupUi(this);
	this->connect_signals();
	this->load_options();
	trayIcon.setToolTip(this->windowTitle());
	trayIcon.show();

	this->setFixedSize(this->sizeHint());
}


InputRepeater::~InputRepeater() {}










void InputRepeater::closeEvent(QCloseEvent * event) {
	if (!ui.recordTabWidget->validate_save()) {
		event->ignore();
		return;
	}
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





void InputRepeater::set_hotkeys() {
	std::function<bool(const QHotkey&)> reg;
	if (recordStartHotkey.shortcut().isEmpty())
		reg = [this](const QHotkey &hotkey) {
		if (&hotkey == &recordStartHotkey || &hotkey == &replayStartHotkey)
			return true;
		return false;
	};
	else
		reg = [](const QHotkey &hotkey) { return hotkey.isRegistered(); };

	recordStartHotkey.setShortcut(options.record_start_hotkey(), reg(recordStartHotkey));
	recordStopHotkey.setShortcut(
		QKeySequence(options.record_stop_hotkey() - VK_F1 + Qt::Key_F1),
		reg(recordStopHotkey)
	);
	replayStartHotkey.setShortcut(options.replay_start_hotkey(), reg(replayStartHotkey));
	replayStopHotkey.setShortcut(options.replay_stop_hotkey(), reg(replayStopHotkey));
}







void InputRepeater::record_started(bool success) {
	if (success) {
		this->user_interact_start_record();

		recordStartHotkey.setRegistered(false);
		replayStartHotkey.setRegistered(false);
		recordStopHotkey.setRegistered(true);
		replayStopHotkey.setRegistered(false);

		ui.replayTabWidget->setEnabled(false);
	} else {
		recordStartHotkey.setRegistered(true);
		replayStartHotkey.setRegistered(true);
		recordStopHotkey.setRegistered(false);
		replayStopHotkey.setRegistered(false);

		ui.replayTabWidget->setEnabled(true);
		this->update_player();
		ui.statusBar->set_status(QStringLiteral("Recording failed"), QStringLiteral("evilred"));
		QMessageBox::warning(
			this, QStringLiteral("Recording failed"),
			QStringLiteral("Could not start recording. Previous record is unloaded. Please try again later")
		);
	}
}



void InputRepeater::record_stopped(RecordTabWidget::RecordResult result) {
	this->user_interact_stop_record();

	recordStartHotkey.setRegistered(true);
	replayStartHotkey.setRegistered(true);
	recordStopHotkey.setRegistered(false);
	replayStopHotkey.setRegistered(false);

	ui.replayTabWidget->setEnabled(true);

	this->update_player();
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
	ui.statusBar->set_status(QStringLiteral("Record loaded"));
	this->update_player();
}



void InputRepeater::record_unloaded() {
	ui.statusBar->set_status(QStringLiteral("Unloaded"));
	ui.statusBar->reset_time();
	this->update_player();
}


















void InputRepeater::replay_started() {
	this->user_interact_start_replay();

	recordStartHotkey.setRegistered(false);
	replayStartHotkey.setRegistered(false);
	recordStopHotkey.setRegistered(false);
	replayStopHotkey.setRegistered(true);
}




void InputRepeater::replay_stopped() {
	this->user_interact_stop_replay();

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













void InputRepeater::options_dialog() {
	if (optionsDialog != nullptr) {
		optionsDialog->show();
		optionsDialog->raise();
		optionsDialog->activateWindow();
		return;
	}
	optionsDialog = new OptionsDialog(options, this);
	optionsDialog->setAttribute(Qt::WA_DeleteOnClose);

	QObject::connect(
		optionsDialog, &QDialog::accepted, this,
		[this] {
			options = optionsDialog->get_options();
			InputRecorder::get_instance().set_stop_key(options.record_stop_hotkey());
			this->set_hotkeys();
		}
	);

	QObject::connect(
		optionsDialog, &QObject::destroyed, this,
		[this] { optionsDialog = nullptr; }
	);

	optionsDialog->show();
}












void InputRepeater::update_player() {
	ui.replayTabWidget->set_record(ui.recordTabWidget->get_record());
	ui.replayTabWidget->set_title(ui.recordTabWidget->get_title());
}









void InputRepeater::user_interact_start_record() {
	ui.statusBar->set_status(QStringLiteral("Recording"), QStringLiteral("red"));
	if (options.play_sound())
		this->single_play(QUrl(QStringLiteral("qrc:/Resources/StartSound.aif")));
	if (options.show_popup_messages())
		trayIcon.showMessage(
			QStringLiteral("Record Started"),
			"Recording has started. Press \""
				+ recordStopHotkey.shortcut().toString()
				+ "\" to stop recording"
		);
	if (options.hide_to_tray())
		this->hide();
}



void InputRepeater::user_interact_stop_record() {
	ui.statusBar->set_status(QStringLiteral("Recording finished"), QStringLiteral("green"));
	if (options.play_sound())
		this->single_play(QUrl(QStringLiteral("qrc:/Resources/StopSound.aif")));
	if (options.show_popup_messages())
		trayIcon.showMessage(
			QStringLiteral("Record Stopped"),
			QStringLiteral("Recoring has finished")
		);
	this->show();
}



void InputRepeater::user_interact_start_replay() {
	ui.statusBar->set_status(QStringLiteral("Replaying"), QStringLiteral("red"));
	if (options.play_sound())
		this->single_play(QUrl(QStringLiteral("qrc:/Resources/StartSound.aif")));
	if (options.show_popup_messages())
		trayIcon.showMessage(
			QStringLiteral("Replay Started"),
			"Replaying has started. Press \""
			+ replayStopHotkey.shortcut().toString()
			+ "\" to stop playing"
		);
	if (options.hide_to_tray())
		this->hide();
}



void InputRepeater::user_interact_stop_replay() {
	ui.statusBar->set_status(QStringLiteral("Replaying finished"), QStringLiteral("green"));
	if (options.play_sound())
		this->single_play(QUrl(QStringLiteral("qrc:/Resources/StopSound.aif")));
	if (options.show_popup_messages())
		trayIcon.showMessage(
			QStringLiteral("Replay Stopped"),
			QStringLiteral("Replaying has finished")
		);
	this->show();
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
	QObject::connect(ui.actionOptions, &QAction::triggered, this, &InputRepeater::options_dialog);
	QObject::connect(ui.actionExit, &QAction::triggered, this, &QWidget::close);

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

	QObject::connect(&recordStartHotkey, &QHotkey::registeredChanged, ui.actionRecord, &QAction::setEnabled);
	QObject::connect(&replayStartHotkey, &QHotkey::registeredChanged, ui.actionReplay, &QAction::setEnabled);
	QObject::connect(ui.actionRecord, &QAction::triggered, ui.recordTabWidget, &RecordTabWidget::start_recording);
	QObject::connect(ui.actionReplay, &QAction::triggered, ui.replayTabWidget, &ReplayTabWidget::start_playing);
}






const QString InputRepeater::OPTIONS_FILE = QStringLiteral("Config.ini");