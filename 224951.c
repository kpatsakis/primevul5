
char *iscsi_get_router_state_name(enum iscsi_router_state router_state)
{
	int i;
	char *state = NULL;

	for (i = 0; i < ARRAY_SIZE(iscsi_router_state_names); i++) {
		if (iscsi_router_state_names[i].value == router_state) {
			state = iscsi_router_state_names[i].name;
			break;
		}
	}
	return state;