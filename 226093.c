static ssize_t f_printer_opts_q_len_store(struct config_item *item,
					  const char *page, size_t len)
{
	struct f_printer_opts *opts = to_f_printer_opts(item);
	int ret;
	u16 num;

	mutex_lock(&opts->lock);
	if (opts->refcnt) {
		ret = -EBUSY;
		goto end;
	}

	ret = kstrtou16(page, 0, &num);
	if (ret)
		goto end;

	opts->q_len = (unsigned)num;
	ret = len;
end:
	mutex_unlock(&opts->lock);
	return ret;
}