print_session_info_verbose(const struct  get_session_info_rsp * session_info,
						   int data_len)
{
	char     buffer[18];
	uint16_t console_port_tmp;
	
	printf("session handle                : %d\n", session_info->session_handle);
	printf("slot count                    : %d\n", session_info->session_slot_count);
	printf("active sessions               : %d\n", session_info->active_session_count);

	if (data_len == 3)
	{
		/* There is no session data here */
		printf("\n");
		return;
	}

	printf("user id                       : %d\n", session_info->user_id);
	printf("privilege level               : %s\n",
		   val2str(session_info->privilege_level, ipmi_privlvl_vals));
	
	printf("session type                  : %s\n", session_info->auxiliary_data?
		   "IPMIv2/RMCP+" : "IPMIv1.5");

	printf("channel number                : 0x%02x\n", session_info->channel_number);

	
	if (data_len == 18)
	{
		/* We have 802.3 LAN data */
		printf("console ip                    : %s\n",
			   inet_ntop(AF_INET,
						 &(session_info->channel_data.lan_data.console_ip),
						 buffer,
						 16));

		printf("console mac                   : %s\n", mac2str(
			session_info->channel_data.lan_data.console_mac));

		console_port_tmp = session_info->channel_data.lan_data.console_port;
		#if WORDS_BIGENDIAN
		console_port_tmp = BSWAP_16(console_port_tmp);
		#endif
		printf("console port                  : %d\n", console_port_tmp);
	}


	else if ((data_len == 12) || (data_len == 14))
	{
		/* Channel async serial/modem */
		printf("Session/Channel Activity Type : %s\n",
			   val2str(session_info->channel_data.modem_data.session_channel_activity_type,
					   ipmi_channel_activity_type_vals));

		printf("Destination selector          : %d\n",
			   session_info->channel_data.modem_data.destination_selector);

		printf("console ip                    : %s\n",
			   inet_ntop(AF_INET,
						 &(session_info->channel_data.modem_data.console_ip),
						 buffer,
						 16));

		if (data_len == 14)
		{
			/* Connection is PPP */
			console_port_tmp = session_info->channel_data.lan_data.console_port;
			#if WORDS_BIGENDIAN
			console_port_tmp = BSWAP_16(console_port_tmp);
			#endif
			printf("console port                  : %d\n", console_port_tmp);
		}
	}

	printf("\n");
}