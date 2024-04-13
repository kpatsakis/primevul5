compileBrailleIndicator(FileInfo *file, const char *ermsg, TranslationTableOpcode opcode,
		TranslationTableOffset *ruleOffset, int noback, int nofor,
		TranslationTableHeader **table) {
	CharsString token;
	CharsString cells;
	if (!getToken(file, &token, ermsg)) return 0;
	if (!parseDots(file, &cells, &token)) return 0;
	return addRule(
			file, opcode, NULL, &cells, 0, 0, ruleOffset, NULL, noback, nofor, table);
}