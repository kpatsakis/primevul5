compileFile(const char *fileName, TranslationTableHeader **table,
		DisplayTableHeader **displayTable) {
	FileInfo file;
	fileCount++;
	file.fileName = fileName;
	if (table) {
		int i;
		for (i = 0; (*table)->sourceFiles[i]; i++)
			;
		if (i >= MAX_SOURCE_FILES) {
			_lou_logMessage(LOU_LOG_WARN, "Max number of source files (%i) reached",
					MAX_SOURCE_FILES);
			file.sourceFile = NULL;
		} else {
			file.sourceFile = (*table)->sourceFiles[i] = strdup(fileName);
		}
	}
	file.encoding = noEncoding;
	file.status = 0;
	file.lineNumber = 0;
	if ((file.in = fopen(file.fileName, "rb"))) {
		// the scope of a macro is the current file (after the macro definition)
		const MacroList *inscopeMacros = NULL;
		while (_lou_getALine(&file))
			if (!compileRule(&file, table, displayTable, &inscopeMacros)) {
				if (!errorCount) compileError(&file, "Rule could not be compiled");
				break;
			}
		fclose(file.in);
		free_macro_list(inscopeMacros);
	} else {
		_lou_logMessage(LOU_LOG_ERROR, "Cannot open table '%s'", file.fileName);
		errorCount++;
	}
	return !errorCount;
}