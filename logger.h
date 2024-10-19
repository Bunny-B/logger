#ifndef LOGGER_H
#define LOGGER_H

/*
	Safe Thread Logger
*/

#ifdef __cplusplus
extern "C" {
#endif


	void RSE_InitLoggerA(const char* logPath);
	#define InitLogger RSE_InitLoggerA

	void RSE_InitLoggerH(void* hLoggerFile);
	#define InitLoggerH RSE_InitLoggerH

	void RSE_Log(const char* format, ...);
	#define Log RSE_Log

	void RSE_WLog(void* format, ...);
	#define WLog RSE_WLog

	void RSE_ErrorLog(const char* format, ...);
	#define ErrorLog RSE_ErrorLog

	void RSE_CloseLogger();
	#define CloseLogger RSE_ErrorLog

#ifdef __cplusplus
}
#endif



#endif // !LOGGER_H

