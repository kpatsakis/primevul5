addForwardRuleWithMultipleChars(TranslationTableOffset ruleOffset,
		TranslationTableRule *rule, TranslationTableHeader *table) {
	/* direction = 0 rule->charslen > 1 */
	TranslationTableOffset *forRule =
			&table->forRules[_lou_stringHash(&rule->charsdots[0], 0, NULL)];
	while (*forRule) {
		TranslationTableRule *r = (TranslationTableRule *)&table->ruleArea[*forRule];
		if (rule->charslen > r->charslen) break;
		if (rule->charslen == r->charslen)
			if ((r->opcode == CTO_Always) && (rule->opcode != CTO_Always)) break;
		forRule = &r->charsnext;
	}
	rule->charsnext = *forRule;
	*forRule = ruleOffset;
}