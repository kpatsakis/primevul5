	return err;
}

static int iscsi_if_ep_connect(struct iscsi_transport *transport,
			       struct iscsi_uevent *ev, int msg_type)
{
	struct iscsi_endpoint *ep;
	struct sockaddr *dst_addr;
	struct Scsi_Host *shost = NULL;
	int non_blocking, err = 0;

	if (!transport->ep_connect)
		return -EINVAL;

	if (msg_type == ISCSI_UEVENT_TRANSPORT_EP_CONNECT_THROUGH_HOST) {
		shost = scsi_host_lookup(ev->u.ep_connect_through_host.host_no);
		if (!shost) {
			printk(KERN_ERR "ep connect failed. Could not find "
			       "host no %u\n",
			       ev->u.ep_connect_through_host.host_no);
			return -ENODEV;
		}
		non_blocking = ev->u.ep_connect_through_host.non_blocking;
	} else
		non_blocking = ev->u.ep_connect.non_blocking;

	dst_addr = (struct sockaddr *)((char*)ev + sizeof(*ev));
	ep = transport->ep_connect(shost, dst_addr, non_blocking);
	if (IS_ERR(ep)) {
		err = PTR_ERR(ep);
		goto release_host;
	}

	ev->r.ep_connect_ret.handle = ep->id;
release_host:
	if (shost)