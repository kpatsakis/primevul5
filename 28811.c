const char *who_am_i(void)
{
	if (am_starting_up)
		return am_server ? "server" : "client";
	return am_sender ? "sender"
	     : am_generator ? "generator"
	     : am_receiver ? "receiver"
	     : "Receiver"; /* pre-forked receiver */
}
