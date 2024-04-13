	return err;
}

static int iscsi_set_chap(struct iscsi_transport *transport,
			  struct iscsi_uevent *ev, uint32_t len)
{
	char *data = (char *)ev + sizeof(*ev);
	struct Scsi_Host *shost;
	int err = 0;

	if (!transport->set_chap)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.set_path.host_no);
	if (!shost) {
		pr_err("%s could not find host no %u\n",
		       __func__, ev->u.set_path.host_no);
		return -ENODEV;
	}

	err = transport->set_chap(shost, data, len);