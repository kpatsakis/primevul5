p11_rpc_message_read_ulong (p11_rpc_message *msg,
                            CK_ULONG *val)
{
	uint64_t v;

	assert (msg != NULL);
	assert (msg->input != NULL);

	/* Make sure this is in the right order */
	assert (!msg->signature || p11_rpc_message_verify_part (msg, "u"));

	if (!p11_rpc_buffer_get_uint64 (msg->input, &msg->parsed, &v))
		return false;
	if (val)
		*val = (CK_ULONG)v;
	return true;
}