getChar(widechar c, TranslationTableHeader *table,
		TranslationTableOffset *characterOffset) {
	const TranslationTableOffset bucket = table->characters[_lou_charHash(c)];
	TranslationTableOffset offset = bucket;
	while (offset) {
		TranslationTableCharacter *character =
				(TranslationTableCharacter *)&table->ruleArea[offset];
		if (character->value == c) {
			if (characterOffset) *characterOffset = offset;
			return character;
		}
		offset = character->next;
	}
	return NULL;
}