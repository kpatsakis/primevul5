	{ISCSI_PORT_SPEED_40GBPS,       "40 Gbps" },
};

char *iscsi_get_port_speed_name(struct Scsi_Host *shost)
{
	int i;
	char *speed = "Unknown!";
	struct iscsi_cls_host *ihost = shost->shost_data;
	uint32_t port_speed = ihost->port_speed;

	for (i = 0; i < ARRAY_SIZE(iscsi_port_speed_names); i++) {
		if (iscsi_port_speed_names[i].value & port_speed) {
			speed = iscsi_port_speed_names[i].name;
			break;
		}