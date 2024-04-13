static void gprinter_free_inst(struct usb_function_instance *f)
{
	struct f_printer_opts *opts;

	opts = container_of(f, struct f_printer_opts, func_inst);

	mutex_lock(&printer_ida_lock);

	gprinter_put_minor(opts->minor);
	if (ida_is_empty(&printer_ida))
		gprinter_cleanup();

	mutex_unlock(&printer_ida_lock);

	if (opts->pnp_string_allocated)
		kfree(opts->pnp_string);
	kfree(opts);
}