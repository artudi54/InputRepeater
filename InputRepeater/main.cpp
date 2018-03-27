#include "stdafx.h"
#include "InputRepeaterApplication.hpp"
#include "InputRepeater.hpp"

int main(int argc, char *argv[]) {

	InputRepeaterApplication application(argc, argv);
	InputRepeaterApplication::setWindowIcon(QIcon(":/Resources/ProgramIcon.ico"));
	if (!application.is_only_instance())
		return EXIT_SUCCESS;
	application.set_program_path();
	InputRepeater window;
	window.show();
	return application.exec();
}
