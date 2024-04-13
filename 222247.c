p11_rpc_message_write_zero_string (p11_rpc_message *msg,
                                   CK_UTF8CHAR *string)
{
	assert (msg != NULL);
	assert (msg->output != NULL);
	assert (string != NULL);

	assert (!msg->signature || p11_rpc_message_verify_part (msg, "z"));

	p11_rpc_buffer_add_byte_array (msg->output, string,
	                               string ? strlen ((char *)string) : 0);
	return !p11_buffer_failed (msg->output);
}