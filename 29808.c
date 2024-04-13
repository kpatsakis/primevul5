channel_request_start(int id, char *service, int wantconfirm)
{
	Channel *c = channel_lookup(id);

	if (c == NULL) {
		logit("channel_request_start: %d: unknown channel id", id);
		return;
	}
	debug2("channel %d: request %s confirm %d", id, service, wantconfirm);
	packet_start(SSH2_MSG_CHANNEL_REQUEST);
	packet_put_int(c->remote_id);
	packet_put_cstring(service);
	packet_put_char(wantconfirm);
}
