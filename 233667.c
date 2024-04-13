ssize_t util_get_sys_subsystem(struct udev *udev, const char *syspath, char *subsystem, size_t size)
{
	return get_sys_link(udev, "subsystem", syspath, subsystem, size);
}