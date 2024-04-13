	{ISCSI_PORT_STATE_UP,		"LINK UP" },
};

char *iscsi_get_port_state_name(struct Scsi_Host *shost)
{
	int i;
	char *state = "Unknown!";
	struct iscsi_cls_host *ihost = shost->shost_data;
	uint32_t port_state = ihost->port_state;

	for (i = 0; i < ARRAY_SIZE(iscsi_port_state_names); i++) {
		if (iscsi_port_state_names[i].value & port_state) {
			state = iscsi_port_state_names[i].name;
			break;
		}