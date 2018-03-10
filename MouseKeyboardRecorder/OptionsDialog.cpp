#include "stdafx.h"
#include "OptionsDialog.hpp"


OptionsDialog::OptionsDialog(const Options &options, QWidget *parent)
	: QDialog(parent)
	, options(options) {
	ui.setupUi(this);

	QObject::connect(ui.buttonOK, &QPushButton::clicked, this, &OptionsDialog::accept_options);
	QObject::connect(ui.buttonCancel, &QPushButton::clicked, this, &OptionsDialog::close);
	QObject::connect(ui.buttonDefaults, &QPushButton::clicked, this, &OptionsDialog::to_defaults);
	this->add_combo_options();
	this->fill_widgets();
	this->setFixedSize(this->size());
}

OptionsDialog::~OptionsDialog() {}

const Options & OptionsDialog::get_options() const {
	return options;
}





void OptionsDialog::accept_options() {
	if (ui.keyStartRecord->keySequence().count() != 1) {
		this->invalid_sequence_prompt(QStringLiteral("Start record"));
		return;
	}
	if (ui.keyStartReplay->keySequence().count() != 1) {
		this->invalid_sequence_prompt(QStringLiteral("Start replay"));
		return;
	}
	if (ui.keyStopReplay->keySequence().count() != 1) {
		this->invalid_sequence_prompt(QStringLiteral("End replay"));
		return;
	}

	if (ui.keyStartRecord->keySequence() == ui.keyStartReplay->keySequence()) {
		QMessageBox::information(
			this,
			QStringLiteral("Invalid Sequence"),
			QStringLiteral("Start hotkeys can not be the same")
		);
		return;
	}
	this->fill_options();
	this->accept();
}


void OptionsDialog::to_defaults() {
	options = Options::default_options();
	this->fill_widgets();
}



void OptionsDialog::invalid_sequence_prompt(const QString name) {
	QMessageBox::information(
		this,
		QStringLiteral("Invalid Sequence"),
		name + " hotkey is not a single shortcut"
	);
}

void OptionsDialog::add_combo_options() {
	for (unsigned i = 0; i < 12; ++i)
		ui.comboStopRecord->addItem("F" + QString::number(i + 1), unsigned(VK_F1 + i));
}



void OptionsDialog::fill_widgets() {
	ui.checkShowPopupMessages->setChecked(options.show_popup_messages());
	ui.checkPlaySound->setChecked(options.play_sound());
	ui.checkHideToTray->setChecked(options.hide_to_tray());
	ui.checkOpenLastLoaded->setChecked(options.open_last_loaded());

	ui.checkRemoveFirstWaitTime->setChecked(options.remove_first_wait_time());
	ui.checkAutoSaveNewRecords->setChecked(options.auto_save_new_records());

	ui.keyStartRecord->setKeySequence(options.record_start_hotkey());
	ui.comboStopRecord->setCurrentIndex(options.record_stop_hotkey() - VK_F1);
	ui.keyStartReplay->setKeySequence(options.replay_start_hotkey());
	ui.keyStopReplay->setKeySequence(options.replay_stop_hotkey());
}

void OptionsDialog::fill_options() {
	options.set_show_popup_messages(ui.checkShowPopupMessages->isChecked());
	options.set_play_sound(ui.checkPlaySound->isChecked());
	options.set_hide_to_tray(ui.checkHideToTray->isChecked());
	options.set_open_last_loaded(ui.checkOpenLastLoaded->isChecked());

	options.set_remove_first_wait_time(ui.checkRemoveFirstWaitTime->isChecked());
	options.set_auto_save_new_records(ui.checkAutoSaveNewRecords->isChecked());

	options.set_record_start_hotkey(ui.keyStartRecord->keySequence());
	options.set_record_stop_hotey(ui.comboStopRecord->currentData().toUInt());
	options.set_replay_start_hotkey(ui.keyStartReplay->keySequence());
	options.set_replay_stop_hotkey(ui.keyStopReplay->keySequence());
}
