_lou_getTranslationTable(const char *tableList) {
	TranslationTableHeader *table;
	getTable(tableList, NULL, &table, NULL);
	if (table)
		if (!finalizeTable(table)) table = NULL;
	return table;
}