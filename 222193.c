p11_rpc_message_parse (p11_rpc_message *msg,
                       p11_rpc_message_type type)
{
	const unsigned char *val;
	size_t len;
	uint32_t call_id;

	assert (msg != NULL);
	assert (msg->input != NULL);

	msg->parsed = 0;

	/* Pull out the call identifier */
	if (!p11_rpc_buffer_get_uint32 (msg->input, &msg->parsed, &call_id)) {
		p11_message ("invalid message: couldn't read call identifier");
		return false;
	}

	msg->signature = msg->sigverify = NULL;

	/* The call id and signature */
	if (call_id >= P11_RPC_CALL_MAX ||
	    (type == P11_RPC_REQUEST && call_id == P11_RPC_CALL_ERROR)) {
		p11_message ("invalid message: bad call id: %d", call_id);
		return false;
	}
	if (type == P11_RPC_REQUEST)
		msg->signature = p11_rpc_calls[call_id].request;
	else if (type == P11_RPC_RESPONSE)
		msg->signature = p11_rpc_calls[call_id].response;
	else
		assert_not_reached ();
	assert (msg->signature != NULL);
	msg->call_id = call_id;
	msg->call_type = type;
	msg->sigverify = msg->signature;

	/* Verify the incoming signature */
	if (!p11_rpc_buffer_get_byte_array (msg->input, &msg->parsed, &val, &len) ||
	    /* This can happen if the length header == 0xffffffff */
	    val == NULL) {
		p11_message ("invalid message: couldn't read signature");
		return false;
	}

	if ((strlen (msg->signature) != len) || (memcmp (val, msg->signature, len) != 0)) {
		p11_message ("invalid message: signature doesn't match");
		return false;
	}

	return true;
}