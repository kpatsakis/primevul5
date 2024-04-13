channel_by_id(int id)
{
	Channel *c;

	if (id < 0 || (u_int)id >= channels_alloc) {
		logit("channel_by_id: %d: bad id", id);
		return NULL;
	}
	c = channels[id];
	if (c == NULL) {
		logit("channel_by_id: %d: bad id: channel free", id);
		return NULL;
	}
	return c;
}
