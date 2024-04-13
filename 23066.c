lou_getTypeformForEmphClass(const char *tableList, const char *emphClass) {
	const TranslationTableHeader *table = _lou_getTranslationTable(tableList);
	if (!table) return 0;
	for (int i = 0; i < MAX_EMPH_CLASSES && table->emphClassNames[i]; i++)
		if (strcmp(emphClass, table->emphClassNames[i]) == 0) return italic << i;
	return 0;
}