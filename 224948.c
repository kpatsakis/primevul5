}
EXPORT_SYMBOL_GPL(iscsi_get_discovery_parent_name);

static int iscsi_set_flashnode_param(struct iscsi_transport *transport,
				     struct iscsi_uevent *ev, uint32_t len)
{
	char *data = (char *)ev + sizeof(*ev);
	struct Scsi_Host *shost;
	struct iscsi_bus_flash_session *fnode_sess;
	struct iscsi_bus_flash_conn *fnode_conn;
	struct device *dev;
	uint32_t idx;
	int err = 0;

	if (!transport->set_flashnode_param) {
		err = -ENOSYS;
		goto exit_set_fnode;
	}

	shost = scsi_host_lookup(ev->u.set_flashnode.host_no);
	if (!shost) {
		pr_err("%s could not find host no %u\n",
		       __func__, ev->u.set_flashnode.host_no);
		err = -ENODEV;
		goto exit_set_fnode;
	}

	idx = ev->u.set_flashnode.flashnode_idx;
	fnode_sess = iscsi_get_flashnode_by_index(shost, idx);
	if (!fnode_sess) {
		pr_err("%s could not find flashnode %u for host no %u\n",
		       __func__, idx, ev->u.set_flashnode.host_no);
		err = -ENODEV;
		goto put_host;
	}

	dev = iscsi_find_flashnode_conn(fnode_sess);
	if (!dev) {
		err = -ENODEV;
		goto put_sess;
	}

	fnode_conn = iscsi_dev_to_flash_conn(dev);
	err = transport->set_flashnode_param(fnode_sess, fnode_conn, data, len);
	put_device(dev);

put_sess:
	put_device(&fnode_sess->dev);

put_host:
	scsi_host_put(shost);
