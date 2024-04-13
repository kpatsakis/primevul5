parsedefault(struct scanner *s, struct environment *env)
{
	struct evalstring *targ, *str, **end;
	struct string *path;
	struct node *n;
	size_t ntarg;

	for (targ = NULL, ntarg = 0, end = &targ; (str = scanstring(s, true)); ++ntarg)
		pushstr(&end, str);
	deftarg = xreallocarray(deftarg, ndeftarg + ntarg, sizeof(*deftarg));
	for (; targ; targ = str) {
		str = targ->next;
		path = enveval(env, targ);
		canonpath(path);
		n = nodeget(path->s, path->n);
		if (!n)
			fatal("unknown target '%s'", path->s);
		free(path);
		deftarg[ndeftarg++] = n;
	}
	scannewline(s);
}