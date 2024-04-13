findRuleName(const CharsString *name, const TranslationTableHeader *table) {
	const RuleName *ruleName = table->ruleNames;
	while (ruleName) {
		if ((name->length == ruleName->length) &&
				(memcmp(&name->chars[0], ruleName->name, CHARSIZE * name->length) == 0))
			return ruleName->ruleOffset;
		ruleName = ruleName->next;
	}
	return 0;
}