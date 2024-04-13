strlen_tex(const char *str)
{
    const char *s = str;
    int len = 0;

    if (!strpbrk(s, "{}$[]\\")) {
	len = strlen(s);
	FPRINTF((stderr,"strlen_tex(\"%s\") = %d\n",s,len));
	return len;
    }

    while (*s) {
	switch (*s) {
	case '[':
		while (*s && *s != ']') s++;
		if (*s) s++;
		break;
	case '\\':
		s++;
		while (*s && isalpha((unsigned char)*s)) s++;
		len++;
		break;
	case '{':
	case '}':
	case '$':
	case '_':
	case '^':
		s++;
		break;
	default:
		s++;
		len++;
	}
    }


    FPRINTF((stderr,"strlen_tex(\"%s\") = %d\n",str,len));
    return len;
}