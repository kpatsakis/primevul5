passGetVariableNumber(
		const FileInfo *file, CharsString *passLine, int *passLinepos, widechar *number) {
	if (!passGetNumber(passLine, passLinepos, number)) {
		compileError(file, "missing variable number");
		return 0;
	}
	if ((*number >= 0) && (*number < NUMVAR)) return 1;
	compileError(file, "variable number out of range");
	return 0;
}