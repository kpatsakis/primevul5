static int flush_grep(struct grep_opt *opt,
		      int argc, int arg0, const char **argv, int *kept)
{
	int status;
	int count = argc - arg0;
	const char *kept_0 = NULL;

	if (count <= 2) {
		/*
		 * Because we keep at least 2 paths in the call from
		 * the main loop (i.e. kept != NULL), and MAXARGS is
		 * far greater than 2, this usually is a call to
		 * conclude the grep.  However, the user could attempt
		 * to overflow the argv buffer by giving too many
		 * options to leave very small number of real
		 * arguments even for the call in the main loop.
		 */
		if (kept)
			die("insanely many options to grep");

		/*
		 * If we have two or more paths, we do not have to do
		 * anything special, but we need to push /dev/null to
		 * get "-H" behaviour of GNU grep portably but when we
		 * are not doing "-l" nor "-L" nor "-c".
		 */
		if (count == 1 &&
		    !opt->name_only &&
		    !opt->unmatch_name_only &&
		    !opt->count) {
			argv[argc++] = "/dev/null";
			argv[argc] = NULL;
		}
	}

	else if (kept) {
		/*
		 * Called because we found many paths and haven't finished
		 * iterating over the cache yet.  We keep two paths
		 * for the concluding call.  argv[argc-2] and argv[argc-1]
		 * has the last two paths, so save the first one away,
		 * replace it with NULL while sending the list to grep,
		 * and recover them after we are done.
		 */
		*kept = 2;
		kept_0 = argv[argc-2];
		argv[argc-2] = NULL;
		argc -= 2;
	}

	status = exec_grep(argc, argv);

	if (kept_0) {
		/*
		 * Then recover them.  Now the last arg is beyond the
		 * terminating NULL which is at argc, and the second
		 * from the last is what we saved away in kept_0
		 */
		argv[arg0++] = kept_0;
		argv[arg0] = argv[argc+1];
	}
	return status;
}