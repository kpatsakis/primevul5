_lou_compileDisplayRule(const char *tableList, const char *inString) {
	DisplayTableHeader *table;
	getTable(NULL, tableList, NULL, &table);
	return compileString(inString, NULL, &table);
}