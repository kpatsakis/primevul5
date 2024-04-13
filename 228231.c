parsepool(struct scanner *s, struct environment *env)
{
	struct pool *p;
	struct evalstring *val;
	struct string *str;
	char *var, *end;

	p = mkpool(scanname(s));
	scannewline(s);
	while (scanindent(s)) {
		var = scanname(s);
		parselet(s, &val);
		if (strcmp(var, "depth") == 0) {
			str = enveval(env, val);
			p->maxjobs = strtol(str->s, &end, 10);
			if (*end)
				fatal("invalid pool depth '%s'", str->s);
			free(str);
		} else {
			fatal("unexpected pool variable '%s'", var);
		}
	}
	if (!p->maxjobs)
		fatal("pool '%s' has no depth", p->name);
}