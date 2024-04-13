_lou_getTable(const char *tableList, const char *displayTableList,
		const TranslationTableHeader **translationTable,
		const DisplayTableHeader **displayTable) {
	TranslationTableHeader *newTable;
	DisplayTableHeader *newDisplayTable;
	getTable(tableList, displayTableList, &newTable, &newDisplayTable);
	if (newTable)
		if (!finalizeTable(newTable)) newTable = NULL;
	*translationTable = newTable;
	*displayTable = newDisplayTable;
}