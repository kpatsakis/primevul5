appendInstructionChar(
		const FileInfo *file, widechar *passInstructions, int *passIC, widechar ch) {
	if (*passIC >= MAXSTRING) {
		compileError(file, "multipass operand too long");
		return 0;
	}
	passInstructions[(*passIC)++] = ch;
	return 1;
}