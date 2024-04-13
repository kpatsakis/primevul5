channel_register_cleanup(int id, channel_callback_fn *fn, int do_close)
{
	Channel *c = channel_by_id(id);

	if (c == NULL) {
		logit("channel_register_cleanup: %d: bad id", id);
		return;
	}
	c->detach_user = fn;
	c->detach_close = do_close;
}
