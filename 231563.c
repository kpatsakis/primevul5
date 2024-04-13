int cmd_grep(int argc, const char **argv, const char *prefix)
{
	int hit = 0;
	int cached = 0;
	int seen_dashdash = 0;
	struct grep_opt opt;
	struct object_array list = { 0, 0, NULL };
	const char **paths = NULL;
	int i;

	memset(&opt, 0, sizeof(opt));
	opt.prefix_length = (prefix && *prefix) ? strlen(prefix) : 0;
	opt.relative = 1;
	opt.pathname = 1;
	opt.pattern_tail = &opt.pattern_list;
	opt.regflags = REG_NEWLINE;

	/*
	 * If there is no -- then the paths must exist in the working
	 * tree.  If there is no explicit pattern specified with -e or
	 * -f, we take the first unrecognized non option to be the
	 * pattern, but then what follows it must be zero or more
	 * valid refs up to the -- (if exists), and then existing
	 * paths.  If there is an explicit pattern, then the first
	 * unrecognized non option is the beginning of the refs list
	 * that continues up to the -- (if exists), and then paths.
	 */

	while (1 < argc) {
		const char *arg = argv[1];
		argc--; argv++;
		if (!strcmp("--cached", arg)) {
			cached = 1;
			continue;
		}
		if (!strcmp("-a", arg) ||
		    !strcmp("--text", arg)) {
			opt.binary = GREP_BINARY_TEXT;
			continue;
		}
		if (!strcmp("-i", arg) ||
		    !strcmp("--ignore-case", arg)) {
			opt.regflags |= REG_ICASE;
			continue;
		}
		if (!strcmp("-I", arg)) {
			opt.binary = GREP_BINARY_NOMATCH;
			continue;
		}
		if (!strcmp("-v", arg) ||
		    !strcmp("--invert-match", arg)) {
			opt.invert = 1;
			continue;
		}
		if (!strcmp("-E", arg) ||
		    !strcmp("--extended-regexp", arg)) {
			opt.regflags |= REG_EXTENDED;
			continue;
		}
		if (!strcmp("-F", arg) ||
		    !strcmp("--fixed-strings", arg)) {
			opt.fixed = 1;
			continue;
		}
		if (!strcmp("-G", arg) ||
		    !strcmp("--basic-regexp", arg)) {
			opt.regflags &= ~REG_EXTENDED;
			continue;
		}
		if (!strcmp("-n", arg)) {
			opt.linenum = 1;
			continue;
		}
		if (!strcmp("-h", arg)) {
			opt.pathname = 0;
			continue;
		}
		if (!strcmp("-H", arg)) {
			opt.pathname = 1;
			continue;
		}
		if (!strcmp("-l", arg) ||
		    !strcmp("--name-only", arg) ||
		    !strcmp("--files-with-matches", arg)) {
			opt.name_only = 1;
			continue;
		}
		if (!strcmp("-L", arg) ||
		    !strcmp("--files-without-match", arg)) {
			opt.unmatch_name_only = 1;
			continue;
		}
		if (!strcmp("-c", arg) ||
		    !strcmp("--count", arg)) {
			opt.count = 1;
			continue;
		}
		if (!strcmp("-w", arg) ||
		    !strcmp("--word-regexp", arg)) {
			opt.word_regexp = 1;
			continue;
		}
		if (!prefixcmp(arg, "-A") ||
		    !prefixcmp(arg, "-B") ||
		    !prefixcmp(arg, "-C") ||
		    (arg[0] == '-' && '1' <= arg[1] && arg[1] <= '9')) {
			unsigned num;
			const char *scan;
			switch (arg[1]) {
			case 'A': case 'B': case 'C':
				if (!arg[2]) {
					if (argc <= 1)
						die(emsg_missing_context_len);
					scan = *++argv;
					argc--;
				}
				else
					scan = arg + 2;
				break;
			default:
				scan = arg + 1;
				break;
			}
			if (strtoul_ui(scan, 10, &num))
				die(emsg_invalid_context_len, scan);
			switch (arg[1]) {
			case 'A':
				opt.post_context = num;
				break;
			default:
			case 'C':
				opt.post_context = num;
			case 'B':
				opt.pre_context = num;
				break;
			}
			continue;
		}
		if (!strcmp("-f", arg)) {
			FILE *patterns;
			int lno = 0;
			char buf[1024];
			if (argc <= 1)
				die(emsg_missing_argument, arg);
			patterns = fopen(argv[1], "r");
			if (!patterns)
				die("'%s': %s", argv[1], strerror(errno));
			while (fgets(buf, sizeof(buf), patterns)) {
				int len = strlen(buf);
				if (len && buf[len-1] == '\n')
					buf[len-1] = 0;
				/* ignore empty line like grep does */
				if (!buf[0])
					continue;
				append_grep_pattern(&opt, xstrdup(buf),
						    argv[1], ++lno,
						    GREP_PATTERN);
			}
			fclose(patterns);
			argv++;
			argc--;
			continue;
		}
		if (!strcmp("--not", arg)) {
			append_grep_pattern(&opt, arg, "command line", 0,
					    GREP_NOT);
			continue;
		}
		if (!strcmp("--and", arg)) {
			append_grep_pattern(&opt, arg, "command line", 0,
					    GREP_AND);
			continue;
		}
		if (!strcmp("--or", arg))
			continue; /* no-op */
		if (!strcmp("(", arg)) {
			append_grep_pattern(&opt, arg, "command line", 0,
					    GREP_OPEN_PAREN);
			continue;
		}
		if (!strcmp(")", arg)) {
			append_grep_pattern(&opt, arg, "command line", 0,
					    GREP_CLOSE_PAREN);
			continue;
		}
		if (!strcmp("--all-match", arg)) {
			opt.all_match = 1;
			continue;
		}
		if (!strcmp("-e", arg)) {
			if (1 < argc) {
				append_grep_pattern(&opt, argv[1],
						    "-e option", 0,
						    GREP_PATTERN);
				argv++;
				argc--;
				continue;
			}
			die(emsg_missing_argument, arg);
		}
		if (!strcmp("--full-name", arg)) {
			opt.relative = 0;
			continue;
		}
		if (!strcmp("--", arg)) {
			/* later processing wants to have this at argv[1] */
			argv--;
			argc++;
			break;
		}
		if (*arg == '-')
			usage(builtin_grep_usage);

		/* First unrecognized non-option token */
		if (!opt.pattern_list) {
			append_grep_pattern(&opt, arg, "command line", 0,
					    GREP_PATTERN);
			break;
		}
		else {
			/* We are looking at the first path or rev;
			 * it is found at argv[1] after leaving the
			 * loop.
			 */
			argc++; argv--;
			break;
		}
	}

	if (!opt.pattern_list)
		die("no pattern given.");
	if ((opt.regflags != REG_NEWLINE) && opt.fixed)
		die("cannot mix --fixed-strings and regexp");
	compile_grep_patterns(&opt);

	/* Check revs and then paths */
	for (i = 1; i < argc; i++) {
		const char *arg = argv[i];
		unsigned char sha1[20];
		/* Is it a rev? */
		if (!get_sha1(arg, sha1)) {
			struct object *object = parse_object(sha1);
			if (!object)
				die("bad object %s", arg);
			add_object_array(object, arg, &list);
			continue;
		}
		if (!strcmp(arg, "--")) {
			i++;
			seen_dashdash = 1;
		}
		break;
	}

	/* The rest are paths */
	if (!seen_dashdash) {
		int j;
		for (j = i; j < argc; j++)
			verify_filename(prefix, argv[j]);
	}

	if (i < argc) {
		paths = get_pathspec(prefix, argv + i);
		if (opt.prefix_length && opt.relative) {
			/* Make sure we do not get outside of paths */
			for (i = 0; paths[i]; i++)
				if (strncmp(prefix, paths[i], opt.prefix_length))
					die("git-grep: cannot generate relative filenames containing '..'");
		}
	}
	else if (prefix) {
		paths = xcalloc(2, sizeof(const char *));
		paths[0] = prefix;
		paths[1] = NULL;
	}

	if (!list.nr)
		return !grep_cache(&opt, paths, cached);

	if (cached)
		die("both --cached and trees are given.");

	for (i = 0; i < list.nr; i++) {
		struct object *real_obj;
		real_obj = deref_tag(list.objects[i].item, NULL, 0);
		if (grep_object(&opt, paths, real_obj, list.objects[i].name))
			hit = 1;
	}
	free_grep_patterns(&opt);
	return !hit;
}