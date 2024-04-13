allocateDisplayTable(const FileInfo *file, DisplayTableHeader **table) {
	/* Allocate memory for the table and a guess on the number of rules */
	const TranslationTableOffset startSize = 2 * sizeof(**table);
	if (*table) return 1;
	TranslationTableOffset bytesUsed =
			sizeof(**table) + OFFSETSIZE; /* So no offset is ever zero */
	if (!(*table = malloc(startSize))) {
		compileError(file, "Not enough memory");
		if (*table != NULL) free(*table);
		*table = NULL;
		_lou_outOfMemory();
	}
	memset(*table, 0, startSize);
	(*table)->tableSize = startSize;
	(*table)->bytesUsed = bytesUsed;
	return 1;
}