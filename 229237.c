format_timestamp_name(char *path, size_t len,
		      const char *timestamp_dir,
		      const char *tty,
		      const char *ruser,
		      const char *user)
{
	if (strcmp(ruser, user) == 0) {
		return snprintf(path, len, "%s/%s/%s", timestamp_dir,
				ruser, tty);
	} else {
		return snprintf(path, len, "%s/%s/%s:%s", timestamp_dir,
				ruser, tty, user);
	}
}