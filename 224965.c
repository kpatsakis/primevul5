}

static int
iscsi_set_iface_params(struct iscsi_transport *transport,
		       struct iscsi_uevent *ev, uint32_t len)
{
	char *data = (char *)ev + sizeof(*ev);
	struct Scsi_Host *shost;
	int err;

	if (!transport->set_iface_param)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.set_iface_params.host_no);
	if (!shost) {
		printk(KERN_ERR "set_iface_params could not find host no %u\n",
		       ev->u.set_iface_params.host_no);
		return -ENODEV;
	}

	err = transport->set_iface_param(shost, data, len);