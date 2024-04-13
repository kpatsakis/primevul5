getCharacters(FileInfo *file, CharsString *characters) {
	/* Get ruleChars string */
	CharsString token;
	if (!getToken(file, &token, "characters")) return 0;
	return parseChars(file, characters, &token);
}