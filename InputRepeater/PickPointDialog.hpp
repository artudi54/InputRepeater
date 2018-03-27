#pragma once

#include <QDialog>

class PickPointDialog : public QDialog
{
	Q_OBJECT

public:
	PickPointDialog(QObject *parent);
	~PickPointDialog();
};
