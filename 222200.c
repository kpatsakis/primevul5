p11_rpc_message_write_byte_buffer (p11_rpc_message *msg,
                                   CK_ULONG count)
{
	assert (msg != NULL);
	assert (msg->output != NULL);

	/* Make sure this is in the right order */
	assert (!msg->signature || p11_rpc_message_verify_part (msg, "fy"));
	p11_rpc_buffer_add_uint32 (msg->output, count);
	return !p11_buffer_failed (msg->output);
}