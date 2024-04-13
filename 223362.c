static void parse_output_words(struct output_struct *words, short *levels,
			       const char *str, uchar priority)
{
	const char *s;
	int j, len, lev;

	for ( ; str; str = s) {
		if ((s = strchr(str, ',')) != NULL)
			len = s++ - str;
		else
			len = strlen(str);
		if (!len)
			continue;
		if (!isDigit(str)) {
			while (len && isDigit(str+len-1))
				len--;
		}
		lev = isDigit(str+len) ? atoi(str+len) : 1;
		if (lev > MAX_OUT_LEVEL)
			lev = MAX_OUT_LEVEL;
		if (len == 4 && strncasecmp(str, "help", 4) == 0) {
			output_item_help(words);
			exit_cleanup(0);
		}
		if (len == 4 && strncasecmp(str, "none", 4) == 0)
			len = lev = 0;
		else if (len == 3 && strncasecmp(str, "all", 3) == 0)
			len = 0;
		for (j = 0; words[j].name; j++) {
			if (!len
			 || (len == words[j].namelen && strncasecmp(str, words[j].name, len) == 0)) {
				if (priority >= words[j].priority) {
					words[j].priority = priority;
					levels[j] = lev;
				}
				if (len)
					break;
			}
		}
		if (len && !words[j].name) {
			rprintf(FERROR, "Unknown %s item: \"%.*s\"\n",
				words[j].help, len, str);
			exit_cleanup(RERR_SYNTAX);
		}
	}
}