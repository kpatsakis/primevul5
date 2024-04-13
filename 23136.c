getNextNumberedAttribute(TranslationTableHeader *table) {
	/* Get the next attribute value for numbered attributes, or 0 if there is no more
	 * space in the table. */
	TranslationTableCharacterAttributes next = table->nextNumberedCharacterClassAttribute;
	if (next > CTC_UserDefined8) return 0;
	table->nextNumberedCharacterClassAttribute <<= 1;
	return next;
}