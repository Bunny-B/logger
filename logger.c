#include "logger.h"
#include "Windows.h"
#include "stdio.h"
#include "consoleapi.h"

HANDLE RSE_hLoggerFile = INVALID_HANDLE_VALUE;
HANDLE  hConsole;
CRITICAL_SECTION logCriticalSection;  // Critical section for thread safety

HANDLE taskFinishedEvent = NULL; // Event to signal task completion


void* RSE_CreateEvent() {
	return  CreateEvent(NULL, FALSE, FALSE, NULL);
}
void RSE_WaitForLogger(void* taskFinishedEvent) {
	WaitForSingleObject((HANDLE)taskFinishedEvent, INFINITE);
	CloseHandle((HANDLE)taskFinishedEvent);
}


static void InternalLog(const char* format, va_list args) {
	char logMessage[1024];
	vsnprintf(logMessage, sizeof(logMessage), format, args);
	DWORD bytesWritten;
	if (WriteFile(RSE_hLoggerFile, logMessage, (DWORD)strlen(logMessage), &bytesWritten, NULL)) {
		printf("%s", logMessage);
}
	else {
#ifdef _DEBUG
		printf("Failed to write to log file\n");
#endif // _DEBUG
	}
}

const char* RSE_CurrentDir(const char* filename) {
	static char logFilePath[MAX_PATH]; // Buffer for the log file path
	char currentDirectory[MAX_PATH];

	DWORD result = GetCurrentDirectoryA(MAX_PATH, currentDirectory);
	if (result) {
		snprintf(logFilePath, sizeof(logFilePath), "%s\\%s", currentDirectory, filename);
		return logFilePath;
	}
	return NULL; 
}


void RSE_InitLoggerH(void* hLoggerFile, void* taskFinishedEvent) {
	InitializeCriticalSection(&logCriticalSection);
	if (hLoggerFile == INVALID_HANDLE_VALUE) {
#ifdef _DEBUG
		printf("Failed to create log file\n");
#endif // _DEBUG
	}
	else {
#ifdef _DEBUG
		printf("Log file created successfully \n");
#endif // _DEBUG
		RSE_hLoggerFile = hLoggerFile;

		if (!AttachConsole(GetCurrentProcessId()))
			AllocConsole();

		if (hConsole = GetStdHandle(STD_OUTPUT_HANDLE)) {
			SetConsoleTitleW(L"");
			SetConsoleOutputCP(CP_UTF8);
			SetConsoleTextAttribute(hConsole, 7);

			FILE* fpOut;
			if (freopen_s(&fpOut, "CONOUT$", "w", stdout) != 0) {
				MessageBoxW(NULL, L"Failed to redirect stdout to console", L"Error", MB_OK | MB_ICONERROR);
			}

			FILE* fpErr;
			if (freopen_s(&fpErr, "CONOUT$", "w", stderr) != 0) {
				MessageBoxW(NULL, L"Failed to redirect stderr to console", L"Error", MB_OK | MB_ICONERROR);
			}
		}

	}
	if (taskFinishedEvent != NULL)
		SetEvent(taskFinishedEvent);
}

void RSE_InitLoggerA(const char* logPath, void* taskFinishedEvent) {
	RSE_hLoggerFile = CreateFileA(logPath, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
	RSE_InitLoggerH(RSE_hLoggerFile, taskFinishedEvent);
}


void RSE_WLog(void* format, ...) {
	wchar_t logMessage[1024];
	memset(logMessage, 0, sizeof(logMessage));

	// Prepare the wide character log message
	va_list args;
	va_start(args, format);
	_vsnwprintf_s(logMessage, _countof(logMessage), _TRUNCATE, format, args);
	va_end(args);

	// Convert the wide log message to UTF-8
	char utf8Message[2048];  // UTF-8 can take more space than wide characters
	int utf8Size = WideCharToMultiByte(CP_UTF8, 0, logMessage, -1, utf8Message, sizeof(utf8Message) - 1, NULL, NULL);

	if (utf8Size > 0) {
		// Ensure null-termination
		utf8Message[utf8Size - 1] = '\0';

		EnterCriticalSection(&logCriticalSection);

		// Write the UTF-8 log message to the log file
		DWORD bytesWritten;
		if (WriteFile(RSE_hLoggerFile, utf8Message, (DWORD)strlen(utf8Message), &bytesWritten, NULL)) {
			// Also write to the console
			wprintf(L"%ls", logMessage);
		}
		else {
#ifdef _DEBUG
			printf("Failed to write to log file\n");
#endif
		}

		LeaveCriticalSection(&logCriticalSection);
	}
}

 void RSE_Log(const char* format, ...) {
	 va_list args;
	 va_start(args, format);

	 EnterCriticalSection(&logCriticalSection);
	 InternalLog(format, args);
	 LeaveCriticalSection(&logCriticalSection);

	 va_end(args);
 }

 void RSE_ErrorLog(const char* format, ...) {
	 SetConsoleTextAttribute(hConsole, 12);
	 va_list args;
	 va_start(args, format);

	 EnterCriticalSection(&logCriticalSection);
	 InternalLog(format, args);
	 LeaveCriticalSection(&logCriticalSection);

	 va_end(args);
	 SetConsoleTextAttribute(hConsole, 7);
 }

 void RSE_CloseLogger() {
	 CloseHandle(RSE_hLoggerFile);
	 DeleteCriticalSection(&logCriticalSection);
 }