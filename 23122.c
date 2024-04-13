compileTable(const char *tableList, const char *displayTableList,
		TranslationTableHeader **translationTable, DisplayTableHeader **displayTable) {
	char **tableFiles;
	char **subTable;
	if (translationTable && !tableList) return 0;
	if (displayTable && !displayTableList) return 0;
	if (!translationTable && !displayTable) return 0;
	if (translationTable) *translationTable = NULL;
	if (displayTable) *displayTable = NULL;
	errorCount = warningCount = fileCount = 0;
	if (!opcodeLengths[0]) {
		TranslationTableOpcode opcode;
		for (opcode = 0; opcode < CTO_None; opcode++)
			opcodeLengths[opcode] = (short)strlen(opcodeNames[opcode]);
	}
	if (translationTable) allocateTranslationTable(NULL, translationTable);
	if (displayTable) allocateDisplayTable(NULL, displayTable);

	if (translationTable) {
		(*translationTable)->emphClassNames[0] = NULL;
		(*translationTable)->characterClasses = NULL;
		(*translationTable)->ruleNames = NULL;
	}

	/* Compile things that are necesary for the proper operation of
	 * liblouis or liblouisxml or liblouisutdml */
	/* TODO: These definitions seem to be necessary for proper functioning of
	   liblouisutdml. Find a way to satisfy those requirements without hard coding
	   some characters in every table notably behind the users back */
	compileString("space \\xffff 123456789abcdef LOU_ENDSEGMENT", translationTable,
			displayTable);

	if (displayTable && translationTable && strcmp(tableList, displayTableList) == 0) {
		/* Compile the display and translation tables in one go */

		/* Compile all subtables in the list */
		if (!(tableFiles = _lou_resolveTable(tableList, NULL))) {
			errorCount++;
			goto cleanup;
		}
		for (subTable = tableFiles; *subTable; subTable++)
			if (!compileFile(*subTable, translationTable, displayTable)) goto cleanup;
	} else {
		/* Compile the display and translation tables separately */

		if (displayTable) {
			if (!(tableFiles = _lou_resolveTable(displayTableList, NULL))) {
				errorCount++;
				goto cleanup;
			}
			for (subTable = tableFiles; *subTable; subTable++)
				if (!compileFile(*subTable, NULL, displayTable)) goto cleanup;
			free_tablefiles(tableFiles);
			tableFiles = NULL;
		}
		if (translationTable) {
			if (!(tableFiles = _lou_resolveTable(tableList, NULL))) {
				errorCount++;
				goto cleanup;
			}
			for (subTable = tableFiles; *subTable; subTable++)
				if (!compileFile(*subTable, translationTable, NULL)) goto cleanup;
		}
	}

/* Clean up after compiling files */
cleanup:
	free_tablefiles(tableFiles);
	if (warningCount) _lou_logMessage(LOU_LOG_WARN, "%d warnings issued", warningCount);
	if (!errorCount) {
		if (translationTable) setDefaults(*translationTable);
		return 1;
	} else {
		_lou_logMessage(LOU_LOG_ERROR, "%d errors found.", errorCount);
		if (translationTable) {
			if (*translationTable) freeTranslationTable(*translationTable);
			*translationTable = NULL;
		}
		if (displayTable) {
			if (*displayTable) free(*displayTable);
			*displayTable = NULL;
		}
		return 0;
	}
}