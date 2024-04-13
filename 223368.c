static char *parse_hostspec(char *str, char **path_start_ptr, int *port_ptr)
{
	char *s, *host_start = str;
	int hostlen = 0, userlen = 0;
	char *ret;

	for (s = str; ; s++) {
		if (!*s) {
			/* It is only OK if we run out of string with rsync:// */
			if (!port_ptr)
				return NULL;
			if (!hostlen)
				hostlen = s - host_start;
			break;
		}
		if (*s == ':' || *s == '/') {
			if (!hostlen)
				hostlen = s - host_start;
			if (*s++ == '/') {
				if (!port_ptr)
					return NULL;
			} else if (port_ptr) {
				*port_ptr = atoi(s);
				while (isDigit(s)) s++;
				if (*s && *s++ != '/')
					return NULL;
			}
			break;
		}
		if (*s == '@') {
			userlen = s - str + 1;
			host_start = s + 1;
		} else if (*s == '[') {
			if (s != host_start++)
				return NULL;
			while (*s && *s != ']' && *s != '/') s++; /*SHARED ITERATOR*/
			hostlen = s - host_start;
			if (*s != ']' || (s[1] && s[1] != '/' && s[1] != ':') || !hostlen)
				return NULL;
		}
	}

	*path_start_ptr = s;
	ret = new_array(char, userlen + hostlen + 1);
	if (userlen)
		strlcpy(ret, str, userlen + 1);
	strlcpy(ret + userlen, host_start, hostlen + 1);
	return ret;
}