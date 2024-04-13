compileNumber(FileInfo *file) {
	CharsString token;
	widechar number;
	if (!getToken(file, &token, "number")) return 0;
	getNumber(&token.chars[0], &number);
	if (!(number > 0)) {
		compileError(file, "a nonzero positive number is required");
		return 0;
	}
	return number;
}