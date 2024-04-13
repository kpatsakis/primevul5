static ssize_t get_sys_link(struct udev *udev, const char *slink, const char *syspath, char *value, size_t size)
{
	char path[UTIL_PATH_SIZE];
	ssize_t len;
	const char *pos;

	util_strlcpy(path, syspath, sizeof(path));
	util_strlcat(path, "/", sizeof(path));
	util_strlcat(path, slink, sizeof(path));
	len = readlink(path, path, sizeof(path));
	if (len < 0 || len >= (ssize_t) sizeof(path))
		return -1;
	path[len] = '\0';
	pos = strrchr(path, '/');
	if (pos == NULL)
		return -1;
	pos = &pos[1];
	dbg(udev, "resolved link to: '%s'\n", pos);
	return util_strlcpy(value, pos, size);
}