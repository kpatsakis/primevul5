ipmi_lan_alert(struct ipmi_intf * intf, int argc, char ** argv)
{
	uint8_t alert;
	uint8_t channel = 1;

	if (argc < 1) {
		print_lan_alert_print_usage();
		print_lan_alert_set_usage();
		return (-1);
	}
	else if (strncasecmp(argv[0], "help", 4) == 0) {
		print_lan_alert_print_usage();
		print_lan_alert_set_usage();
		return 0;
	}

	/* alert print [channel] [alert] */
	if (strncasecmp(argv[0], "print", 5) == 0) {
		if (argc < 2) {
			channel = find_lan_channel(intf, 1);
			if (!is_lan_channel(intf, channel)) {
				lprintf(LOG_ERR, "Channel %d is not a LAN channel", channel);
				return -1;
			}
			return ipmi_lan_alert_print_all(intf, channel);
		}

		if (strncasecmp(argv[1], "help", 4) == 0) {
			print_lan_alert_print_usage();
			return 0;
		}

		if (str2uchar(argv[1], &channel) != 0) {
			lprintf(LOG_ERR, "Invalid channel: %s", argv[1]);
			return (-1);
		}
		if (!is_lan_channel(intf, channel)) {
			lprintf(LOG_ERR, "Channel %d is not a LAN channel", channel);
			return -1;
		}

		if (argc < 3)
			return ipmi_lan_alert_print_all(intf, channel);

		if (str2uchar(argv[2], &alert) != 0) {
			lprintf(LOG_ERR, "Invalid alert: %s", argv[2]);
			return (-1);
		}
		if (is_alert_destination(intf, channel, alert) == 0) {
			lprintf(LOG_ERR, "Alert %d is not a valid destination", alert);
			return -1;
		}
		return ipmi_lan_alert_print(intf, channel, alert);
	}

	/* alert set <channel> <alert> [option] */
	if (strncasecmp(argv[0], "set", 3) == 0) {
		if (argc < 5) {
			print_lan_alert_set_usage();
			return (-1);
		}
		else if (strncasecmp(argv[1], "help", 4) == 0) {
			print_lan_alert_set_usage();
			return 0;
		}

		if (str2uchar(argv[1], &channel) != 0) {
			lprintf(LOG_ERR, "Invalid channel: %s", argv[1]);
			return (-1);
		}
		if (!is_lan_channel(intf, channel)) {
			lprintf(LOG_ERR, "Channel %d is not a LAN channel", channel);
			return -1;
		}

		if (str2uchar(argv[2], &alert) != 0) {
			lprintf(LOG_ERR, "Invalid alert: %s", argv[2]);
			return (-1);
		}
		if (is_alert_destination(intf, channel, alert) == 0) {
			lprintf(LOG_ERR, "Alert %d is not a valid destination", alert);
			return -1;
		}

		return ipmi_lan_alert_set(intf, channel, alert, argc-3, &(argv[3]));
	}

	return 0;
}