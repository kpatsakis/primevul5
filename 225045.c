}

static int
iscsi_get_host_stats(struct iscsi_transport *transport, struct nlmsghdr *nlh)
{
	struct iscsi_uevent *ev = nlmsg_data(nlh);
	struct Scsi_Host *shost = NULL;
	struct iscsi_internal *priv;
	struct sk_buff *skbhost_stats;
	struct nlmsghdr *nlhhost_stats;
	struct iscsi_uevent *evhost_stats;
	int host_stats_size = 0;
	int len, err = 0;
	char *buf;

	if (!transport->get_host_stats)
		return -ENOSYS;

	priv = iscsi_if_transport_lookup(transport);
	if (!priv)
		return -EINVAL;

	host_stats_size = sizeof(struct iscsi_offload_host_stats);
	len = nlmsg_total_size(sizeof(*ev) + host_stats_size);

	shost = scsi_host_lookup(ev->u.get_host_stats.host_no);
	if (!shost) {
		pr_err("%s: failed. Could not find host no %u\n",
		       __func__, ev->u.get_host_stats.host_no);
		return -ENODEV;
	}

	do {
		int actual_size;

		skbhost_stats = alloc_skb(len, GFP_KERNEL);
		if (!skbhost_stats) {
			pr_err("cannot deliver host stats: OOM\n");
			err = -ENOMEM;
			goto exit_host_stats;
		}

		nlhhost_stats = __nlmsg_put(skbhost_stats, 0, 0, 0,
				      (len - sizeof(*nlhhost_stats)), 0);
		evhost_stats = nlmsg_data(nlhhost_stats);
		memset(evhost_stats, 0, sizeof(*evhost_stats));
		evhost_stats->transport_handle = iscsi_handle(transport);
		evhost_stats->type = nlh->nlmsg_type;
		evhost_stats->u.get_host_stats.host_no =
					ev->u.get_host_stats.host_no;
		buf = (char *)evhost_stats + sizeof(*evhost_stats);
		memset(buf, 0, host_stats_size);

		err = transport->get_host_stats(shost, buf, host_stats_size);
		if (err) {
			kfree_skb(skbhost_stats);
			goto exit_host_stats;
		}

		actual_size = nlmsg_total_size(sizeof(*ev) + host_stats_size);
		skb_trim(skbhost_stats, NLMSG_ALIGN(actual_size));
		nlhhost_stats->nlmsg_len = actual_size;

		err = iscsi_multicast_skb(skbhost_stats, ISCSI_NL_GRP_ISCSID,
					  GFP_KERNEL);
	} while (err < 0 && err != -ECONNREFUSED);

exit_host_stats: