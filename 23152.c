putChar(const FileInfo *file, widechar c, TranslationTableHeader **table,
		TranslationTableOffset *characterOffset) {
	/* See if a character is in the appropriate table. If not, insert it. In either case,
	 * return a pointer to it. */
	TranslationTableCharacter *character;
	TranslationTableOffset offset;
	if ((character = getChar(c, *table, characterOffset))) return character;
	if (!allocateSpaceInTranslationTable(file, &offset, sizeof(*character), table))
		return NULL;
	character = (TranslationTableCharacter *)&(*table)->ruleArea[offset];
	memset(character, 0, sizeof(*character));
	character->sourceFile = file->sourceFile;
	character->sourceLine = file->lineNumber;
	character->value = c;
	const unsigned long int charHash = _lou_charHash(c);
	const TranslationTableOffset bucket = (*table)->characters[charHash];
	if (!bucket)
		(*table)->characters[charHash] = offset;
	else {
		TranslationTableCharacter *oldchar =
				(TranslationTableCharacter *)&(*table)->ruleArea[bucket];
		while (oldchar->next)
			oldchar = (TranslationTableCharacter *)&(*table)->ruleArea[oldchar->next];
		oldchar->next = offset;
	}
	if (characterOffset) *characterOffset = offset;
	return character;
}