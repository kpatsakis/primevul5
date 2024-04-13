atEndOfLine(const FileInfo *file) {
	return file->linepos >= file->linelen;
}