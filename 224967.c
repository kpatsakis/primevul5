static ISCSI_CLASS_ATTR(priv_sess, creator, S_IRUGO, show_priv_session_creator,
			NULL);
static ssize_t
show_priv_session_target_id(struct device *dev, struct device_attribute *attr,
			    char *buf)
{