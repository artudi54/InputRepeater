#pragma once

#ifdef RECORD_DLL
#define RECORD_EXPORT __declspec(dllexport)
#else
#define RECORD_EXPORT __declspec(dllimport)
#endif



#include "InputRecord.hpp"
#include <thread>
#include <atomic>
#include <string>
#include <map>

//todo: hardware keycodes
class InputRecorder {
#ifdef RECORD_DLL
	friend BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);
#endif
public:
	RECORD_EXPORT static InputRecorder& get_instance();

	RECORD_EXPORT void set_record_type(InputRecord::RecordType type);
	RECORD_EXPORT InputRecord::RecordType get_record_type() const;

	RECORD_EXPORT void set_stop_key(int stopKey);
	RECORD_EXPORT int get_stop_key() const;

	RECORD_EXPORT bool start_recording();
	RECORD_EXPORT void stop_recording();
	RECORD_EXPORT bool is_recording() const;

	RECORD_EXPORT const InputRecord& get_record() const;
	RECORD_EXPORT InputRecord&& retrieve_record();
private:
	RECORD_EXPORT InputRecorder();
	RECORD_EXPORT ~InputRecorder();
	RECORD_EXPORT LRESULT CALLBACK mouse_proc(int code, WPARAM wParam, LPARAM lParam);
	RECORD_EXPORT LRESULT CALLBACK keyboard_proc(int code, WPARAM wParam, LPARAM lParam);
	RECORD_EXPORT void convert_hookinfo_to_inputinfo(std::uintptr_t keyPressType, KBDLLHOOKSTRUCT &keyHook, KEYBDINPUT &keyInput);
	RECORD_EXPORT void convert_hookinfo_to_inputinfo(std::uintptr_t mouseMoveType, MSLLHOOKSTRUCT &mouseHook, MOUSEINPUT  &mouseInput);

	RECORD_EXPORT bool validate_key_press(std::uintptr_t keyPressType, const KBDLLHOOKSTRUCT &keyPress);
	RECORD_EXPORT void update_keymap_info(std::uintptr_t keyPressType, const InputRecord::InputAction& action);
	RECORD_EXPORT void fix_unreleased_keys();


	HINSTANCE instanceHandle;
	HHOOK mouseHook, keyboardHook;
	int stopKey;
	std::chrono::steady_clock::time_point begin;
	InputRecord record;

	std::unordered_map<int, std::size_t> keysPressed; //key-index pair

	RECORD_EXPORT static LRESULT CALLBACK mouse_proc_static(int code, WPARAM wParam, LPARAM lParam);
	RECORD_EXPORT static LRESULT CALLBACK keyboard_proc_static(int code, WPARAM wParam, LPARAM lParam);
	RECORD_EXPORT static void set_module_instance(HINSTANCE instanceHandle);
};

