const char *cgit_repobasename(const char *reponame)
{
	/* I assume we don't need to store more than one repo basename */
	static char rvbuf[1024];
	int p;
	const char *rv;
	strncpy(rvbuf, reponame, sizeof(rvbuf));
	if (rvbuf[sizeof(rvbuf)-1])
		die("cgit_repobasename: truncated repository name '%s'", reponame);
	p = strlen(rvbuf)-1;
	/* strip trailing slashes */
	while (p && rvbuf[p] == '/') rvbuf[p--] = 0;
	/* strip trailing .git */
	if (p >= 3 && starts_with(&rvbuf[p-3], ".git")) {
		p -= 3; rvbuf[p--] = 0;
	}
	/* strip more trailing slashes if any */
	while ( p && rvbuf[p] == '/') rvbuf[p--] = 0;
	/* find last slash in the remaining string */
	rv = strrchr(rvbuf,'/');
	if (rv)
		return ++rv;
	return rvbuf;
}