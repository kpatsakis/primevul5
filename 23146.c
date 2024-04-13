_lou_getDisplayTable(const char *tableList) {
	DisplayTableHeader *table;
	getTable(NULL, tableList, NULL, &table);
	return table;
}