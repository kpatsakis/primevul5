compileCharDef(FileInfo *file, TranslationTableOpcode opcode,
		TranslationTableCharacterAttributes attributes, int noback, int nofor,
		TranslationTableHeader **table, DisplayTableHeader **displayTable) {
	CharsString ruleChars;
	CharsString ruleDots;
	if (!getRuleCharsText(file, &ruleChars)) return 0;
	if (!getRuleDotsPattern(file, &ruleDots)) return 0;
	if (ruleChars.length != 1) {
		compileError(file, "Exactly one character is required.");
		return 0;
	}
	if (ruleDots.length < 1) {
		compileError(file, "At least one cell is required.");
		return 0;
	}
	if (table) {
		TranslationTableCharacter *character;
		TranslationTableCharacter *cell = NULL;
		int k;
		if (attributes & (CTC_UpperCase | CTC_LowerCase)) attributes |= CTC_Letter;
		character = putChar(file, ruleChars.chars[0], table, NULL);
		character->attributes |= attributes;
		for (k = ruleDots.length - 1; k >= 0; k -= 1) {
			cell = getDots(ruleDots.chars[k], *table);
			if (!cell) cell = putDots(file, ruleDots.chars[k], table);
		}
		if (ruleDots.length == 1) cell->attributes |= attributes;
	}
	if (displayTable && ruleDots.length == 1)
		putCharDotsMapping(file, ruleChars.chars[0], ruleDots.chars[0], displayTable);
	if (table)
		if (!addRule(file, opcode, &ruleChars, &ruleDots, 0, 0, NULL, NULL, noback, nofor,
					table))
			return 0;
	return 1;
}