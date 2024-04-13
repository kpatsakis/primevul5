	return err;
}

static int iscsi_login_flashnode(struct iscsi_transport *transport,
				 struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct iscsi_bus_flash_session *fnode_sess;
	struct iscsi_bus_flash_conn *fnode_conn;
	struct device *dev;
	uint32_t idx;
	int err = 0;

	if (!transport->login_flashnode) {
		err = -ENOSYS;
		goto exit_login_fnode;
	}

	shost = scsi_host_lookup(ev->u.login_flashnode.host_no);
	if (!shost) {
		pr_err("%s could not find host no %u\n",
		       __func__, ev->u.login_flashnode.host_no);
		err = -ENODEV;
		goto put_host;
	}

	idx = ev->u.login_flashnode.flashnode_idx;
	fnode_sess = iscsi_get_flashnode_by_index(shost, idx);
	if (!fnode_sess) {
		pr_err("%s could not find flashnode %u for host no %u\n",
		       __func__, idx, ev->u.login_flashnode.host_no);
		err = -ENODEV;
		goto put_host;
	}

	dev = iscsi_find_flashnode_conn(fnode_sess);
	if (!dev) {
		err = -ENODEV;
		goto put_sess;
	}

	fnode_conn = iscsi_dev_to_flash_conn(dev);
	err = transport->login_flashnode(fnode_sess, fnode_conn);
	put_device(dev);

put_sess:
	put_device(&fnode_sess->dev);

put_host:
	scsi_host_put(shost);
