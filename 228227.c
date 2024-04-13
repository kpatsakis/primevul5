parse(const char *name, struct environment *env)
{
	struct scanner s;
	char *var;
	struct string *val;
	struct evalstring *str;

	scaninit(&s, name);
	for (;;) {
		switch (scankeyword(&s, &var)) {
		case RULE:
			parserule(&s, env);
			break;
		case BUILD:
			parseedge(&s, env);
			break;
		case INCLUDE:
			parseinclude(&s, env, false);
			break;
		case SUBNINJA:
			parseinclude(&s, env, true);
			break;
		case DEFAULT:
			parsedefault(&s, env);
			break;
		case POOL:
			parsepool(&s, env);
			break;
		case VARIABLE:
			parselet(&s, &str);
			val = enveval(env, str);
			if (strcmp(var, "ninja_required_version") == 0)
				checkversion(val->s);
			envaddvar(env, var, val);
			break;
		case EOF:
			scanclose(&s);
			return;
		}
	}
}