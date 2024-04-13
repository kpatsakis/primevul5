_lou_getALine(FileInfo *file) {
	/* Read a line of widechar's from an input file */
	int ch;
	int pch = 0;
	file->linelen = 0;
	while ((ch = getAChar(file)) != EOF) {
		if (ch == 13) continue;
		if (pch == '\\' && ch == 10) {
			file->linelen--;
			pch = ch;
			continue;
		}
		if (ch == 10 || file->linelen >= MAXSTRING - 1) break;
		file->line[file->linelen++] = (widechar)ch;
		pch = ch;
	}
	file->line[file->linelen] = 0;
	file->linepos = 0;
	if (ch == EOF && !file->linelen) return 0;
	file->lineNumber++;
	return 1;
}