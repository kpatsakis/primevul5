channel_pre_input_draining(Channel *c, fd_set *readset, fd_set *writeset)
{
	if (buffer_len(&c->input) == 0) {
		packet_start(SSH_MSG_CHANNEL_CLOSE);
		packet_put_int(c->remote_id);
		packet_send();
		c->type = SSH_CHANNEL_CLOSED;
		debug2("channel %d: closing after input drain.", c->self);
	}
}
