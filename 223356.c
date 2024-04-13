static char *make_output_option(struct output_struct *words, short *levels, uchar where)
{
	char *str = words == info_words ? "--info=" : "--debug=";
	int j, counts[MAX_OUT_LEVEL+1], pos, skipped = 0, len = 0, max = 0, lev = 0;
	int word_count = words == info_words ? COUNT_INFO : COUNT_DEBUG;
	char *buf;

	memset(counts, 0, sizeof counts);

	for (j = 0; words[j].name; j++) {
		if (words[j].flag != j) {
			rprintf(FERROR, "rsync: internal error on %s%s: %d != %d\n",
				words == info_words ? "INFO_" : "DEBUG_",
				words[j].name, words[j].flag, j);
			exit_cleanup(RERR_UNSUPPORTED);
		}
		if (!(words[j].where & where))
			continue;
		if (words[j].priority == DEFAULT_PRIORITY) {
			/* Implied items don't need to be mentioned. */
			skipped++;
			continue;
		}
		len += len ? 1 : strlen(str);
		len += strlen(words[j].name);
		len += levels[j] == 1 ? 0 : 1;

		if (words[j].priority == HELP_PRIORITY)
			continue; /* no abbreviating for help */

		assert(levels[j] <= MAX_OUT_LEVEL);
		if (++counts[levels[j]] > max) {
			/* Determine which level has the most items. */
			lev = levels[j];
			max = counts[lev];
		}
	}

	/* Sanity check the COUNT_* define against the length of the table. */
	if (j != word_count) {
		rprintf(FERROR, "rsync: internal error: %s is wrong! (%d != %d)\n",
			words == info_words ? "COUNT_INFO" : "COUNT_DEBUG",
			j, word_count);
		exit_cleanup(RERR_UNSUPPORTED);
	}

	if (!len)
		return NULL;

	len++;
	if (!(buf = new_array(char, len)))
		out_of_memory("make_output_option");
	pos = 0;

	if (skipped || max < 5)
		lev = -1;
	else {
		if (lev == 0)
			pos += snprintf(buf, len, "%sNONE", str);
		else if (lev == 1)
			pos += snprintf(buf, len, "%sALL", str);
		else
			pos += snprintf(buf, len, "%sALL%d", str, lev);
	}

	for (j = 0; words[j].name && pos < len; j++) {
		if (words[j].priority == DEFAULT_PRIORITY || levels[j] == lev || !(words[j].where & where))
			continue;
		if (pos)
			buf[pos++] = ',';
		else
			pos += strlcpy(buf+pos, str, len-pos);
		if (pos < len)
			pos += strlcpy(buf+pos, words[j].name, len-pos);
		/* Level 1 is implied by the name alone. */
		if (levels[j] != 1 && pos < len)
			buf[pos++] = '0' + levels[j];
	}

	buf[pos] = '\0';

	return buf;
}