putCharDotsMapping(
		const FileInfo *file, widechar c, widechar d, DisplayTableHeader **table) {
	if (!getDotsForChar(c, *table)) {
		CharDotsMapping *cdPtr;
		TranslationTableOffset offset;
		if (!allocateSpaceInDisplayTable(file, &offset, sizeof(*cdPtr), table)) return 0;
		cdPtr = (CharDotsMapping *)&(*table)->ruleArea[offset];
		cdPtr->next = 0;
		cdPtr->lookFor = c;
		cdPtr->found = d;
		const unsigned long int charHash = _lou_charHash(c);
		const TranslationTableOffset bucket = (*table)->charToDots[charHash];
		if (!bucket)
			(*table)->charToDots[charHash] = offset;
		else {
			CharDotsMapping *oldcdPtr = (CharDotsMapping *)&(*table)->ruleArea[bucket];
			while (oldcdPtr->next)
				oldcdPtr = (CharDotsMapping *)&(*table)->ruleArea[oldcdPtr->next];
			oldcdPtr->next = offset;
		}
	}
	if (!getCharForDots(d, *table)) {
		CharDotsMapping *cdPtr;
		TranslationTableOffset offset;
		if (!allocateSpaceInDisplayTable(file, &offset, sizeof(*cdPtr), table)) return 0;
		cdPtr = (CharDotsMapping *)&(*table)->ruleArea[offset];
		cdPtr->next = 0;
		cdPtr->lookFor = d;
		cdPtr->found = c;
		const unsigned long int charHash = _lou_charHash(d);
		const TranslationTableOffset bucket = (*table)->dotsToChar[charHash];
		if (!bucket)
			(*table)->dotsToChar[charHash] = offset;
		else {
			CharDotsMapping *oldcdPtr = (CharDotsMapping *)&(*table)->ruleArea[bucket];
			while (oldcdPtr->next)
				oldcdPtr = (CharDotsMapping *)&(*table)->ruleArea[oldcdPtr->next];
			oldcdPtr->next = offset;
		}
	}
	return 1;
}