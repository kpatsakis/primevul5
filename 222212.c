p11_rpc_message_init (p11_rpc_message *msg,
                      p11_buffer *input,
                      p11_buffer *output)
{
	assert (input != NULL);
	assert (output != NULL);
	assert (output->ffree != NULL);
	assert (output->frealloc != NULL);

	memset (msg, 0, sizeof (*msg));

	msg->output = output;
	msg->input = input;
}