#include "InputRecorder.hpp"
#include <algorithm>
namespace chrono = std::chrono;
using namespace std::literals;







InputRecorder & InputRecorder::get_instance() noexcept {
	static InputRecorder recorder;
	return recorder;
}





void InputRecorder::set_record_type(InputRecord::RecordType type) noexcept {
	if (this->is_recording())
		this->stop_recording();
	record.clear();
	record.set_record_type(type);
}

InputRecord::RecordType InputRecorder::get_record_type() const noexcept {
	return record.get_record_type();
}






void InputRecorder::set_stop_key(unsigned stopKey) noexcept {
	this->stopKey = stopKey;
}

unsigned InputRecorder::get_stop_key() const noexcept {
	return stopKey;
}






bool InputRecorder::start_recording() noexcept {
	bool success = true;
	this->stop_recording();
	record.clear();
	timeBegin = chrono::steady_clock::now();

	if ((int)record.get_record_type() & (int)InputRecord::RecordType::Keyboard) {
		keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, &InputRecorder::keyboard_proc_static, instanceHandle, 0);
		success &= keyboardHook != nullptr;
	}
	if ((int)record.get_record_type() & (int)InputRecord::RecordType::Mouse) {
		mouseHook = SetWindowsHookExW(WH_MOUSE_LL, &InputRecorder::mouse_proc_static, instanceHandle, 0);
		success &= mouseHook != nullptr;
	}

	if (!success) {
		this->stop_recording();
		return false;
	}
	return true;
}


void InputRecorder::stop_recording() noexcept {
	if (mouseHook != nullptr) {
		::UnhookWindowsHookEx(mouseHook);
		mouseHook = nullptr;
	}
	if (keyboardHook != nullptr) {
		::UnhookWindowsHookEx(keyboardHook);
		keyboardHook = nullptr;
		this->fix_unreleased_keys();
	}
}


bool InputRecorder::is_recording() const noexcept {
	return mouseHook != nullptr || keyboardHook != nullptr;
}






const InputRecord& InputRecorder::get_record() const noexcept {
	return record;
}

InputRecord&& InputRecorder::retrieve_record() noexcept {
	return std::move(record);
}





InputRecorder::InputRecorder() noexcept
	: instanceHandle(::GetModuleHandleW(nullptr))
	, mouseHook(nullptr)
	, keyboardHook(nullptr)
	, stopKey(0)
	, timeBegin()
	, record(InputRecord::RecordType::None) {}


InputRecorder::~InputRecorder() {}








LRESULT  InputRecorder::mouse_proc(int code, WPARAM wParam, LPARAM lParam) {
	if (code == HC_ACTION) {
		chrono::steady_clock::time_point current = chrono::steady_clock::now();
		MSLLHOOKSTRUCT &mouseAction = *reinterpret_cast<LPMSLLHOOKSTRUCT>(lParam);
		INPUT input;
		input.type = INPUT_MOUSE;
		this->convert_hookinfo_to_inputinfo(wParam, mouseAction, input.mi);
		record.add(
			chrono::duration_cast<chrono::microseconds>(current - timeBegin),
			input
		);
		timeBegin = current;
	}
	return CallNextHookEx(nullptr, code, wParam, lParam);
}



LRESULT InputRecorder::keyboard_proc(int code, WPARAM wParam, LPARAM lParam) {
	if (code == HC_ACTION) {
		chrono::steady_clock::time_point current = chrono::steady_clock::now();
		KBDLLHOOKSTRUCT &keyPress = *reinterpret_cast<LPKBDLLHOOKSTRUCT>(lParam);
		if (this->validate_key_press(wParam, keyPress)) {
			INPUT input;
			input.type = INPUT_KEYBOARD;
			this->convert_hookinfo_to_inputinfo(wParam, keyPress, input.ki);
			InputRecord::InputAction& action = record.add(
				chrono::duration_cast<chrono::microseconds>(current - timeBegin),
				input
			);
			this->update_keymap_info(wParam, action);
			timeBegin = current;
		} else
			this->stop_recording();
	}
	return CallNextHookEx(nullptr, code, wParam, lParam);
}






void InputRecorder::convert_hookinfo_to_inputinfo(std::uintptr_t keyPressType, KBDLLHOOKSTRUCT &keyHook, KEYBDINPUT &keyInput) noexcept {
	keyInput.time = 0;
	keyInput.dwExtraInfo = 0;

	keyInput.wVk = static_cast<WORD>(keyHook.vkCode);
	keyInput.wScan = static_cast<WORD>(keyHook.scanCode);

	keyInput.dwFlags = 0;
	if (keyInput.wScan != 0)
		keyInput.dwFlags |= KEYEVENTF_SCANCODE;
	if (keyPressType == WM_KEYUP || keyPressType == WM_SYSKEYUP)
		keyInput.dwFlags |= KEYEVENTF_KEYUP;
	if (keyHook.flags & LLKHF_EXTENDED)
		keyInput.dwFlags |= KEYEVENTF_EXTENDEDKEY;
}



void InputRecorder::convert_hookinfo_to_inputinfo(std::uintptr_t mouseMoveType, MSLLHOOKSTRUCT &mouseHook, MOUSEINPUT &mouseInput) noexcept {
	static const int RES_X = ::GetSystemMetrics(SM_CXSCREEN), RES_Y = GetSystemMetrics(SM_CYSCREEN);
	static const int MAX_COORD = 65535;
	
	mouseInput.time = 0;
	mouseInput.dwExtraInfo = 0;

	mouseInput.dx = mouseHook.pt.x * MAX_COORD / RES_X;
	mouseInput.dy = mouseHook.pt.y * MAX_COORD / RES_Y;
	mouseInput.mouseData = HIWORD(mouseInput.mouseData);
	mouseInput.dwFlags = MOUSEEVENTF_ABSOLUTE;

	switch (mouseMoveType) {
	case WM_MOUSEMOVE:
		mouseInput.dwFlags |= MOUSEEVENTF_MOVE;
		break;

	case WM_MOUSEHWHEEL:
		mouseInput.dwFlags |= MOUSEEVENTF_HWHEEL;
		break;
	case WM_MOUSEWHEEL:
		mouseInput.dwFlags |= MOUSEEVENTF_WHEEL;
		break;

	case WM_LBUTTONDOWN:
		mouseInput.dwFlags |= MOUSEEVENTF_LEFTDOWN;
		break;
	case WM_RBUTTONDOWN:
		mouseInput.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
		break;
	case WM_LBUTTONUP:
		mouseInput.dwFlags |= MOUSEEVENTF_LEFTUP;
		break;
	case WM_RBUTTONUP:
		mouseInput.dwFlags |= MOUSEEVENTF_RIGHTUP;
		break;

	case WM_MBUTTONDOWN:
		mouseInput.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
		break;
	case WM_MBUTTONUP:
		mouseInput.dwFlags |= MOUSEEVENTF_MIDDLEUP;
		break;

	case WM_XBUTTONDOWN:
	case WM_NCXBUTTONDOWN:
		mouseInput.dwFlags |= MOUSEEVENTF_XUP;
		break;
	case WM_XBUTTONUP:
	case WM_NCXBUTTONUP:
		mouseInput.dwFlags |= MOUSEEVENTF_XUP;
		break;
	}
}




bool InputRecorder::validate_key_press(std::uintptr_t keyPressType, const KBDLLHOOKSTRUCT &keyPress) {
	if (keyPress.vkCode == stopKey) {
		if (keysPressed.empty() && keyPressType != WM_KEYUP)
			return false;
		if (keysPressed.size() == 1) {
			auto it = keysPressed.find(stopKey);
			if (it != keysPressed.end() && keyPressType != WM_KEYUP)
				return false;
			return true;
		}
		return true;
	}
	return true;
}

void InputRecorder::update_keymap_info(std::uintptr_t keyPressType, const InputRecord::InputAction & action) {
	if (keyPressType == WM_KEYUP || keyPressType == WM_SYSKEYUP)
		keysPressed.erase(action.input.ki.wVk);
	else
		keysPressed[action.input.ki.wVk] = record.size() - 1;
}

void InputRecorder::fix_unreleased_keys() {
	for (auto &keyPair : keysPressed)
		record.add(record.get_actions()[keyPair.second])
			  .input.ki.dwFlags |= KEYEVENTF_KEYUP;
}











LRESULT InputRecorder::mouse_proc_static(int code, WPARAM wParam, LPARAM lParam) {
	return InputRecorder::get_instance().mouse_proc(code, wParam, lParam);
}

LRESULT InputRecorder::keyboard_proc_static(int code, WPARAM wParam, LPARAM lParam) {
	return InputRecorder::get_instance().keyboard_proc(code, wParam, lParam);
}

void InputRecorder::set_module_instance(HINSTANCE instanceHandle) {
	InputRecorder::get_instance().instanceHandle = instanceHandle;
}