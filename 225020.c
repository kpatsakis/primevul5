	[ISCSI_CONN_FAILED] = "failed"
};

static ssize_t show_conn_state(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	struct iscsi_cls_conn *conn = iscsi_dev_to_conn(dev->parent);
	const char *state = "unknown";

	if (conn->state >= 0 &&
	    conn->state < ARRAY_SIZE(connection_state_names))
		state = connection_state_names[conn->state];