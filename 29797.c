channel_lookup(int id)
{
	Channel *c;

	if ((c = channel_by_id(id)) == NULL)
		return (NULL);

	switch (c->type) {
	case SSH_CHANNEL_X11_OPEN:
	case SSH_CHANNEL_LARVAL:
	case SSH_CHANNEL_CONNECTING:
	case SSH_CHANNEL_DYNAMIC:
	case SSH_CHANNEL_OPENING:
	case SSH_CHANNEL_OPEN:
	case SSH_CHANNEL_INPUT_DRAINING:
	case SSH_CHANNEL_OUTPUT_DRAINING:
	case SSH_CHANNEL_ABANDONED:
		return (c);
	}
	logit("Non-public channel %d, type %d.", id, c->type);
	return (NULL);
}
