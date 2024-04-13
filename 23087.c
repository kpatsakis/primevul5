addBackwardRuleWithMultipleCells(widechar *cells, int dotslen,
		TranslationTableOffset ruleOffset, TranslationTableRule *rule,
		TranslationTableHeader *table) {
	/* direction = 1, dotslen > 1 */
	TranslationTableOffset *backRule = &table->backRules[_lou_stringHash(cells, 0, NULL)];
	if (rule->opcode == CTO_SwapCc) return;
	int ruleLength = dotslen + rule->charslen;
	while (*backRule) {
		TranslationTableRule *r = (TranslationTableRule *)&table->ruleArea[*backRule];
		int rLength = r->dotslen + r->charslen;
		if (ruleLength > rLength) break;
		if (rLength == ruleLength)
			if ((r->opcode == CTO_Always) && (rule->opcode != CTO_Always)) break;
		backRule = &r->dotsnext;
	}
	rule->dotsnext = *backRule;
	*backRule = ruleOffset;
}