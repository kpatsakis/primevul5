addRuleName(const FileInfo *file, CharsString *name, TranslationTableOffset ruleOffset,
		TranslationTableHeader *table) {
	int k;
	RuleName *ruleName;
	if (!(ruleName = malloc(sizeof(*ruleName) + CHARSIZE * (name->length - 1)))) {
		compileError(file, "not enough memory");
		_lou_outOfMemory();
	}
	memset(ruleName, 0, sizeof(*ruleName));
	// a name is a sequence of characters in the ranges 'a'..'z' and 'A'..'Z'
	for (k = 0; k < name->length; k++) {
		widechar c = name->chars[k];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
			ruleName->name[k] = c;
		else {
			compileError(file, "a name may contain only letters");
			free(ruleName);
			return 0;
		}
	}
	ruleName->length = name->length;
	ruleName->ruleOffset = ruleOffset;
	ruleName->next = table->ruleNames;
	table->ruleNames = ruleName;
	return 1;
}