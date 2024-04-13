static void set_refuse_options(char *bp)
{
	struct poptOption *op;
	char *cp, shortname[2];
	int is_wild, found_match;

	shortname[1] = '\0';

	while (1) {
		while (*bp == ' ') bp++;
		if (!*bp)
			break;
		if ((cp = strchr(bp, ' ')) != NULL)
			*cp= '\0';
		is_wild = strpbrk(bp, "*?[") != NULL;
		found_match = 0;
		for (op = long_options; ; op++) {
			*shortname = op->shortName;
			if (!op->longName && !*shortname)
				break;
			if ((op->longName && wildmatch(bp, op->longName))
			    || (*shortname && wildmatch(bp, shortname))) {
				if (op->argInfo == POPT_ARG_VAL)
					op->argInfo = POPT_ARG_NONE;
				op->val = (op - long_options) + OPT_REFUSED_BASE;
				found_match = 1;
				/* These flags are set to let us easily check
				 * an implied option later in the code. */
				switch (*shortname) {
				case 'r': case 'd': case 'l': case 'p':
				case 't': case 'g': case 'o': case 'D':
					refused_archive_part = op->val;
					break;
				case 'z':
					refused_compress = op->val;
					break;
				case '\0':
					if (wildmatch("delete", op->longName))
						refused_delete = op->val;
					else if (wildmatch("delete-before", op->longName))
						refused_delete_before = op->val;
					else if (wildmatch("delete-during", op->longName))
						refused_delete_during = op->val;
					else if (wildmatch("partial", op->longName))
						refused_partial = op->val;
					else if (wildmatch("progress", op->longName))
						refused_progress = op->val;
					else if (wildmatch("inplace", op->longName))
						refused_inplace = op->val;
					else if (wildmatch("no-iconv", op->longName))
						refused_no_iconv = op->val;
					break;
				}
				if (!is_wild)
					break;
			}
		}
		if (!found_match) {
			rprintf(FLOG, "No match for refuse-options string \"%s\"\n",
				bp);
		}
		if (!cp)
			break;
		*cp = ' ';
		bp = cp + 1;
	}
}