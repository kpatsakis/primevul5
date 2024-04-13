static int isAnonGraph(char *name)
{
    if (*name++ != '%')
	return 0;
    while (isdigit(*name))
	name++;			/* skip over digits */
    return (*name == '\0');
}