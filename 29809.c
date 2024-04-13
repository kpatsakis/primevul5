channel_send_open(int id)
{
	Channel *c = channel_lookup(id);

	if (c == NULL) {
		logit("channel_send_open: %d: bad id", id);
		return;
	}
	debug2("channel %d: send open", id);
	packet_start(SSH2_MSG_CHANNEL_OPEN);
	packet_put_cstring(c->ctype);
	packet_put_int(c->self);
	packet_put_int(c->local_window);
	packet_put_int(c->local_maxpacket);
	packet_send();
}
