addCharacterClass(const FileInfo *file, const widechar *name, int length,
		TranslationTableHeader *table, int validate) {
	/* Define a character class, Whether predefined or user-defined */
	if (validate) {
		for (int i = 0; i < length; i++) {
			if (!((name[i] >= 'a' && name[i] <= 'z') ||
						(name[i] >= 'A' && name[i] <= 'Z'))) {
				// don't abort because in some cases (before/after rules)
				// this will work fine, but it will not work in multipass
				// expressions
				compileWarning(file,
						"Invalid attribute name: must be a digit between "
						"0 and 7 or a word containing only letters");
			}
		}
		// check that name is not reserved
		int k = 0;
		while (reservedAttributeNames[k]) {
			if (strlen(reservedAttributeNames[k]) == length) {
				int i;
				for (i = 0; i < length; i++)
					if (reservedAttributeNames[k][i] != name[i]) break;
				if (i == length) {
					compileError(file, "Attribute name is reserved: %s",
							reservedAttributeNames[k]);
					return NULL;
				}
			}
			k++;
		}
	}
	CharacterClass **classes = &table->characterClasses;
	TranslationTableCharacterAttributes attribute = getNextAttribute(table);
	CharacterClass *class;
	if (attribute) {
		if (!(class = malloc(sizeof(*class) + CHARSIZE * (length - 1))))
			_lou_outOfMemory();
		else {
			memset(class, 0, sizeof(*class));
			memcpy(class->name, name, CHARSIZE * (class->length = length));
			class->attribute = attribute;
			class->next = *classes;
			*classes = class;
			return class;
		}
	}
	compileError(file, "character class table overflow.");
	return NULL;
}