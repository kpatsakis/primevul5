getRuleDotsPattern(FileInfo *file, CharsString *ruleDots) {
	/* Interpret the dets operand */
	CharsString token;
	if (!getToken(file, &token, "Dots operand")) return 0;
	if (token.length == 1 && token.chars[0] == '=') {
		ruleDots->length = 0;
		return 1;
	} else
		return parseDots(file, ruleDots, &token);
}