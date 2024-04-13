	return err;
}

static int iscsi_del_flashnode(struct iscsi_transport *transport,
			       struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct iscsi_bus_flash_session *fnode_sess;
	uint32_t idx;
	int err = 0;

	if (!transport->del_flashnode) {
		err = -ENOSYS;
		goto exit_del_fnode;
	}

	shost = scsi_host_lookup(ev->u.del_flashnode.host_no);
	if (!shost) {
		pr_err("%s could not find host no %u\n",
		       __func__, ev->u.del_flashnode.host_no);
		err = -ENODEV;
		goto put_host;
	}

	idx = ev->u.del_flashnode.flashnode_idx;
	fnode_sess = iscsi_get_flashnode_by_index(shost, idx);
	if (!fnode_sess) {
		pr_err("%s could not find flashnode %u for host no %u\n",
		       __func__, idx, ev->u.del_flashnode.host_no);
		err = -ENODEV;
		goto put_host;
	}

	err = transport->del_flashnode(fnode_sess);
	put_device(&fnode_sess->dev);

put_host:
	scsi_host_put(shost);
