includeFile(const FileInfo *file, CharsString *includedFile,
		TranslationTableHeader **table, DisplayTableHeader **displayTable) {
	int k;
	char includeThis[MAXSTRING];
	char **tableFiles;
	int rv;
	for (k = 0; k < includedFile->length; k++)
		includeThis[k] = (char)includedFile->chars[k];
	if (k >= MAXSTRING) {
		compileError(file, "Include statement too long: 'include %s'", includeThis);
		return 0;
	}
	includeThis[k] = 0;
	tableFiles = _lou_resolveTable(includeThis, file->fileName);
	if (tableFiles == NULL) {
		errorCount++;
		return 0;
	}
	if (tableFiles[1] != NULL) {
		free_tablefiles(tableFiles);
		compileError(file, "Table list not supported in include statement: 'include %s'",
				includeThis);
		return 0;
	}
	rv = compileFile(*tableFiles, table, displayTable);
	free_tablefiles(tableFiles);
	if (!rv)
		_lou_logMessage(LOU_LOG_ERROR, "%s:%d: Error in included file", file->fileName,
				file->lineNumber);
	return rv;
}