iscsi_session_attr(discovery_parent_type, ISCSI_PARAM_DISCOVERY_PARENT_TYPE, 0);

static ssize_t
show_priv_session_state(struct device *dev, struct device_attribute *attr,
			char *buf)
{