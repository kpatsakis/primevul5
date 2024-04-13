bfad_im_drv_name_show(struct device *dev, struct device_attribute *attr,
				char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", BFAD_DRIVER_NAME);
}