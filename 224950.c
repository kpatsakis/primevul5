}

static int
iscsi_tgt_dscvr(struct iscsi_transport *transport,
		struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct sockaddr *dst_addr;
	int err;

	if (!transport->tgt_dscvr)
		return -EINVAL;

	shost = scsi_host_lookup(ev->u.tgt_dscvr.host_no);
	if (!shost) {
		printk(KERN_ERR "target discovery could not find host no %u\n",
		       ev->u.tgt_dscvr.host_no);
		return -ENODEV;
	}


	dst_addr = (struct sockaddr *)((char*)ev + sizeof(*ev));
	err = transport->tgt_dscvr(shost, ev->u.tgt_dscvr.type,
				   ev->u.tgt_dscvr.enable, dst_addr);