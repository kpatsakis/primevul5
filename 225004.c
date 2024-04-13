}

static int
iscsi_set_path(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct iscsi_path *params;
	int err;

	if (!transport->set_path)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.set_path.host_no);
	if (!shost) {
		printk(KERN_ERR "set path could not find host no %u\n",
		       ev->u.set_path.host_no);
		return -ENODEV;
	}

	params = (struct iscsi_path *)((char *)ev + sizeof(*ev));
	err = transport->set_path(shost, params);
