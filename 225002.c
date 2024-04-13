	return err;
}

static int iscsi_new_flashnode(struct iscsi_transport *transport,
			       struct iscsi_uevent *ev, uint32_t len)
{
	char *data = (char *)ev + sizeof(*ev);
	struct Scsi_Host *shost;
	int index;
	int err = 0;

	if (!transport->new_flashnode) {
		err = -ENOSYS;
		goto exit_new_fnode;
	}

	shost = scsi_host_lookup(ev->u.new_flashnode.host_no);
	if (!shost) {
		pr_err("%s could not find host no %u\n",
		       __func__, ev->u.new_flashnode.host_no);
		err = -ENODEV;
		goto put_host;
	}

	index = transport->new_flashnode(shost, data, len);

	if (index >= 0)
		ev->r.new_flashnode_ret.flashnode_idx = index;
	else
		err = -EIO;

put_host:
	scsi_host_put(shost);
