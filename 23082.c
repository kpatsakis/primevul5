compileSwap(FileInfo *file, TranslationTableOpcode opcode, int noback, int nofor,
		TranslationTableHeader **table) {
	CharsString ruleChars;
	CharsString ruleDots;
	CharsString name;
	CharsString matches;
	CharsString replacements;
	TranslationTableOffset ruleOffset;
	if (!getToken(file, &name, "name operand")) return 0;
	if (!getToken(file, &matches, "matches operand")) return 0;
	if (!getToken(file, &replacements, "replacements operand")) return 0;
	if (opcode == CTO_SwapCc || opcode == CTO_SwapCd) {
		if (!parseChars(file, &ruleChars, &matches)) return 0;
	} else {
		if (!compileSwapDots(file, &matches, &ruleChars)) return 0;
	}
	if (opcode == CTO_SwapCc) {
		if (!parseChars(file, &ruleDots, &replacements)) return 0;
	} else {
		if (!compileSwapDots(file, &replacements, &ruleDots)) return 0;
	}
	if (!addRule(file, opcode, &ruleChars, &ruleDots, 0, 0, &ruleOffset, NULL, noback,
				nofor, table))
		return 0;
	if (!addRuleName(file, &name, ruleOffset, *table)) return 0;
	return 1;
}