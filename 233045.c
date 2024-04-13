static int sock_read_ready(SSL *ssl, uint32_t ms)
{
	int r = 0;
	fd_set fds;
	struct timeval tv;

	FD_ZERO(&fds);

	FD_SET(SSL_get_fd(ssl), &fds);

	tv.tv_sec = ms / 1000;
	tv.tv_usec = (ms % 1000) * 1000;
	
	r = select (SSL_get_fd(ssl) + 1, &fds, NULL, NULL, &tv); 

	return r;
}