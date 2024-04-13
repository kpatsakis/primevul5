addBackwardPassRule(TranslationTableOffset ruleOffset, TranslationTableRule *rule,
		TranslationTableHeader *table) {
	TranslationTableOffset *backPassRule;
	switch (rule->opcode) {
	case CTO_Correct:
		backPassRule = &table->backPassRules[0];
		break;
	case CTO_Context:
		backPassRule = &table->backPassRules[1];
		break;
	case CTO_Pass2:
		backPassRule = &table->backPassRules[2];
		break;
	case CTO_Pass3:
		backPassRule = &table->backPassRules[3];
		break;
	case CTO_Pass4:
		backPassRule = &table->backPassRules[4];
		break;
	default:
		return 0;
	}
	while (*backPassRule) {
		TranslationTableRule *r = (TranslationTableRule *)&table->ruleArea[*backPassRule];
		if (rule->charslen > r->charslen) break;
		backPassRule = &r->dotsnext;
	}
	rule->dotsnext = *backPassRule;
	*backPassRule = ruleOffset;
	return 1;
}