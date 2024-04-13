p11_rpc_message_write_version (p11_rpc_message *msg,
                               CK_VERSION *version)
{
	assert (msg != NULL);
	assert (msg->output != NULL);
	assert (version != NULL);

	/* Check that we're supposed to have this at this point */
	assert (!msg->signature || p11_rpc_message_verify_part (msg, "v"));

	p11_rpc_buffer_add_byte (msg->output, version->major);
	p11_rpc_buffer_add_byte (msg->output, version->minor);

	return !p11_buffer_failed (msg->output);
}