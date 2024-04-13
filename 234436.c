escape_reserved_chars(const char *str, const char *reserved)
{
    int i;
    char *escaped_str;
    int newsize = strlen(str);

	/* Count number of reserved characters */
	for (i = 0; str[i] != '\0'; i++) {
	    if (strchr(reserved, str[i]))
		newsize++;
	}
	escaped_str = gp_alloc(newsize + 1, NULL);

	/* Prefix each reserved character with a backslash */
	for (i = 0, newsize = 0; str[i] != '\0'; i++) {
	    if (strchr(reserved, str[i]))
		escaped_str[newsize++] = '\\';
	    escaped_str[newsize++] = str[i];
	}
	escaped_str[newsize] = '\0';

    return escaped_str;
}