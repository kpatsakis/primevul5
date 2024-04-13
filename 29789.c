channel_cancel_cleanup(int id)
{
	Channel *c = channel_by_id(id);

	if (c == NULL) {
		logit("channel_cancel_cleanup: %d: bad id", id);
		return;
	}
	c->detach_user = NULL;
	c->detach_close = 0;
}
