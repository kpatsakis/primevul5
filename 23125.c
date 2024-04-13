getNextAttribute(TranslationTableHeader *table) {
	/* Get the next attribute value, or 0 if there is no more space in the table. */
	TranslationTableCharacterAttributes next = table->nextCharacterClassAttribute;
	if (next) {
		if (next == CTC_LitDigit)
			table->nextCharacterClassAttribute = CTC_UserDefined9;
		else
			table->nextCharacterClassAttribute <<= 1;
		return next;
	} else
		return getNextNumberedAttribute(table);
}