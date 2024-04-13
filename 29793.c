channel_find_maxfd(void)
{
	u_int i;
	int max = 0;
	Channel *c;

	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c != NULL) {
			max = MAX(max, c->rfd);
			max = MAX(max, c->wfd);
			max = MAX(max, c->efd);
		}
	}
	return max;
}
