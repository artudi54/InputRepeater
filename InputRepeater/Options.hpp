#pragma once


class Options {
public:
	Options();

	Options(const Options&) = default;
	Options& operator=(const Options&) = default;

	bool show_popup_messages() const noexcept;
	bool play_sound() const noexcept;

	bool hide_to_tray() const noexcept;
	bool open_last_loaded() const noexcept;

	bool remove_first_wait_time() const noexcept;
	bool auto_save_new_records() const noexcept;

	const QString& last_loaded_path() const noexcept;

	QKeySequence record_start_hotkey() const;
	unsigned record_stop_hotkey() const noexcept;

	QKeySequence replay_start_hotkey() const;
	QKeySequence replay_stop_hotkey() const;


	void set_show_popup_messages(bool showPopupMessages) noexcept;
	void set_play_sound(bool playSound) noexcept;


	void set_hide_to_tray(bool hideToTray) noexcept;
	void set_open_last_loaded(bool openLastLoaded) noexcept;

	void set_remove_first_wait_time(bool removeFirstWaitTime) noexcept;
	void set_auto_save_new_records(bool autoSautoSaveNewRecordsave) noexcept;

	void set_last_loaded_path(const QString &lastLoadedPath);

	void set_record_start_hotkey(const QKeySequence &recordStartHotkey);
	void set_record_stop_hotey(unsigned recorStopHotkey) noexcept;

	void set_replay_start_hotkey(const QKeySequence &replayEndHotkey);
	void set_replay_stop_hotkey(const QKeySequence &replayStopHotkey);

	static Options default_options();
	static std::optional<Options> load_from(const QString &path);
	bool save_to(const QString &path);
private:

	Options(bool showPopupMessages, bool playSound, bool hideToTray, bool openLastLoaded,
		bool removeFirstWaitTime, bool autoSaveNewRecords, const QString &lastLoadedPath,
		const QKeySequence &recordStartHotkey,
		unsigned recordStopHotkey, const QKeySequence &replayStartHotkey,
		const QKeySequence &replayStopHotkey
	);

	bool showPopupMessages, playSound;
	bool hideToTray, openLastLoaded;
	bool removeFirstWaitTime, autoSaveNewRecords;
	QString lastLoadedPath;
	QKeySequence recordStartHotkey;
	unsigned recordStopHotkey;
	QKeySequence replayStartHotkey, replayStopHotkey;

	static const QString SHOW_POPUP_MESSAGES;
	static const QString PLAY_SOUND;
	static const QString HIDE_TO_TRAY;
	static const QString OPEN_LAST_LOADED;
	static const QString REMOVE_FIRST_WAIT_TIME;
	static const QString AUTO_SAVE_NEW_RECORDS;
	static const QString LAST_LOADED_PATH;
	static const QString RECORD_START_HOTKEY;
	static const QString RECORD_STOP_HOTKEY;
	static const QString REPLAY_START_HOTKEY;
	static const QString REPLAY_STOP_HOTKEY;
};