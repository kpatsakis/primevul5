static ISCSI_CLASS_ATTR(priv_sess, state, S_IRUGO, show_priv_session_state,
			NULL);
static ssize_t
show_priv_session_creator(struct device *dev, struct device_attribute *attr,
			char *buf)
{