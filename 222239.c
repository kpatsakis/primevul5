p11_rpc_message_clear (p11_rpc_message *msg)
{
	void *allocated;
	void **data;

	assert (msg != NULL);

	/* Free up the extra allocated memory */
	allocated = msg->extra;
	while (allocated != NULL) {
		data = (void **)allocated;

		/* Pointer to the next allocation */
		allocated = *data;
		assert (msg->output->ffree);
		(msg->output->ffree) (data);
	}

	msg->output = NULL;
	msg->input = NULL;
	msg->extra = NULL;
}