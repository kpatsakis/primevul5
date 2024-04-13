static void enable_keepalive(int sockfd)
{
	int ka = 1;

	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &ka, sizeof(ka)) < 0)
		fprintf(stderr, "unable to set SO_KEEPALIVE on socket: %s\n",
			strerror(errno));
}