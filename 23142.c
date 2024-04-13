printSource(const FileInfo *currentFile, const char *sourceFile, int sourceLine) {
	static char scratchBuf[MAXSTRING];
	if (sourceFile) {
		if (currentFile && currentFile->sourceFile &&
				strcmp(currentFile->sourceFile, sourceFile) == 0)
			snprintf(scratchBuf, MAXSTRING, "line %d", sourceLine);
		else
			snprintf(scratchBuf, MAXSTRING, "%s:%d", sourceFile, sourceLine);
	} else
		snprintf(scratchBuf, MAXSTRING, "source unknown");
	return scratchBuf;
}