#pragma once

#include <cassert> // for assert
#include <crtdbg.h> // for _ASSERTE
#include <windows.h> // for HANDLE OutputDebugString
#include <stdio.h> // for printf && _vsnwprintf_s
/*
	cassert is useful because it is part of c++, it works on windows linux mac etc
	_ASSERTE works only on Windows, but provides much more information on the dialog box
*/

#define WIN32

// This is the best way to define ASSERT
// Upper case to indicate that it is a Macro
// and use _ASSERTE on Windows
// and use assert on other platforms
#ifdef  WIN32
#define ASSERT _ASSERTE
#else
#define ASSERT assert
#endif //  WIN32
/*
	VERIFY Macro, very useful for debugging
	on Debug version it works on ASSERT, if there is an error
	it is reportes
	on release version, just the expression is kept
*/
#ifdef _DEBUG
#define VERIFY ASSERT
#else
#define VERIFY(expression) (expression)
#endif

struct LastException {
	int error;
	LastException() :error(GetLastError()) {}
};

/*
	TRACE Macro
*/

struct Tracer {
	Tracer() = delete;
	const wchar_t * m_file;
	int m_line;
	Tracer(const wchar_t *file, int line) :m_file(file), m_line(line) {}
	
	template<typename ...Args>
	void operator()(const wchar_t *format, Args ...args) {
		wchar_t buffer[256];
		auto count = swprintf_s(buffer, _countof(buffer), L"%s(%d) ", m_file, m_line);
		ASSERT(count != -1);
		ASSERT(-1 != _snwprintf_s(buffer + count,
			_countof(buffer)-count -1,
			_countof(buffer)-count,
			format, 
			args...));
		OutputDebugString(buffer);
	}
};
#ifdef _DEBUG
#define WIDE2(x) L##x
#define WIDE(x) WIDE2(x)
#define TRACE Tracer(WIDE(__FILE__), __LINE__)
#else
#define	TRACE __noop
#endif // _DEBUG



struct ManualResetEvent {
	HANDLE m_handle = INVALID_HANDLE_VALUE;
	ManualResetEvent() {
		m_handle = CreateEvent(nullptr, // Security description
			true, // manual reset event
			false, // not signal
			nullptr); // no name
		if (!m_handle) {
			throw LastException();
		}
	}
	~ManualResetEvent() {
		VERIFY(CloseHandle(m_handle));
	}
};