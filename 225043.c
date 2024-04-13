}

static int
iscsi_if_get_stats(struct iscsi_transport *transport, struct nlmsghdr *nlh)
{
	struct iscsi_uevent *ev = nlmsg_data(nlh);
	struct iscsi_stats *stats;
	struct sk_buff *skbstat;
	struct iscsi_cls_conn *conn;
	struct nlmsghdr	*nlhstat;
	struct iscsi_uevent *evstat;
	struct iscsi_internal *priv;
	int len = nlmsg_total_size(sizeof(*ev) +
				   sizeof(struct iscsi_stats) +
				   sizeof(struct iscsi_stats_custom) *
				   ISCSI_STATS_CUSTOM_MAX);
	int err = 0;

	priv = iscsi_if_transport_lookup(transport);
	if (!priv)
		return -EINVAL;

	conn = iscsi_conn_lookup(ev->u.get_stats.sid, ev->u.get_stats.cid);
	if (!conn)
		return -EEXIST;

	do {
		int actual_size;

		skbstat = alloc_skb(len, GFP_ATOMIC);
		if (!skbstat) {
			iscsi_cls_conn_printk(KERN_ERR, conn, "can not "
					      "deliver stats: OOM\n");
			return -ENOMEM;
		}

		nlhstat = __nlmsg_put(skbstat, 0, 0, 0,
				      (len - sizeof(*nlhstat)), 0);
		evstat = nlmsg_data(nlhstat);
		memset(evstat, 0, sizeof(*evstat));
		evstat->transport_handle = iscsi_handle(conn->transport);
		evstat->type = nlh->nlmsg_type;
		evstat->u.get_stats.cid =
			ev->u.get_stats.cid;
		evstat->u.get_stats.sid =
			ev->u.get_stats.sid;
		stats = (struct iscsi_stats *)
			((char*)evstat + sizeof(*evstat));
		memset(stats, 0, sizeof(*stats));

		transport->get_stats(conn, stats);
		actual_size = nlmsg_total_size(sizeof(struct iscsi_uevent) +
					       sizeof(struct iscsi_stats) +
					       sizeof(struct iscsi_stats_custom) *
					       stats->custom_length);
		actual_size -= sizeof(*nlhstat);
		actual_size = nlmsg_msg_size(actual_size);
		skb_trim(skbstat, NLMSG_ALIGN(actual_size));
		nlhstat->nlmsg_len = actual_size;

		err = iscsi_multicast_skb(skbstat, ISCSI_NL_GRP_ISCSID,
					  GFP_ATOMIC);
	} while (err < 0 && err != -ECONNREFUSED);