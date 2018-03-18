#pragma once

#include "ui_InputRepeater.h"
#include <QHotkey>



class InputRepeater : public QMainWindow {
	Q_OBJECT
public:
	explicit InputRepeater(QWidget *parent = nullptr);
	virtual ~InputRepeater();
protected:
	virtual void closeEvent(QCloseEvent *event) override;
private slots:
	void set_hotkeys();
	void record_started(bool success);
	void record_stopped(RecordTabWidget::RecordResult result);
	void record_updated();
	void record_unloaded();
	void replay_started();
	void replay_stopped();

	void auto_disable_hotkey();
	void options_dialog();
private:
	void update_player();

	void user_interact_start_record();
	void user_interact_stop_record();
	void user_interact_start_replay();
	void user_interact_stop_replay();

	void single_play(const QUrl &media);
	void load_options();
	void connect_signals();

	Ui::InputRepeater ui;
	QSystemTrayIcon trayIcon;
	Options options;
	OptionsDialog *optionsDialog;
	QHotkey recordStartHotkey, recordStopHotkey;
	QHotkey replayStartHotkey, replayStopHotkey;
	static const QString OPTIONS_FILE;
};
