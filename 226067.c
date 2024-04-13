static void printer_attr_release(struct config_item *item)
{
	struct f_printer_opts *opts = to_f_printer_opts(item);

	usb_put_function_instance(&opts->func_inst);
}