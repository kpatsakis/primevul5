static ssize_t f_printer_opts_q_len_show(struct config_item *item,
					 char *page)
{
	struct f_printer_opts *opts = to_f_printer_opts(item);
	int result;

	mutex_lock(&opts->lock);
	result = sprintf(page, "%d\n", opts->q_len);
	mutex_unlock(&opts->lock);

	return result;
}