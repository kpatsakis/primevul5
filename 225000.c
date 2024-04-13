	NULL,
};

static umode_t iscsi_host_attr_is_visible(struct kobject *kobj,
					 struct attribute *attr, int i)
{
	struct device *cdev = container_of(kobj, struct device, kobj);
	struct Scsi_Host *shost = transport_class_to_shost(cdev);
	struct iscsi_internal *priv = to_iscsi_internal(shost->transportt);
	int param;

	if (attr == &dev_attr_host_netdev.attr)
		param = ISCSI_HOST_PARAM_NETDEV_NAME;
	else if (attr == &dev_attr_host_hwaddress.attr)
		param = ISCSI_HOST_PARAM_HWADDRESS;
	else if (attr == &dev_attr_host_ipaddress.attr)
		param = ISCSI_HOST_PARAM_IPADDRESS;
	else if (attr == &dev_attr_host_initiatorname.attr)
		param = ISCSI_HOST_PARAM_INITIATOR_NAME;
	else if (attr == &dev_attr_host_port_state.attr)
		param = ISCSI_HOST_PARAM_PORT_STATE;
	else if (attr == &dev_attr_host_port_speed.attr)
		param = ISCSI_HOST_PARAM_PORT_SPEED;
	else {
		WARN_ONCE(1, "Invalid host attr");
		return 0;
	}