static int external_grep(struct grep_opt *opt, const char **paths, int cached)
{
	int i, nr, argc, hit, len, status;
	const char *argv[MAXARGS+1];
	char randarg[ARGBUF];
	char *argptr = randarg;
	struct grep_pat *p;

	if (opt->extended || (opt->relative && opt->prefix_length))
		return -1;
	len = nr = 0;
	push_arg("grep");
	if (opt->fixed)
		push_arg("-F");
	if (opt->linenum)
		push_arg("-n");
	if (!opt->pathname)
		push_arg("-h");
	if (opt->regflags & REG_EXTENDED)
		push_arg("-E");
	if (opt->regflags & REG_ICASE)
		push_arg("-i");
	if (opt->word_regexp)
		push_arg("-w");
	if (opt->name_only)
		push_arg("-l");
	if (opt->unmatch_name_only)
		push_arg("-L");
	if (opt->count)
		push_arg("-c");
	if (opt->post_context || opt->pre_context) {
		if (opt->post_context != opt->pre_context) {
			if (opt->pre_context) {
				push_arg("-B");
				len += snprintf(argptr, sizeof(randarg)-len,
						"%u", opt->pre_context) + 1;
				if (sizeof(randarg) <= len)
					die("maximum length of args exceeded");
				push_arg(argptr);
				argptr += len;
			}
			if (opt->post_context) {
				push_arg("-A");
				len += snprintf(argptr, sizeof(randarg)-len,
						"%u", opt->post_context) + 1;
				if (sizeof(randarg) <= len)
					die("maximum length of args exceeded");
				push_arg(argptr);
				argptr += len;
			}
		}
		else {
			push_arg("-C");
			len += snprintf(argptr, sizeof(randarg)-len,
					"%u", opt->post_context) + 1;
			if (sizeof(randarg) <= len)
				die("maximum length of args exceeded");
			push_arg(argptr);
			argptr += len;
		}
	}
	for (p = opt->pattern_list; p; p = p->next) {
		push_arg("-e");
		push_arg(p->pattern);
	}

	hit = 0;
	argc = nr;
	for (i = 0; i < active_nr; i++) {
		struct cache_entry *ce = active_cache[i];
		char *name;
		int kept;
		if (!S_ISREG(ce->ce_mode))
			continue;
		if (!pathspec_matches(paths, ce->name))
			continue;
		name = ce->name;
		if (name[0] == '-') {
			int len = ce_namelen(ce);
			name = xmalloc(len + 3);
			memcpy(name, "./", 2);
			memcpy(name + 2, ce->name, len + 1);
		}
		argv[argc++] = name;
		if (MAXARGS <= argc) {
			status = flush_grep(opt, argc, nr, argv, &kept);
			if (0 < status)
				hit = 1;
			argc = nr + kept;
		}
		if (ce_stage(ce)) {
			do {
				i++;
			} while (i < active_nr &&
				 !strcmp(ce->name, active_cache[i]->name));
			i--; /* compensate for loop control */
		}
	}
	if (argc > nr) {
		status = flush_grep(opt, argc, nr, argv, NULL);
		if (0 < status)
			hit = 1;
	}
	return hit;
}