compileGrouping(FileInfo *file, int noback, int nofor, TranslationTableHeader **table,
		DisplayTableHeader **displayTable) {
	int k;
	CharsString name;
	CharsString groupChars;
	CharsString groupDots;
	CharsString dotsParsed;
	if (!getToken(file, &name, "name operand")) return 0;
	if (!getRuleCharsText(file, &groupChars)) return 0;
	if (!getToken(file, &groupDots, "dots operand")) return 0;
	for (k = 0; k < groupDots.length && groupDots.chars[k] != ','; k++)
		;
	if (k == groupDots.length) {
		compileError(file, "Dots operand must consist of two cells separated by a comma");
		return 0;
	}
	groupDots.chars[k] = '-';
	if (!parseDots(file, &dotsParsed, &groupDots)) return 0;
	if (groupChars.length != 2 || dotsParsed.length != 2) {
		compileError(file,
				"two Unicode characters and two cells separated by a comma are needed.");
		return 0;
	}
	if (table) {
		TranslationTableOffset ruleOffset;
		TranslationTableCharacter *charsDotsPtr;
		charsDotsPtr = putChar(file, groupChars.chars[0], table, NULL);
		charsDotsPtr->attributes |= CTC_Math;
		charsDotsPtr = putChar(file, groupChars.chars[1], table, NULL);
		charsDotsPtr->attributes |= CTC_Math;
		charsDotsPtr = putDots(file, dotsParsed.chars[0], table);
		charsDotsPtr->attributes |= CTC_Math;
		charsDotsPtr = putDots(file, dotsParsed.chars[1], table);
		charsDotsPtr->attributes |= CTC_Math;
		if (!addRule(file, CTO_Grouping, &groupChars, &dotsParsed, 0, 0, &ruleOffset,
					NULL, noback, nofor, table))
			return 0;
		if (!addRuleName(file, &name, ruleOffset, *table)) return 0;
	}
	if (displayTable) {
		putCharDotsMapping(file, groupChars.chars[0], dotsParsed.chars[0], displayTable);
		putCharDotsMapping(file, groupChars.chars[1], dotsParsed.chars[1], displayTable);
	}
	if (table) {
		widechar endChar;
		widechar endDots;
		endChar = groupChars.chars[1];
		endDots = dotsParsed.chars[1];
		groupChars.length = dotsParsed.length = 1;
		if (!addRule(file, CTO_Math, &groupChars, &dotsParsed, 0, 0, NULL, NULL, noback,
					nofor, table))
			return 0;
		groupChars.chars[0] = endChar;
		dotsParsed.chars[0] = endDots;
		if (!addRule(file, CTO_Math, &groupChars, &dotsParsed, 0, 0, NULL, NULL, noback,
					nofor, table))
			return 0;
	}
	return 1;
}