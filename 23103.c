compileString(const char *inString, TranslationTableHeader **table,
		DisplayTableHeader **displayTable) {
	/* This function can be used to make changes to tables on the fly. */
	int k;
	FileInfo file;
	if (inString == NULL) return 0;
	memset(&file, 0, sizeof(file));
	file.fileName = inString;
	file.encoding = noEncoding;
	file.lineNumber = 1;
	file.status = 0;
	file.linepos = 0;
	for (k = 0; inString[k]; k++) file.line[k] = inString[k];
	file.line[k] = 0;
	file.linelen = k;
	if (table && *table && (*table)->finalized) {
		compileError(&file, "Table is finalized");
		return 0;
	}
	return compileRule(&file, table, displayTable, NULL);
}