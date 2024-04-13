 * @session: iscsi class session
 * @event: type of event
 */
int iscsi_session_event(struct iscsi_cls_session *session,
			enum iscsi_uevent_e event)
{
	struct iscsi_internal *priv;
	struct Scsi_Host *shost;
	struct iscsi_uevent *ev;
	struct sk_buff  *skb;
	struct nlmsghdr *nlh;
	int rc, len = nlmsg_total_size(sizeof(*ev));

	priv = iscsi_if_transport_lookup(session->transport);
	if (!priv)
		return -EINVAL;
	shost = iscsi_session_to_shost(session);

	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb) {
		iscsi_cls_session_printk(KERN_ERR, session,
					 "Cannot notify userspace of session "
					 "event %u\n", event);
		return -ENOMEM;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = nlmsg_data(nlh);
	ev->transport_handle = iscsi_handle(session->transport);

	ev->type = event;
	switch (event) {
	case ISCSI_KEVENT_DESTROY_SESSION:
		ev->r.d_session.host_no = shost->host_no;
		ev->r.d_session.sid = session->sid;
		break;
	case ISCSI_KEVENT_CREATE_SESSION:
		ev->r.c_session_ret.host_no = shost->host_no;
		ev->r.c_session_ret.sid = session->sid;
		break;
	case ISCSI_KEVENT_UNBIND_SESSION:
		ev->r.unbind_session.host_no = shost->host_no;
		ev->r.unbind_session.sid = session->sid;
		break;
	default:
		iscsi_cls_session_printk(KERN_ERR, session, "Invalid event "
					 "%u.\n", event);
		kfree_skb(skb);
		return -EINVAL;
	}

	/*
	 * this will occur if the daemon is not up, so we just warn
	 * the user and when the daemon is restarted it will handle it
	 */
	rc = iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_KERNEL);
	if (rc == -ESRCH)
		iscsi_cls_session_printk(KERN_ERR, session,
					 "Cannot notify userspace of session "
					 "event %u. Check iscsi daemon\n",
					 event);

	ISCSI_DBG_TRANS_SESSION(session, "Completed handling event %d rc %d\n",