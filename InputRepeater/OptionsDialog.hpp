#pragma once

#include "ui_OptionsDialog.h"
#include "Options.hpp"


class OptionsDialog : public QDialog {
	Q_OBJECT
public:
	OptionsDialog(const Options &options = Options(), QWidget *parent = nullptr);
	virtual ~OptionsDialog();
	const Options& get_options() const;
private slots:
	void accept_options();
	void to_defaults();
	void open_records_dir();
private:
	void invalid_sequence_prompt(const QString name);

	void add_combo_options();
	void fill_widgets();
	void fill_options();
	Ui::OptionsDialog ui;
	Options options;
};
