lou_compileString(const char *tableList, const char *inString) {
	TranslationTableHeader *table;
	DisplayTableHeader *displayTable;
	getTable(tableList, tableList, &table, &displayTable);
	if (!table) return 0;
	if (!compileString(inString, &table, &displayTable)) return 0;
	return 1;
}