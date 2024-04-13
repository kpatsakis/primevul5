close_socket(SocketEntry *e)
{
	close(e->fd);
	sshbuf_free(e->input);
	sshbuf_free(e->output);
	sshbuf_free(e->request);
	memset(e, '\0', sizeof(*e));
	e->fd = -1;
	e->type = AUTH_UNUSED;
}