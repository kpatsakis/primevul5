deallocateRuleNames(TranslationTableHeader *table) {
	RuleName **ruleName = &table->ruleNames;
	while (*ruleName) {
		RuleName *rn = *ruleName;
		*ruleName = rn->next;
		free(rn);
	}
}