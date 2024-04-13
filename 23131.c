_lou_defaultTableResolver(const char *tableList, const char *base) {
	char *searchPath;
	char **tableFiles;
	char *subTable;
	char *tableList_copy;
	char *cp;
	int last;
	int k;

	/* Set up search path */
	searchPath = _lou_getTablePath();

	/* Count number of subtables in table list */
	k = 0;
	for (cp = (char *)tableList; *cp != '\0'; cp++)
		if (*cp == ',') k++;
	tableFiles = (char **)calloc(k + 2, sizeof(char *));
	if (!tableFiles) _lou_outOfMemory();

	/* Resolve subtables */
	k = 0;
	tableList_copy = strdup(tableList);
	for (subTable = tableList_copy;; subTable = cp + 1) {
		for (cp = subTable; *cp != '\0' && *cp != ','; cp++)
			;
		last = (*cp == '\0');
		*cp = '\0';
		if (!(tableFiles[k++] = resolveSubtable(subTable, base, searchPath))) {
			char *path;
			_lou_logMessage(LOU_LOG_ERROR, "Cannot resolve table '%s'", subTable);
			path = getenv("LOUIS_TABLEPATH");
			if (path != NULL && path[0] != '\0')
				_lou_logMessage(LOU_LOG_ERROR, "LOUIS_TABLEPATH=%s", path);
			free(searchPath);
			free(tableList_copy);
			free_tablefiles(tableFiles);
			return NULL;
		}
		if (k == 1) base = subTable;
		if (last) break;
	}
	free(searchPath);
	free(tableList_copy);
	tableFiles[k] = NULL;
	return tableFiles;
}