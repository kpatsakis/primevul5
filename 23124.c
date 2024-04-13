setDefaults(TranslationTableHeader *table) {
	for (int i = 0; i < 3; i++)
		if (!table->emphRules[i][lenPhraseOffset])
			table->emphRules[i][lenPhraseOffset] = 4;
	if (table->numPasses == 0) table->numPasses = 1;
	return 1;
}