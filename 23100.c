addForwardRuleWithSingleChar(const FileInfo *file, TranslationTableOffset ruleOffset,
		TranslationTableRule *rule, TranslationTableHeader **table) {
	/* direction = 0, rule->charslen = 1 */
	TranslationTableCharacter *character;
	// get the character from the table, or if the character is not defined yet, define it
	// (without adding attributes)
	if (rule->opcode >= CTO_Pass2 && rule->opcode <= CTO_Pass4) {
		character = putDots(file, rule->charsdots[0], table);
		// putDots may have moved table, so make sure rule is still valid
		rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
	} else if (rule->opcode == CTO_CompDots || rule->opcode == CTO_Comp6) {
		character = putChar(file, rule->charsdots[0], table, NULL);
		// putChar may have moved table, so make sure rule is still valid
		rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
		character->compRule = ruleOffset;
		return;
	} else {
		character = putChar(file, rule->charsdots[0], table, NULL);
		// putChar may have moved table, so make sure rule is still valid
		rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
		// if the new rule is a character definition rule, set the main definition rule of
		// this character to it (possibly overwriting previous definition rules)
		// adding the attributes to the character has already been done elsewhere
		if (rule->opcode >= CTO_Space && rule->opcode < CTO_UpLow) {
			if (character->definitionRule) {
				TranslationTableRule *prevRule =
						(TranslationTableRule *)&(*table)
								->ruleArea[character->definitionRule];
				_lou_logMessage(LOU_LOG_DEBUG,
						"%s:%d: Character already defined (%s). The new definition will "
						"take precedence.",
						file->fileName, file->lineNumber,
						printSource(file, prevRule->sourceFile, prevRule->sourceLine));
			} else if (character->basechar) {
				_lou_logMessage(LOU_LOG_DEBUG,
						"%s:%d: A base rule already exists for this character (%s). The "
						"%s rule will take precedence.",
						file->fileName, file->lineNumber,
						printSource(file, character->sourceFile, character->sourceLine),
						_lou_findOpcodeName(rule->opcode));
				character->basechar = 0;
				character->mode = 0;
			}
			character->definitionRule = ruleOffset;
		}
	}
	// add the new rule to the list of rules associated with this character
	// if the new rule is a character definition rule, it is inserted at the end of the
	// list
	// otherwise it is inserted before the first character definition rule
	TranslationTableOffset *otherRule = &character->otherRules;
	while (*otherRule) {
		TranslationTableRule *r = (TranslationTableRule *)&(*table)->ruleArea[*otherRule];
		if (r->charslen == 0) break;
		if (r->opcode >= CTO_Space && r->opcode < CTO_UpLow)
			if (!(rule->opcode >= CTO_Space && rule->opcode < CTO_UpLow)) break;
		otherRule = &r->charsnext;
	}
	rule->charsnext = *otherRule;
	*otherRule = ruleOffset;
}