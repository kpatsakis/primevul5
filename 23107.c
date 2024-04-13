getRuleDotsText(FileInfo *file, CharsString *ruleDots) {
	CharsString token;
	if (!getToken(file, &token, "characters")) return 0;
	return parseChars(file, ruleDots, &token);
}