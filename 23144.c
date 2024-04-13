lou_getTable(const char *tableList) {
	const TranslationTableHeader *table;
	const DisplayTableHeader *displayTable;
	_lou_getTable(tableList, tableList, &table, &displayTable);
	if (!table || !displayTable) return NULL;
	return table;
}