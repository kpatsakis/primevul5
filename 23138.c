compileHyphenation(
		FileInfo *file, CharsString *encoding, TranslationTableHeader **table) {
	CharsString hyph;
	HyphenationTrans *holdPointer;
	HyphenHashTab *hashTab;
	CharsString word;
	char pattern[MAXSTRING + 1];
	unsigned int stateNum = 0, lastState = 0;
	int i, j, k = encoding->length;
	widechar ch;
	int found;
	HyphenHashEntry *e;
	HyphenDict dict;
	TranslationTableOffset holdOffset;
	/* Set aside enough space for hyphenation states and transitions in
	 * translation table. Must be done before anything else */
	allocateSpaceInTranslationTable(file, NULL, 250000, table);
	hashTab = hyphenHashNew();
	dict.numStates = 1;
	dict.states = malloc(sizeof(HyphenationState));
	if (!dict.states) _lou_outOfMemory();
	dict.states[0].hyphenPattern = 0;
	dict.states[0].fallbackState = DEFAULTSTATE;
	dict.states[0].numTrans = 0;
	dict.states[0].trans.pointer = NULL;
	do {
		if (encoding->chars[0] == 'I') {
			if (!getToken(file, &hyph, NULL)) continue;
		} else {
			/* UTF-8 */
			if (!getToken(file, &word, NULL)) continue;
			parseChars(file, &hyph, &word);
		}
		if (hyph.length == 0 || hyph.chars[0] == '#' || hyph.chars[0] == '%' ||
				hyph.chars[0] == '<')
			continue; /* comment */
		j = 0;
		pattern[j] = '0';
		for (i = 0; i < hyph.length; i++) {
			if (hyph.chars[i] >= '0' && hyph.chars[i] <= '9')
				pattern[j] = (char)hyph.chars[i];
			else {
				word.chars[j] = hyph.chars[i];
				pattern[++j] = '0';
			}
		}
		word.chars[j] = 0;
		word.length = j;
		pattern[j + 1] = 0;
		for (i = 0; pattern[i] == '0'; i++)
			;
		found = hyphenHashLookup(hashTab, &word);
		if (found != DEFAULTSTATE)
			stateNum = found;
		else
			stateNum = hyphenGetNewState(&dict, hashTab, &word);
		k = j + 2 - i;
		if (k > 0) {
			allocateSpaceInTranslationTable(
					file, &dict.states[stateNum].hyphenPattern, k, table);
			memcpy(&(*table)->ruleArea[dict.states[stateNum].hyphenPattern], &pattern[i],
					k);
		}
		/* now, put in the prefix transitions */
		while (found == DEFAULTSTATE) {
			lastState = stateNum;
			ch = word.chars[word.length-- - 1];
			found = hyphenHashLookup(hashTab, &word);
			if (found != DEFAULTSTATE)
				stateNum = found;
			else
				stateNum = hyphenGetNewState(&dict, hashTab, &word);
			hyphenAddTrans(&dict, stateNum, lastState, ch);
		}
	} while (_lou_getALine(file));
	/* put in the fallback states */
	for (i = 0; i < HYPHENHASHSIZE; i++) {
		for (e = hashTab->entries[i]; e; e = e->next) {
			for (j = 1; j <= e->key->length; j++) {
				word.length = 0;
				for (k = j; k < e->key->length; k++)
					word.chars[word.length++] = e->key->chars[k];
				stateNum = hyphenHashLookup(hashTab, &word);
				if (stateNum != DEFAULTSTATE) break;
			}
			if (e->val) dict.states[e->val].fallbackState = stateNum;
		}
	}
	hyphenHashFree(hashTab);
	/* Transfer hyphenation information to table */
	for (i = 0; i < dict.numStates; i++) {
		if (dict.states[i].numTrans == 0)
			dict.states[i].trans.offset = 0;
		else {
			holdPointer = dict.states[i].trans.pointer;
			allocateSpaceInTranslationTable(file, &dict.states[i].trans.offset,
					dict.states[i].numTrans * sizeof(HyphenationTrans), table);
			memcpy(&(*table)->ruleArea[dict.states[i].trans.offset], holdPointer,
					dict.states[i].numTrans * sizeof(HyphenationTrans));
			free(holdPointer);
		}
	}
	allocateSpaceInTranslationTable(
			file, &holdOffset, dict.numStates * sizeof(HyphenationState), table);
	(*table)->hyphenStatesArray = holdOffset;
	/* Prevents segmentation fault if table is reallocated */
	memcpy(&(*table)->ruleArea[(*table)->hyphenStatesArray], &dict.states[0],
			dict.numStates * sizeof(HyphenationState));
	free(dict.states);
	return 1;
}