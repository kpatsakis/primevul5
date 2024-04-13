static ssize_t f_printer_opts_pnp_string_show(struct config_item *item,
					      char *page)
{
	struct f_printer_opts *opts = to_f_printer_opts(item);
	int result = 0;

	mutex_lock(&opts->lock);
	if (!opts->pnp_string)
		goto unlock;

	result = strlcpy(page, opts->pnp_string, PAGE_SIZE);
	if (result >= PAGE_SIZE) {
		result = PAGE_SIZE;
	} else if (page[result - 1] != '\n' && result + 1 < PAGE_SIZE) {
		page[result++] = '\n';
		page[result] = '\0';
	}

unlock:
	mutex_unlock(&opts->lock);

	return result;
}