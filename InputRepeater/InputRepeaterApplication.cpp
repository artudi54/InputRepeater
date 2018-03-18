#include "stdafx.h"
#include "InputRepeaterApplication.hpp"

InputRepeaterApplication::InputRepeaterApplication(int &argc, char **argv)
	: QApplication(argc, argv)
	, oneInstanceMemory(QStringLiteral("InputRepeater_OneInstanceMemory"), this) {
	oneInstanceMemory.create(1);
}

InputRepeaterApplication::~InputRepeaterApplication() {}

bool InputRepeaterApplication::is_only_instance() const {
	if (oneInstanceMemory.isAttached())
		return true;

	return oneInstanceMemory.create(1);
}

void InputRepeaterApplication::set_program_path() {
	QString workingDirectory = QDir::currentPath();
	QString programDirectory = QApplication::applicationDirPath();
	if (workingDirectory != programDirectory)
		QDir::setCurrent(programDirectory);
}
