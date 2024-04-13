getDotsForChar(widechar c, const DisplayTableHeader *table) {
	CharDotsMapping *cdPtr;
	const TranslationTableOffset bucket = table->charToDots[_lou_charHash(c)];
	TranslationTableOffset offset = bucket;
	while (offset) {
		cdPtr = (CharDotsMapping *)&table->ruleArea[offset];
		if (cdPtr->lookFor == c) return cdPtr;
		offset = cdPtr->next;
	}
	return NULL;
}