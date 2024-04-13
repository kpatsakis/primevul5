static const char *field_get_default(const char *data)
{
	const char *p;

	p = strchr(data, ':');
	if (p == NULL)
		return "";
	else {
		/* default value given */
		return p+1;
	}
}