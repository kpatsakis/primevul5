static inline int gprinter_get_minor(void)
{
	int ret;

	ret = ida_simple_get(&printer_ida, 0, 0, GFP_KERNEL);
	if (ret >= PRINTER_MINORS) {
		ida_simple_remove(&printer_ida, ret);
		ret = -ENODEV;
	}

	return ret;
}