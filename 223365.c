char *check_for_hostspec(char *s, char **host_ptr, int *port_ptr)
{
	char *path;

	if (port_ptr && strncasecmp(URL_PREFIX, s, strlen(URL_PREFIX)) == 0) {
		*host_ptr = parse_hostspec(s + strlen(URL_PREFIX), &path, port_ptr);
		if (*host_ptr) {
			if (!*port_ptr)
				*port_ptr = RSYNC_PORT;
			return path;
		}
	}

	*host_ptr = parse_hostspec(s, &path, NULL);
	if (!*host_ptr)
		return NULL;

	if (*path == ':') {
		if (port_ptr && !*port_ptr)
			*port_ptr = RSYNC_PORT;
		return path + 1;
	}
	if (port_ptr)
		*port_ptr = 0;

	return path;
}