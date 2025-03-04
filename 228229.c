parserule(struct scanner *s, struct environment *env)
{
	struct rule *r;
	char *var;
	struct evalstring *val;
	bool hascommand = false, hasrspfile = false, hasrspcontent = false;

	r = mkrule(scanname(s));
	scannewline(s);
	while (scanindent(s)) {
		var = scanname(s);
		parselet(s, &val);
		ruleaddvar(r, var, val);
		if (!val)
			continue;
		if (strcmp(var, "command") == 0)
			hascommand = true;
		else if (strcmp(var, "rspfile") == 0)
			hasrspfile = true;
		else if (strcmp(var, "rspfile_content") == 0)
			hasrspcontent = true;
	}
	if (!hascommand)
		fatal("rule '%s' has no command", r->name);
	if (hasrspfile != hasrspcontent)
		fatal("rule '%s' has rspfile and no rspfile_content or vice versa", r->name);
	envaddrule(env, r);
}