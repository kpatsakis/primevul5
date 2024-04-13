ipmi_lanp_main(struct ipmi_intf * intf, int argc, char ** argv)
{
	int rc = 0;
	uint8_t chan = 0;

	if (argc == 0) {
		print_lan_usage();
		return (-1);
	} else if (strncmp(argv[0], "help", 4) == 0) {
		print_lan_usage();
		return 0;
	}

	if (strncmp(argv[0], "printconf", 9) == 0 ||
			strncmp(argv[0], "print", 5) == 0) 
	{
		if (argc > 2) {
			print_lan_usage();
			return (-1);
		} else if (argc == 2) {
			if (str2uchar(argv[1], &chan) != 0) {
				lprintf(LOG_ERR, "Invalid channel: %s", argv[1]);
				return (-1);
			}
		} else {
			chan = find_lan_channel(intf, 1);
		}
		if (!is_lan_channel(intf, chan)) {
			lprintf(LOG_ERR, "Invalid channel: %d", chan);
			return (-1);
		}
		rc = ipmi_lan_print(intf, chan);
	} else if (strncmp(argv[0], "set", 3) == 0) {
		rc = ipmi_lan_set(intf, argc-1, &(argv[1]));
	} else if (strncmp(argv[0], "alert", 5) == 0) {
		rc = ipmi_lan_alert(intf, argc-1, &(argv[1]));
	} else if (strncmp(argv[0], "stats", 5) == 0) {
		if (argc < 2) {
			print_lan_usage();
			return (-1);
		} else if (argc == 3) {
			if (str2uchar(argv[2], &chan) != 0) {
				lprintf(LOG_ERR, "Invalid channel: %s", argv[2]);
				return (-1);
			}
		} else {
			chan = find_lan_channel(intf, 1);
		}
		if (!is_lan_channel(intf, chan)) {
			lprintf(LOG_ERR, "Invalid channel: %d", chan);
			return (-1);
		}
		if (strncmp(argv[1], "get", 3) == 0) {
			rc = ipmi_lan_stats_get(intf, chan);
		} else if (strncmp(argv[1], "clear", 5) == 0) {
			rc = ipmi_lan_stats_clear(intf, chan);
		} else {
			print_lan_usage();
			return (-1);
		}
	} else {
		lprintf(LOG_NOTICE, "Invalid LAN command: %s", argv[0]);
		return (-1);
	}
	return rc;
}