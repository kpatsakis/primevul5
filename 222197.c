p11_rpc_message_read_byte (p11_rpc_message *msg,
                           CK_BYTE *val)
{
	assert (msg != NULL);
	assert (msg->input != NULL);

	/* Make sure this is in the right order */
	assert (!msg->signature || p11_rpc_message_verify_part (msg, "y"));
	return p11_rpc_buffer_get_byte (msg->input, &msg->parsed, val);
}