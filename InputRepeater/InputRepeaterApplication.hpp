#pragma once

class InputRepeaterApplication : public QApplication {
	Q_OBJECT
public:
	InputRepeaterApplication(int &argc, char **argv);
	virtual ~InputRepeaterApplication();
	bool is_only_instance() const;
	void set_program_path();
private:
	mutable QSharedMemory oneInstanceMemory;
};
