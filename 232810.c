static int is_printable(char p)
{
	if ((p >= 'a' && p <= 'z') || (p >= 'A' && p <= 'Z') ||
	    (p >= '0' && p <= '9') || p == ' ' || p == '(' || p == ')' ||
	    p == '+' || p == ',' || p == '-' || p == '.' || p == '/' ||
	    p == ':' || p == '=' || p == '?')
		return 1;

	return 0;
}