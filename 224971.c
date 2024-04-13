	return err;
}

static int iscsi_delete_chap(struct iscsi_transport *transport,
			     struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	int err = 0;

	if (!transport->delete_chap)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.delete_chap.host_no);
	if (!shost) {
		printk(KERN_ERR "%s could not find host no %u\n",
		       __func__, ev->u.delete_chap.host_no);
		return -ENODEV;
	}

	err = transport->delete_chap(shost, ev->u.delete_chap.chap_tbl_idx);