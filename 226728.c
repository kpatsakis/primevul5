ipmi_session_main(struct ipmi_intf * intf, int argc, char ** argv)
{
	int retval = 0;

	if (argc == 0 || strncmp(argv[0], "help", 4) == 0)
	{
		printf_session_usage();
	}
	else if (strncmp(argv[0], "info", 4) == 0)
	{

		if ((argc < 2) || strncmp(argv[1], "help", 4) == 0)
		{
				printf_session_usage();
		}
		else
		{
			Ipmi_Session_Request_Type session_request_type = 0;
			uint32_t                  id_or_handle = 0;

			if (strncmp(argv[1], "active", 6) == 0)
				session_request_type = IPMI_SESSION_REQUEST_CURRENT;
			else if (strncmp(argv[1], "all", 3) == 0)
				session_request_type = IPMI_SESSION_REQUEST_ALL;
			else if (strncmp(argv[1], "id", 2) == 0)
			{
				if (argc >= 3)
				{
					session_request_type = IPMI_SESSION_REQUEST_BY_ID;
					if (str2uint(argv[2], &id_or_handle) != 0) {
						lprintf(LOG_ERR, "HEX number expected, but '%s' given.",
								argv[2]);
						printf_session_usage();
						retval = -1;
					}
				}
				else
				{
					lprintf(LOG_ERR, "Missing id argument");
					printf_session_usage();
					retval = -1;
				}
			}
			else if (strncmp(argv[1], "handle", 6) == 0)
			{
				if (argc >= 3)
				{
					session_request_type = IPMI_SESSION_REQUEST_BY_HANDLE;
					if (str2uint(argv[2], &id_or_handle) != 0) {
						lprintf(LOG_ERR, "HEX number expected, but '%s' given.",
								argv[2]);
						printf_session_usage();
						retval = -1;
					}
				}
				else
				{
					lprintf(LOG_ERR, "Missing handle argument");
					printf_session_usage();
					retval = -1;
				}
			}
			else
			{
				lprintf(LOG_ERR, "Invalid SESSION info parameter: %s", argv[1]);
				printf_session_usage();
				retval = -1;
			}
			

			if (retval == 0)
				retval = ipmi_get_session_info(intf,
											   session_request_type,
											   id_or_handle);
		}
	}
	else
	{
		lprintf(LOG_ERR, "Invalid SESSION command: %s", argv[0]);
		printf_session_usage();
		retval = -1;
	}

	return retval;
}