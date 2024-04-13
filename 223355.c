static void popt_unalias(poptContext con, const char *opt)
{
	struct poptAlias unalias;

	unalias.longName = opt + 2; /* point past the leading "--" */
	unalias.shortName = '\0';
	unalias.argc = 1;
	unalias.argv = new_array(const char*, 1);
	unalias.argv[0] = strdup(opt);

	poptAddAlias(con, unalias, 0);
}