}

static int
iscsi_send_ping(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct sockaddr *dst_addr;
	int err;

	if (!transport->send_ping)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.iscsi_ping.host_no);
	if (!shost) {
		printk(KERN_ERR "iscsi_ping could not find host no %u\n",
		       ev->u.iscsi_ping.host_no);
		return -ENODEV;
	}

	dst_addr = (struct sockaddr *)((char *)ev + sizeof(*ev));
	err = transport->send_ping(shost, ev->u.iscsi_ping.iface_num,
				   ev->u.iscsi_ping.iface_type,
				   ev->u.iscsi_ping.payload_size,
				   ev->u.iscsi_ping.pid,
				   dst_addr);