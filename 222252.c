p11_rpc_message_prep (p11_rpc_message *msg,
                      int call_id,
                      p11_rpc_message_type type)
{
	int len;

	assert (type != 0);
	assert (call_id >= P11_RPC_CALL_ERROR);
	assert (call_id < P11_RPC_CALL_MAX);

	p11_buffer_reset (msg->output, 0);
	msg->signature = NULL;

	/* The call id and signature */
	if (type == P11_RPC_REQUEST)
		msg->signature = p11_rpc_calls[call_id].request;
	else if (type == P11_RPC_RESPONSE)
		msg->signature = p11_rpc_calls[call_id].response;
	else
		assert_not_reached ();
	assert (msg->signature != NULL);
	msg->sigverify = msg->signature;

	msg->call_id = call_id;
	msg->call_type = type;

	/* Encode the two of them */
	p11_rpc_buffer_add_uint32 (msg->output, call_id);
	if (msg->signature) {
		len = strlen (msg->signature);
		p11_rpc_buffer_add_byte_array (msg->output, (unsigned char*)msg->signature, len);
	}

	msg->parsed = 0;
	return !p11_buffer_failed (msg->output);
}