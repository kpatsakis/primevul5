ipmi_lan_set_password(struct ipmi_intf *intf,
		uint8_t user_id, const char *password)
{
	int ccode = 0;
	ccode = _ipmi_set_user_password(intf, user_id,
			IPMI_PASSWORD_SET_PASSWORD, password, 0);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR, "Unable to Set LAN Password for user %d",
				user_id);
		return (-1);
	}
	/* adjust our session password
	 * or we will no longer be able to communicate with BMC
	 */
	ipmi_intf_session_set_password(intf, (char *)password);
	printf("Password %s for user %d\n",
	       password ? "set" : "cleared", user_id);

	return 0;
}