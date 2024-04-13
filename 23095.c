lou_free(void) {
	TranslationTableChainEntry *currentEntry;
	TranslationTableChainEntry *previousEntry;
	lou_logEnd();
	if (translationTableChain != NULL) {
		currentEntry = translationTableChain;
		while (currentEntry) {
			freeTranslationTable(currentEntry->table);
			previousEntry = currentEntry;
			currentEntry = currentEntry->next;
			free(previousEntry);
		}
		translationTableChain = NULL;
	}
	if (typebuf != NULL) free(typebuf);
	typebuf = NULL;
	if (wordBuffer != NULL) free(wordBuffer);
	wordBuffer = NULL;
	if (emphasisBuffer != NULL) free(emphasisBuffer);
	emphasisBuffer = NULL;
	sizeTypebuf = 0;
	if (destSpacing != NULL) free(destSpacing);
	destSpacing = NULL;
	sizeDestSpacing = 0;
	{
		int k;
		for (k = 0; k < MAXPASSBUF; k++) {
			if (passbuf[k] != NULL) free(passbuf[k]);
			passbuf[k] = NULL;
			sizePassbuf[k] = 0;
		}
	}
	if (posMapping1 != NULL) free(posMapping1);
	posMapping1 = NULL;
	sizePosMapping1 = 0;
	if (posMapping2 != NULL) free(posMapping2);
	posMapping2 = NULL;
	sizePosMapping2 = 0;
	if (posMapping3 != NULL) free(posMapping3);
	posMapping3 = NULL;
	sizePosMapping3 = 0;
	opcodeLengths[0] = 0;
}