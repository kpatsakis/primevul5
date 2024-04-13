atTokenDelimiter(const FileInfo *file) {
	return file->line[file->linepos] <= 32;
}