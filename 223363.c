static void output_item_help(struct output_struct *words)
{
	short *levels = words == info_words ? info_levels : debug_levels;
	const char **verbosity = words == info_words ? info_verbosity : debug_verbosity;
	char buf[128], *opt, *fmt = "%-10s %s\n";
	int j;

	reset_output_levels();

	rprintf(FINFO, "Use OPT or OPT1 for level 1 output, OPT2 for level 2, etc.; OPT0 silences.\n");
	rprintf(FINFO, "\n");
	for (j = 0; words[j].name; j++)
		rprintf(FINFO, fmt, words[j].name, words[j].help);
	rprintf(FINFO, "\n");

	snprintf(buf, sizeof buf, "Set all %s options (e.g. all%d)",
		 words[j].help, MAX_OUT_LEVEL);
	rprintf(FINFO, fmt, "ALL", buf);

	snprintf(buf, sizeof buf, "Silence all %s options (same as all0)",
		 words[j].help);
	rprintf(FINFO, fmt, "NONE", buf);

	rprintf(FINFO, fmt, "HELP", "Output this help message");
	rprintf(FINFO, "\n");
	rprintf(FINFO, "Options added for each increase in verbose level:\n");

	for (j = 1; j <= MAX_VERBOSITY; j++) {
		parse_output_words(words, levels, verbosity[j], HELP_PRIORITY);
		opt = make_output_option(words, levels, W_CLI|W_SRV|W_SND|W_REC);
		if (opt) {
			rprintf(FINFO, "%d) %s\n", j, strchr(opt, '=')+1);
			free(opt);
		}
		reset_output_levels();
	}
}