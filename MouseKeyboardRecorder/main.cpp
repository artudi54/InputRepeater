#include "stdafx.h"
#include "InputRepeater.hpp"

//todo: review noexcept
int main(int argc, char *argv[]) {
	QApplication application(argc, argv);
	InputRepeater window;
	window.show();
	return application.exec();
}
