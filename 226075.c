*to_f_printer_opts(struct config_item *item)
{
	return container_of(to_config_group(item), struct f_printer_opts,
			    func_inst.group);
}