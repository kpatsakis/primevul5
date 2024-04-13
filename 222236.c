p11_rpc_message_write_byte (p11_rpc_message *msg,
                            CK_BYTE val)
{
	assert (msg != NULL);
	assert (msg->output != NULL);

	/* Make sure this is in the right order */
	assert (!msg->signature || p11_rpc_message_verify_part (msg, "y"));
	p11_rpc_buffer_add_byte (msg->output, val);
	return !p11_buffer_failed (msg->output);
}