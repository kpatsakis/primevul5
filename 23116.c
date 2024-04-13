compileWarning(const FileInfo *file, const char *format, ...) {
#ifndef __SYMBIAN32__
	char buffer[MAXSTRING];
	va_list arguments;
	va_start(arguments, format);
	vsnprintf(buffer, sizeof(buffer), format, arguments);
	va_end(arguments);
	if (file)
		_lou_logMessage(LOU_LOG_WARN, "%s:%d: warning: %s", file->fileName,
				file->lineNumber, buffer);
	else
		_lou_logMessage(LOU_LOG_WARN, "warning: %s", buffer);
	warningCount++;
#endif
}