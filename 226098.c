static struct usb_function_instance *gprinter_alloc_inst(void)
{
	struct f_printer_opts *opts;
	struct usb_function_instance *ret;
	int status = 0;

	opts = kzalloc(sizeof(*opts), GFP_KERNEL);
	if (!opts)
		return ERR_PTR(-ENOMEM);

	mutex_init(&opts->lock);
	opts->func_inst.free_func_inst = gprinter_free_inst;
	ret = &opts->func_inst;

	mutex_lock(&printer_ida_lock);

	if (ida_is_empty(&printer_ida)) {
		status = gprinter_setup(PRINTER_MINORS);
		if (status) {
			ret = ERR_PTR(status);
			kfree(opts);
			goto unlock;
		}
	}

	opts->minor = gprinter_get_minor();
	if (opts->minor < 0) {
		ret = ERR_PTR(opts->minor);
		kfree(opts);
		if (ida_is_empty(&printer_ida))
			gprinter_cleanup();
		goto unlock;
	}
	config_group_init_type_name(&opts->func_inst.group, "",
				    &printer_func_type);

unlock:
	mutex_unlock(&printer_ida_lock);
	return ret;
}