print_session_info_csv(const struct  get_session_info_rsp * session_info,
					   int data_len)
{
	char     buffer[18];
	uint16_t console_port_tmp;
	
	printf("%d", session_info->session_handle);
	printf(",%d", session_info->session_slot_count);
	printf(",%d", session_info->active_session_count);

	if (data_len == 3)
	{
		/* There is no session data here*/
		printf("\n");
		return;
	}

	printf(",%d", session_info->user_id);
	printf(",%s", val2str(session_info->privilege_level, ipmi_privlvl_vals));

	printf(",%s", session_info->auxiliary_data?
		   "IPMIv2/RMCP+" : "IPMIv1.5");

	printf(",0x%02x", session_info->channel_number);

	if (data_len == 18)
	{
		/* We have 802.3 LAN data */
		printf(",%s",
			   inet_ntop(AF_INET,
						 &(session_info->channel_data.lan_data.console_ip),
						 buffer,
						 16));

		printf(",%s", mac2str(
			session_info->channel_data.lan_data.console_mac));

		console_port_tmp = session_info->channel_data.lan_data.console_port;
		#if WORDS_BIGENDIAN
		console_port_tmp = BSWAP_16(console_port_tmp);
		#endif
		printf(",%d", console_port_tmp);
	}


	else if ((data_len == 12) || (data_len == 14))
	{
		/* Channel async serial/modem */
		printf(",%s",
			   val2str(session_info->channel_data.modem_data.session_channel_activity_type,
					   ipmi_channel_activity_type_vals));

		printf(",%d",
			   session_info->channel_data.modem_data.destination_selector);

		printf(",%s",
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
			printf(",%d", console_port_tmp);
		}
	}

	printf("\n");
}