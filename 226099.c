static ssize_t f_printer_opts_pnp_string_store(struct config_item *item,
					       const char *page, size_t len)
{
	struct f_printer_opts *opts = to_f_printer_opts(item);
	char *new_pnp;
	int result;

	mutex_lock(&opts->lock);

	new_pnp = kstrndup(page, len, GFP_KERNEL);
	if (!new_pnp) {
		result = -ENOMEM;
		goto unlock;
	}

	if (opts->pnp_string_allocated)
		kfree(opts->pnp_string);

	opts->pnp_string_allocated = true;
	opts->pnp_string = new_pnp;
	result = len;
unlock:
	mutex_unlock(&opts->lock);

	return result;
}