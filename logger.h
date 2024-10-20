#ifndef LOGGER_H
#define LOGGER_H

/*
	Safe Thread Logger
	https://github.com/Bunny-B/logger
*/

#ifdef __cplusplus
extern "C" {
#endif

	extern void RSE_InitLoggerA(const char* logPath, void* taskFinishedEvent);
	#define InitLogger RSE_InitLoggerA

	extern void RSE_InitLoggerH(void* hLoggerFile, void* taskFinishedEvent);
	#define InitLoggerH RSE_InitLoggerH

	extern void* RSE_CreateEvent();

	extern void RSE_WaitForLogger(void* taskFinishedEvent);
	#define WaitForLogger RSE_WaitForLogger

	extern void RSE_Log(const char* format, ...);
	#define Log RSE_Log

	extern void RSE_WLog(void* format, ...);
	#define WLog RSE_WLog

	extern void RSE_ErrorLog(const char* format, ...);
	#define ErrorLog RSE_ErrorLog

	extern void RSE_CloseLogger();
	#define CloseLogger RSE_ErrorLog

#ifdef __cplusplus
}
#endif



#endif // !LOGGER_H

