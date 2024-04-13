
char *iscsi_get_ipaddress_state_name(enum iscsi_ipaddress_state port_state)
{
	int i;
	char *state = NULL;

	for (i = 0; i < ARRAY_SIZE(iscsi_ipaddress_state_names); i++) {
		if (iscsi_ipaddress_state_names[i].value == port_state) {
			state = iscsi_ipaddress_state_names[i].name;
			break;
		}
	}
	return state;