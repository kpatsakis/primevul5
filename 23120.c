_lou_compileTranslationRule(const char *tableList, const char *inString) {
	TranslationTableHeader *table;
	getTable(tableList, NULL, &table, NULL);
	return compileString(inString, &table, NULL);
}