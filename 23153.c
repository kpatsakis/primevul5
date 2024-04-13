getDots(widechar d, TranslationTableHeader *table) {
	const TranslationTableOffset bucket = table->dots[_lou_charHash(d)];
	TranslationTableOffset offset = bucket;
	while (offset) {
		TranslationTableCharacter *character =
				(TranslationTableCharacter *)&table->ruleArea[offset];
		if (character->value == d) return character;
		offset = character->next;
	}
	return NULL;
}