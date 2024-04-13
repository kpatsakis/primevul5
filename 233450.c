static char *get_val(char *p, float *v)
{
	char tmp[32], *r = tmp;

	while (isspace((unsigned char) *p))
		p++;
	while ((isdigit((unsigned char) *p) && r < &tmp[32 - 1])
	    || *p == '-' || *p == '.')
		*r++ = *p++;
	*r = '\0';
	sscanf(tmp, "%f", v);
	return p;
}