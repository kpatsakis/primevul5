addRule(const FileInfo *file, TranslationTableOpcode opcode, CharsString *ruleChars,
		CharsString *ruleDots, TranslationTableCharacterAttributes after,
		TranslationTableCharacterAttributes before, TranslationTableOffset *ruleOffset,
		TranslationTableRule **rule, int noback, int nofor,
		TranslationTableHeader **table) {
	/* Add a rule to the table, using the hash function to find the start of
	 * chains and chaining both the chars and dots strings */
	TranslationTableOffset offset;
	int ruleSize = sizeof(TranslationTableRule) - (DEFAULTRULESIZE * CHARSIZE);
	if (ruleChars) ruleSize += CHARSIZE * ruleChars->length;
	if (ruleDots) ruleSize += CHARSIZE * ruleDots->length;
	if (!allocateSpaceInTranslationTable(file, &offset, ruleSize, table)) return 0;
	TranslationTableRule *r = (TranslationTableRule *)&(*table)->ruleArea[offset];
	if (rule) *rule = r;
	if (ruleOffset) *ruleOffset = offset;
	r->sourceFile = file->sourceFile;
	r->sourceLine = file->lineNumber;
	r->opcode = opcode;
	r->after = after;
	r->before = before;
	r->nocross = 0;
	if (ruleChars)
		memcpy(&r->charsdots[0], &ruleChars->chars[0],
				CHARSIZE * (r->charslen = ruleChars->length));
	else
		r->charslen = 0;
	if (ruleDots)
		memcpy(&r->charsdots[r->charslen], &ruleDots->chars[0],
				CHARSIZE * (r->dotslen = ruleDots->length));
	else
		r->dotslen = 0;

	/* link new rule into table. */
	if (opcode == CTO_SwapCc || opcode == CTO_SwapCd || opcode == CTO_SwapDd) return 1;
	if (opcode >= CTO_Context && opcode <= CTO_Pass4)
		if (!(opcode == CTO_Context && r->charslen > 0)) {
			if (!nofor)
				if (!addForwardPassRule(offset, r, *table)) return 0;
			if (!noback)
				if (!addBackwardPassRule(offset, r, *table)) return 0;
			return 1;
		}
	if (!nofor) {
		if (r->charslen == 1) {
			addForwardRuleWithSingleChar(file, offset, r, table);
			// addForwardRuleWithSingleChar may have moved table, so make sure rule is
			// still valid
			r = (TranslationTableRule *)&(*table)->ruleArea[offset];
			if (rule) *rule = r;
		} else if (r->charslen > 1)
			addForwardRuleWithMultipleChars(offset, r, *table);
	}
	if (!noback) {
		widechar *cells;
		int dotslen;

		if (r->opcode == CTO_Context) {
			cells = &r->charsdots[0];
			dotslen = r->charslen;
		} else {
			cells = &r->charsdots[r->charslen];
			dotslen = r->dotslen;
		}
		if (dotslen == 1) {
			addBackwardRuleWithSingleCell(file, *cells, offset, r, table);
			// addBackwardRuleWithSingleCell may have moved table, so make sure rule is
			// still valid
			r = (TranslationTableRule *)&(*table)->ruleArea[offset];
			if (rule) *rule = r;
		} else if (dotslen > 1)
			addBackwardRuleWithMultipleCells(cells, dotslen, offset, r, *table);
	}
	return 1;
}