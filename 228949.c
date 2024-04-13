ipmi_set_user_access(struct ipmi_intf *intf, uint8_t channel, uint8_t user_id)
{
	struct user_access_t user_access;
	int ccode = 0;
	memset(&user_access, 0, sizeof(user_access));
	user_access.channel = channel;
	user_access.user_id = user_id;
	user_access.privilege_limit = 0x04;

	ccode = _ipmi_set_user_access(intf, &user_access, 1);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR, "Set User Access for channel %d failed",
				channel);
		return (-1);
	} else {
		printf("Set User Access for channel %d was successful.",
				channel);
		return 0;
	}
}