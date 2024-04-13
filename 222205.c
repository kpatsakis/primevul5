p11_rpc_message_write_byte_array (p11_rpc_message *msg,
                                  CK_BYTE_PTR arr,
                                  CK_ULONG num)
{
	assert (msg != NULL);
	assert (msg->output != NULL);

	/* Make sure this is in the right order */
	assert (!msg->signature || p11_rpc_message_verify_part (msg, "ay"));

	/* No array, no data, just length */
	if (!arr) {
		p11_rpc_buffer_add_byte (msg->output, 0);
		p11_rpc_buffer_add_uint32 (msg->output, num);
	} else {
		p11_rpc_buffer_add_byte (msg->output, 1);
		p11_rpc_buffer_add_byte_array (msg->output, arr, num);
	}

	return !p11_buffer_failed (msg->output);
}