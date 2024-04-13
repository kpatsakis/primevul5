parseedge(struct scanner *s, struct environment *env)
{
	struct edge *e;
	struct evalstring *out, *in, *str, **end;
	char *name;
	struct string *val;
	struct node *n;
	size_t i;
	int p;

	e = mkedge(env);

	for (out = NULL, end = &out; (str = scanstring(s, true)); ++e->nout)
		pushstr(&end, str);
	e->outimpidx = e->nout;
	if (scanpipe(s, 1)) {
		for (; (str = scanstring(s, true)); ++e->nout)
			pushstr(&end, str);
	}
	if (e->nout == 0)
		scanerror(s, "expected output path");
	scanchar(s, ':');
	name = scanname(s);
	e->rule = envrule(env, name);
	if (!e->rule)
		fatal("undefined rule '%s'", name);
	free(name);
	for (in = NULL, end = &in; (str = scanstring(s, true)); ++e->nin)
		pushstr(&end, str);
	e->inimpidx = e->nin;
	p = scanpipe(s, 1 | 2);
	if (p == 1) {
		for (; (str = scanstring(s, true)); ++e->nin)
			pushstr(&end, str);
		p = scanpipe(s, 2);
	}
	e->inorderidx = e->nin;
	if (p == 2) {
		for (; (str = scanstring(s, true)); ++e->nin)
			pushstr(&end, str);
	}
	scannewline(s);
	while (scanindent(s)) {
		name = scanname(s);
		parselet(s, &str);
		val = enveval(env, str);
		envaddvar(e->env, name, val);
	}

	e->out = xreallocarray(NULL, e->nout, sizeof(e->out[0]));
	for (i = 0; i < e->nout; out = str) {
		str = out->next;
		val = enveval(e->env, out);
		canonpath(val);
		n = mknode(val);
		if (n->gen) {
			if (!parseopts.dupbuildwarn)
				fatal("multiple rules generate '%s'", n->path->s);
			warn("multiple rules generate '%s'", n->path->s);
			--e->nout;
			if (i < e->outimpidx)
				--e->outimpidx;
		} else {
			n->gen = e;
			e->out[i] = n;
			++i;
		}
	}

	e->in = xreallocarray(NULL, e->nin, sizeof(e->in[0]));
	for (i = 0; i < e->nin; in = str, ++i) {
		str = in->next;
		val = enveval(e->env, in);
		canonpath(val);
		n = mknode(val);
		e->in[i] = n;
		nodeuse(n, e);
	}

	val = edgevar(e, "pool", true);
	if (val)
		e->pool = poolget(val->s);
}