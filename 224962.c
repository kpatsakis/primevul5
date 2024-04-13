	{ISCSI_DISC_PARENT_ISNS,	"isns" },
};

char *iscsi_get_discovery_parent_name(int parent_type)
{
	int i;
	char *state = "Unknown!";

	for (i = 0; i < ARRAY_SIZE(iscsi_discovery_parent_names); i++) {
		if (iscsi_discovery_parent_names[i].value & parent_type) {
			state = iscsi_discovery_parent_names[i].name;
			break;
		}