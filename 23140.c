addForwardPassRule(TranslationTableOffset ruleOffset, TranslationTableRule *rule,
		TranslationTableHeader *table) {
	TranslationTableOffset *forPassRule;
	switch (rule->opcode) {
	case CTO_Correct:
		forPassRule = &table->forPassRules[0];
		break;
	case CTO_Context:
		forPassRule = &table->forPassRules[1];
		break;
	case CTO_Pass2:
		forPassRule = &table->forPassRules[2];
		break;
	case CTO_Pass3:
		forPassRule = &table->forPassRules[3];
		break;
	case CTO_Pass4:
		forPassRule = &table->forPassRules[4];
		break;
	default:
		return 0;
	}
	while (*forPassRule) {
		TranslationTableRule *r = (TranslationTableRule *)&table->ruleArea[*forPassRule];
		if (rule->charslen > r->charslen) break;
		forPassRule = &r->charsnext;
	}
	rule->charsnext = *forPassRule;
	*forPassRule = ruleOffset;
	return 1;
}