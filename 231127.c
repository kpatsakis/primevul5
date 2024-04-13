bfad_im_drv_version_show(struct device *dev, struct device_attribute *attr,
				char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", BFAD_DRIVER_VERSION);
}