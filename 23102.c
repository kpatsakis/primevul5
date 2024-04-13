allocateSpaceInTranslationTable(const FileInfo *file, TranslationTableOffset *offset,
		int size, TranslationTableHeader **table) {
	/* allocate memory for table and expand previously allocated memory if necessary */
	int spaceNeeded = ((size + OFFSETSIZE - 1) / OFFSETSIZE) * OFFSETSIZE;
	TranslationTableOffset newTableSize = (*table)->bytesUsed + spaceNeeded;
	TranslationTableOffset tableSize = (*table)->tableSize;
	if (newTableSize > tableSize) {
		TranslationTableHeader *newTable;
		newTableSize += (newTableSize / OFFSETSIZE);
		newTable = realloc(*table, newTableSize);
		if (!newTable) {
			compileError(file, "Not enough memory for translation table.");
			_lou_outOfMemory();
		}
		memset(((unsigned char *)newTable) + tableSize, 0, newTableSize - tableSize);
		/* update references to the old table */
		{
			TranslationTableChainEntry *entry;
			for (entry = translationTableChain; entry != NULL; entry = entry->next)
				if (entry->table == *table)
					entry->table = (TranslationTableHeader *)newTable;
		}
		newTable->tableSize = newTableSize;
		*table = newTable;
	}
	if (offset != NULL) {
		*offset = ((*table)->bytesUsed - sizeof(**table)) / OFFSETSIZE;
		(*table)->bytesUsed += spaceNeeded;
	}
	return 1;
}