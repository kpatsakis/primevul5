channel_close_all(void)
{
	u_int i;

	for (i = 0; i < channels_alloc; i++)
		if (channels[i] != NULL)
			channel_close_fds(channels[i]);
}
