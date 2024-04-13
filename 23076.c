passGetString(CharsString *passLine, int *passLinepos, CharsString *string,
		const FileInfo *file) {
	string->length = 0;
	while (1) {
		if ((*passLinepos >= passLine->length) || !passLine->chars[*passLinepos]) {
			compileError(file, "unterminated string");
			return 0;
		}
		if (passLine->chars[*passLinepos] == 34) break;
		if (passLine->chars[*passLinepos] == QUOTESUB)
			string->chars[string->length++] = 34;
		else
			string->chars[string->length++] = passLine->chars[*passLinepos];
		(*passLinepos)++;
	}
	string->chars[string->length] = 0;
	(*passLinepos)++;
	return 1;
}