#include "stdafx.h"
#include "Options.hpp"

Options::Options()
	: Options(Options::default_options()) {
}



bool Options::show_popup_messages() const noexcept {
	return showPopupMessages;
}

bool Options::play_sound() const noexcept {
	return playSound;
}

bool Options::hide_to_tray() const noexcept {
	return hideToTray;
}

bool Options::open_last_loaded() const noexcept {
	return openLastLoaded;
}

bool Options::remove_first_wait_time() const noexcept {
	return removeFirstWaitTime;
}

bool Options::auto_save_new_records() const noexcept {
	return autoSaveNewRecords;
}

const QString & Options::last_loaded_path() const noexcept {
	return lastLoadedPath;
}

QKeySequence Options::record_start_hotkey() const {
	return recordStartHotkey;
}

unsigned Options::record_stop_hotkey() const noexcept {
	return recordStopHotkey;
}

QKeySequence Options::replay_start_hotkey() const {
	return replayStartHotkey;
}

QKeySequence Options::replay_stop_hotkey() const {
	return replayStopHotkey;
}





void Options::set_show_popup_messages(bool showPopupMessages) noexcept {
	this->showPopupMessages = showPopupMessages;
}

void Options::set_play_sound(bool playSound) noexcept {
	this->playSound = playSound;
}

void Options::set_hide_to_tray(bool hideToTray) noexcept {
	this->hideToTray = hideToTray;
}

void Options::set_open_last_loaded(bool openLastLoaded) noexcept {
	this->openLastLoaded = openLastLoaded;
}

void Options::set_remove_first_wait_time(bool removeFirstWaitTime) noexcept {
	this->removeFirstWaitTime = removeFirstWaitTime;
}

void Options::set_auto_save_new_records(bool autoSaveNewRecords) noexcept {
	this->autoSaveNewRecords = autoSaveNewRecords;
}

void Options::set_last_loaded_path(const QString & lastLoadedPath) {
	this->lastLoadedPath = lastLoadedPath;
}

void Options::set_record_start_hotkey(const QKeySequence &recordStartHotkey) {
	this->recordStartHotkey = recordStartHotkey;
}

void Options::set_record_stop_hotey(unsigned recorStopHotkey) noexcept {
	this->recordStopHotkey = recorStopHotkey;
}

void Options::set_replay_start_hotkey(const QKeySequence &replayStartHotkey) {
	this->replayStartHotkey = replayStartHotkey;
}

void Options::set_replay_stop_hotkey(const QKeySequence &replayStopHotkey) {
	this->replayStopHotkey = replayStopHotkey;
}










Options Options::default_options() {
	return Options(
		true, true,
		true, true,
		false, false,
		QString(),
		QKeySequence(Qt::Key_F5),
		VK_F6,
		QKeySequence(Qt::Key_F7),
		QKeySequence(Qt::Key_F8)
	);
}






std::optional<Options> Options::load_from(const QString &path) {
	if (!QFileInfo::exists(path))
		return std::optional<Options>();
	QSettings optionsFile(path, QSettings::IniFormat);
	Options options;

	options.showPopupMessages = optionsFile
		.value(SHOW_POPUP_MESSAGES, options.showPopupMessages)
		.value<bool>();

	options.playSound = optionsFile
		.value(PLAY_SOUND, options.playSound)
		.value<bool>();

	options.hideToTray = optionsFile
		.value(HIDE_TO_TRAY, options.hideToTray)
		.value<bool>();

	options.openLastLoaded = optionsFile
		.value(OPEN_LAST_LOADED, options.openLastLoaded)
		.value<bool>();

	options.removeFirstWaitTime = optionsFile
		.value(REMOVE_FIRST_WAIT_TIME, options.removeFirstWaitTime)
		.value<bool>();

	options.autoSaveNewRecords = optionsFile
		.value(AUTO_SAVE_NEW_RECORDS, options.autoSaveNewRecords)
		.value<bool>();

	options.lastLoadedPath = optionsFile
		.value(LAST_LOADED_PATH, options.lastLoadedPath)
		.value<QString>();

	options.recordStartHotkey = optionsFile
		.value(RECORD_START_HOTKEY, options.recordStartHotkey)
		.value<QKeySequence>();

	options.recordStopHotkey = optionsFile
		.value(RECORD_STOP_HOTKEY, options.recordStopHotkey)
		.value<unsigned>();

	options.replayStartHotkey = optionsFile
		.value(REPLAY_START_HOTKEY, options.replayStartHotkey)
		.value<QKeySequence>();

	options.replayStopHotkey = optionsFile
		.value(REPLAY_STOP_HOTKEY, options.replayStopHotkey)
		.value<QKeySequence>();

	return options;
}


bool Options::save_to(const QString & path) {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	file.close();

	QSettings optionsFile(path, QSettings::IniFormat);

	optionsFile.setValue(SHOW_POPUP_MESSAGES, showPopupMessages);
	optionsFile.setValue(PLAY_SOUND, playSound);
	optionsFile.setValue(HIDE_TO_TRAY, hideToTray);
	optionsFile.setValue(OPEN_LAST_LOADED, openLastLoaded);
	optionsFile.setValue(REMOVE_FIRST_WAIT_TIME, removeFirstWaitTime);
	optionsFile.setValue(AUTO_SAVE_NEW_RECORDS, autoSaveNewRecords);
	optionsFile.setValue(LAST_LOADED_PATH, lastLoadedPath);
	optionsFile.setValue(RECORD_START_HOTKEY, recordStartHotkey);
	optionsFile.setValue(RECORD_STOP_HOTKEY, recordStopHotkey);
	optionsFile.setValue(REPLAY_START_HOTKEY, replayStartHotkey);
	optionsFile.setValue(REPLAY_STOP_HOTKEY, replayStopHotkey);

	return true;
}







Options::Options(bool showPopupMessages, bool playSound, bool hideToTray,
	bool openLastLoaded, bool removeFirstWaitTime, bool autoSaveNewRecords,
	const QString &lastLoadedPath,
	const QKeySequence &recordStartHotkey, unsigned recordStopHotkey,
	const QKeySequence &replayStartHotkey, const QKeySequence &replayStopHotkey)
	: showPopupMessages(showPopupMessages)
	, playSound(playSound)
	, hideToTray(hideToTray)
	, openLastLoaded(openLastLoaded)
	, removeFirstWaitTime(removeFirstWaitTime)
	, autoSaveNewRecords(autoSaveNewRecords)
	, lastLoadedPath(lastLoadedPath)
	, recordStartHotkey(recordStartHotkey)
	, recordStopHotkey(recordStopHotkey)
	, replayStartHotkey(replayStartHotkey)
	, replayStopHotkey(replayStopHotkey) {
}






const QString Options::SHOW_POPUP_MESSAGES = QStringLiteral("CONFIG/ShowPopupMessages");
const QString Options::PLAY_SOUND = QStringLiteral("CONFIG/PlaySound");
const QString Options::HIDE_TO_TRAY = QStringLiteral("CONFIG/HideToTray");
const QString Options::OPEN_LAST_LOADED = QStringLiteral("CONFIG/OpenLastLoaded");
const QString Options::REMOVE_FIRST_WAIT_TIME = QStringLiteral("CONFIG/RemoveFirstWaitTime");
const QString Options::AUTO_SAVE_NEW_RECORDS = QStringLiteral("CONFIG/AutoSaveNewRecords");
const QString Options::LAST_LOADED_PATH = QStringLiteral("CONFIG/LastLoadedPath");
const QString Options::RECORD_START_HOTKEY = QStringLiteral("CONFIG/RecordStartHotkey");
const QString Options::RECORD_STOP_HOTKEY = QStringLiteral("CONFIG/RecordStopHotkey");
const QString Options::REPLAY_START_HOTKEY = QStringLiteral("CONFIG/ReplayStartHotkey");
const QString Options::REPLAY_STOP_HOTKEY = QStringLiteral("CONFIG/ReplayStopHotkey");









