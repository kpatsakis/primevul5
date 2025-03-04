parseinclude(struct scanner *s, struct environment *env, bool newscope)
{
	struct evalstring *str;
	struct string *path;

	str = scanstring(s, true);
	if (!str)
		scanerror(s, "expected include path");
	scannewline(s);
	path = enveval(env, str);

	if (newscope)
		env = mkenv(env);
	parse(path->s, env);
	free(path);
}