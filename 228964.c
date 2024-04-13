ipmi_lan_alert_print(struct ipmi_intf * intf, uint8_t channel, uint8_t alert)
{
# define PTYPE_LEN	4
# define PADDR_LEN	13
	struct lan_param *lp_ptr = NULL;
	int isack = 0;
	uint8_t ptype[PTYPE_LEN];
	uint8_t paddr[PADDR_LEN];

	lp_ptr = get_lan_param_select(intf, channel, IPMI_LANP_DEST_TYPE, alert);
	if (!lp_ptr || !lp_ptr->data
			|| lp_ptr->data_len < PTYPE_LEN) {
		return (-1);
	}
	memcpy(ptype, lp_ptr->data, PTYPE_LEN);

	lp_ptr = get_lan_param_select(intf, channel, IPMI_LANP_DEST_ADDR, alert);
	if (!lp_ptr || !lp_ptr->data || lp_ptr->data_len < PADDR_LEN) {
		return (-1);
	}
	memcpy(paddr, lp_ptr->data, PADDR_LEN);

	printf("%-24s: %d\n", "Alert Destination",
			ptype[0]);

	if (ptype[1] & 0x80) {
		isack = 1;
	}
	printf("%-24s: %s\n", "Alert Acknowledge",
			isack ? "Acknowledged" : "Unacknowledged");

	printf("%-24s: ", "Destination Type");
	switch (ptype[1] & 0x7) {
	case 0:
		printf("PET Trap\n");
		break;
	case 6:
		printf("OEM 1\n");
		break;
	case 7:
		printf("OEM 2\n");
		break;
	default:
		printf("Unknown\n");
		break;
	}

	printf("%-24s: %d\n",
			isack ? "Acknowledge Timeout" : "Retry Interval",
			ptype[2]);

	printf("%-24s: %d\n", "Number of Retries",
			ptype[3] & 0x7);

	if ((paddr[1] & 0xf0) != 0) {
		/* unknown address format */
		printf("\n");
		return 0;
	}

	printf("%-24s: %s\n", "Alert Gateway",
			(paddr[2] & 1) ? "Backup" : "Default");

	printf("%-24s: %d.%d.%d.%d\n", "Alert IP Address",
			paddr[3], paddr[4], paddr[5], paddr[6]);

	printf("%-24s: %s\n", "Alert MAC Address",
			mac2str(&paddr[7]));

	printf("\n");
	return 0;
}