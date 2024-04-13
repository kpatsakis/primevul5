lou_readCharFromFile(const char *fileName, int *mode) {
	/* Read a character from a file, whether big-endian, little-endian or
	 * ASCII8 */
	int ch;
	static FileInfo file;
	if (fileName == NULL) return 0;
	if (*mode == 1) {
		*mode = 0;
		file.fileName = fileName;
		file.encoding = noEncoding;
		file.status = 0;
		file.lineNumber = 0;
		if (!(file.in = fopen(file.fileName, "r"))) {
			_lou_logMessage(LOU_LOG_ERROR, "Cannot open file '%s'", file.fileName);
			*mode = 1;
			return EOF;
		}
	}
	if (file.in == NULL) {
		*mode = 1;
		return EOF;
	}
	ch = getAChar(&file);
	if (ch == EOF) {
		fclose(file.in);
		file.in = NULL;
		*mode = 1;
	}
	return ch;
}