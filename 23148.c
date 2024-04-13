freeTranslationTable(TranslationTableHeader *t) {
	for (int i = 0; i < MAX_EMPH_CLASSES && t->emphClassNames[i]; i++)
		free(t->emphClassNames[i]);
	for (int i = 0; t->sourceFiles[i]; i++) free(t->sourceFiles[i]);
	if (t->characterClasses) deallocateCharacterClasses(t);
	if (t->ruleNames) deallocateRuleNames(t);
	free(t);
}