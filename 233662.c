ssize_t util_get_sys_driver(struct udev *udev, const char *syspath, char *driver, size_t size)
{
	return get_sys_link(udev, "driver", syspath, driver, size);
}