	{ ISCSI_SESSION_FREE,		"FREE" },
};

static const char *iscsi_session_state_name(int state)
{
	int i;
	char *name = NULL;

	for (i = 0; i < ARRAY_SIZE(iscsi_session_state_names); i++) {
		if (iscsi_session_state_names[i].value == state) {
			name = iscsi_session_state_names[i].name;
			break;
		}