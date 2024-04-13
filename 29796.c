channel_free_all(void)
{
	u_int i;

	for (i = 0; i < channels_alloc; i++)
		if (channels[i] != NULL)
			channel_free(channels[i]);
}
