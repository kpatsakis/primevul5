my_atoi(const char *p)
{
	unsigned cp;

	cp = 0;
	while (*p) {
		if (*p >= '0' && *p <= '9')
			cp = cp * 10 + (*p - '0');
		else
			return (-1);
		p++;
	}
	return (cp);
}