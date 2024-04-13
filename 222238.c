p11_rpc_buffer_new_full (size_t reserve,
                         void * (* frealloc) (void *data, size_t size),
                         void (* ffree) (void *data))
{
	p11_buffer *buffer;

	buffer = calloc (1, sizeof (p11_buffer));
	return_val_if_fail (buffer != NULL, NULL);

	p11_buffer_init_full (buffer, NULL, 0, 0, frealloc, ffree);
	if (!p11_buffer_reset (buffer, reserve))
		return_val_if_reached (NULL);

	return buffer;
}