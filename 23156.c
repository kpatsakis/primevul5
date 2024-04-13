getToken(FileInfo *file, CharsString *result, const char *description) {
	/* Find the next string of contiguous non-whitespace characters. If this
	 * is the last token on the line, return 2 instead of 1. */
	while (!atEndOfLine(file) && atTokenDelimiter(file)) file->linepos++;
	result->length = 0;
	while (!atEndOfLine(file) && !atTokenDelimiter(file)) {
		int maxlen = MAXSTRING;
		if (result->length >= maxlen) {
			compileError(file, "more than %d characters (bytes)", maxlen);
			return 0;
		} else
			result->chars[result->length++] = file->line[file->linepos++];
	}
	if (!result->length) {
		/* Not enough tokens */
		if (description) compileError(file, "%s not specified.", description);
		return 0;
	}
	result->chars[result->length] = 0;
	while (!atEndOfLine(file) && atTokenDelimiter(file)) file->linepos++;
	return 1;
}