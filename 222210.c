p11_rpc_message_alloc_extra (p11_rpc_message *msg,
                             size_t length)
{
	void **data;

	assert (msg != NULL);

	if (length > 0x7fffffff)
		return NULL;

	assert (msg->output->frealloc != NULL);
	data = (msg->output->frealloc) (NULL, sizeof (void *) + length);
	if (data == NULL)
		return NULL;

	/* Munch up the memory to help catch bugs */
	memset (data, 0xff, sizeof (void *) + length);

	/* Store pointer to next allocated block at beginning */
	*data = msg->extra;
	msg->extra = data;

	/* Data starts after first pointer */
	return (void *)(data + 1);
}