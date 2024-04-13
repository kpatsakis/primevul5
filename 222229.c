p11_rpc_message_write_space_string (p11_rpc_message *msg,
                                    CK_UTF8CHAR *data,
                                    CK_ULONG length)
{
	assert (msg != NULL);
	assert (msg->output != NULL);
	assert (data != NULL);
	assert (length != 0);

	assert (!msg->signature || p11_rpc_message_verify_part (msg, "s"));

	p11_rpc_buffer_add_byte_array (msg->output, data, length);
	return !p11_buffer_failed (msg->output);
}