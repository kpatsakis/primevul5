p11_rpc_message_verify_part (p11_rpc_message *msg,
                             const char* part)
{
	int len;
	bool ok;

	if (!msg->sigverify)
		return true;

	len = strlen (part);
	ok = (strncmp (msg->sigverify, part, len) == 0);
	if (ok)
		msg->sigverify += len;
	return ok;
}