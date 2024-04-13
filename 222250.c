p11_rpc_message_read_version (p11_rpc_message *msg,
                              CK_VERSION *version)
{
	assert (msg != NULL);
	assert (msg->input != NULL);
	assert (version != NULL);

	/* Check that we're supposed to have this at this point */
	assert (!msg->signature || p11_rpc_message_verify_part (msg, "v"));

	return p11_rpc_buffer_get_byte (msg->input, &msg->parsed, &version->major) &&
	       p11_rpc_buffer_get_byte (msg->input, &msg->parsed, &version->minor);
}