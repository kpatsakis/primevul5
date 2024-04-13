deallocateCharacterClasses(TranslationTableHeader *table) {
	CharacterClass **classes = &table->characterClasses;
	while (*classes) {
		CharacterClass *class = *classes;
		*classes = (*classes)->next;
		if (class) free(class);
	}
}