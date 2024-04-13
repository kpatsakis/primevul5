compileBeforeAfter(FileInfo *file) {
	/* 1=before, 2=after, 0=error */
	CharsString token;
	CharsString tmp;
	if (!getToken(file, &token, "last word before or after")) return 0;
	if (!parseChars(file, &tmp, &token)) return 0;
	if (eqasc2uni((unsigned char *)"before", tmp.chars, 6))
		return 1;
	else if (eqasc2uni((unsigned char *)"after", tmp.chars, 5))
		return 2;
	return 0;
}