p11_rpc_message_read_space_string (p11_rpc_message *msg,
                                   CK_UTF8CHAR *buffer,
                                   CK_ULONG length)
{
	const unsigned char *data;
	size_t n_data;

	assert (msg != NULL);
	assert (msg->input != NULL);
	assert (buffer != NULL);
	assert (length != 0);

	assert (!msg->signature || p11_rpc_message_verify_part (msg, "s"));

	if (!p11_rpc_buffer_get_byte_array (msg->input, &msg->parsed, &data, &n_data))
		return false;

	if (n_data != length) {
		p11_message ("invalid length space padded string received: %d != %d",
		             (int)length, (int)n_data);
		return false;
	}

	memcpy (buffer, data, length);
	return true;
}