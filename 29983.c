static char *clean_path(TALLOC_CTX *ctx, const char *path)
{
	size_t len;
	char *p1, *p2, *p;
	char *path_out;

	/* No absolute paths. */
	while (IS_DIRECTORY_SEP(*path)) {
		path++;
	}

	path_out = talloc_strdup(ctx, path);
	if (!path_out) {
		return NULL;
	}

	p1 = strchr_m(path_out, '*');
	p2 = strchr_m(path_out, '?');

	if (p1 || p2) {
		if (p1 && p2) {
			p = MIN(p1,p2);
		} else if (!p1) {
			p = p2;
		} else {
			p = p1;
		}
		*p = '\0';

		/* Now go back to the start of this component. */
		p1 = strrchr_m(path_out, '/');
		p2 = strrchr_m(path_out, '\\');
		p = MAX(p1,p2);
		if (p) {
			*p = '\0';
		}
	}

	/* Strip any trailing separator */

	len = strlen(path_out);
	if ( (len > 0) && IS_DIRECTORY_SEP(path_out[len-1])) {
		path_out[len-1] = '\0';
	}

	return path_out;
}
