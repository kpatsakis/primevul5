}

static int
iscsi_if_transport_ep(struct iscsi_transport *transport,
		      struct iscsi_uevent *ev, int msg_type)
{
	struct iscsi_endpoint *ep;
	int rc = 0;

	switch (msg_type) {
	case ISCSI_UEVENT_TRANSPORT_EP_CONNECT_THROUGH_HOST:
	case ISCSI_UEVENT_TRANSPORT_EP_CONNECT:
		rc = iscsi_if_ep_connect(transport, ev, msg_type);
		break;
	case ISCSI_UEVENT_TRANSPORT_EP_POLL:
		if (!transport->ep_poll)
			return -EINVAL;

		ep = iscsi_lookup_endpoint(ev->u.ep_poll.ep_handle);
		if (!ep)
			return -EINVAL;

		ev->r.retcode = transport->ep_poll(ep,
						   ev->u.ep_poll.timeout_ms);
		break;
	case ISCSI_UEVENT_TRANSPORT_EP_DISCONNECT:
		rc = iscsi_if_ep_disconnect(transport,
					    ev->u.ep_disconnect.ep_handle);
		break;