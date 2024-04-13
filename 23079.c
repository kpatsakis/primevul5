putDots(const FileInfo *file, widechar d, TranslationTableHeader **table) {
	/* See if a dot pattern is in the appropriate table. If not, insert it. In either
	 * case, return a pointer to it. */
	TranslationTableCharacter *character;
	TranslationTableOffset offset;
	if ((character = getDots(d, *table))) return character;
	if (!allocateSpaceInTranslationTable(file, &offset, sizeof(*character), table))
		return NULL;
	character = (TranslationTableCharacter *)&(*table)->ruleArea[offset];
	memset(character, 0, sizeof(*character));
	character->sourceFile = file->sourceFile;
	character->sourceLine = file->lineNumber;
	character->value = d;
	const unsigned long int charHash = _lou_charHash(d);
	const TranslationTableOffset bucket = (*table)->dots[charHash];
	if (!bucket)
		(*table)->dots[charHash] = offset;
	else {
		TranslationTableCharacter *oldchar =
				(TranslationTableCharacter *)&(*table)->ruleArea[bucket];
		while (oldchar->next)
			oldchar = (TranslationTableCharacter *)&(*table)->ruleArea[oldchar->next];
		oldchar->next = offset;
	}
	return character;
}