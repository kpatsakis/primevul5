int util_resolve_sys_link(struct udev *udev, char *syspath, size_t size)
{
	char link_target[UTIL_PATH_SIZE];

	int len;
	int i;
	int back;

	len = readlink(syspath, link_target, sizeof(link_target));
	if (len <= 0)
		return -1;
	link_target[len] = '\0';
	dbg(udev, "path link '%s' points to '%s'\n", syspath, link_target);

	for (back = 0; strncmp(&link_target[back * 3], "../", 3) == 0; back++)
		;
	dbg(udev, "base '%s', tail '%s', back %i\n", syspath, &link_target[back * 3], back);
	for (i = 0; i <= back; i++) {
		char *pos = strrchr(syspath, '/');

		if (pos == NULL)
			return -1;
		pos[0] = '\0';
	}
	dbg(udev, "after moving back '%s'\n", syspath);
	util_strlcat(syspath, "/", size);
	util_strlcat(syspath, &link_target[back * 3], size);
	return 0;
}