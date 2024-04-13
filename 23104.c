getCharForDots(widechar d, const DisplayTableHeader *table) {
	CharDotsMapping *cdPtr;
	const TranslationTableOffset bucket = table->dotsToChar[_lou_charHash(d)];
	TranslationTableOffset offset = bucket;
	while (offset) {
		cdPtr = (CharDotsMapping *)&table->ruleArea[offset];
		if (cdPtr->lookFor == d) return cdPtr;
		offset = cdPtr->next;
	}
	return NULL;
}