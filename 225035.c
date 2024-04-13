}

static int
iscsi_get_chap(struct iscsi_transport *transport, struct nlmsghdr *nlh)
{
	struct iscsi_uevent *ev = nlmsg_data(nlh);
	struct Scsi_Host *shost = NULL;
	struct iscsi_chap_rec *chap_rec;
	struct iscsi_internal *priv;
	struct sk_buff *skbchap;
	struct nlmsghdr *nlhchap;
	struct iscsi_uevent *evchap;
	uint32_t chap_buf_size;
	int len, err = 0;
	char *buf;

	if (!transport->get_chap)
		return -EINVAL;

	priv = iscsi_if_transport_lookup(transport);
	if (!priv)
		return -EINVAL;

	chap_buf_size = (ev->u.get_chap.num_entries * sizeof(*chap_rec));
	len = nlmsg_total_size(sizeof(*ev) + chap_buf_size);

	shost = scsi_host_lookup(ev->u.get_chap.host_no);
	if (!shost) {
		printk(KERN_ERR "%s: failed. Could not find host no %u\n",
		       __func__, ev->u.get_chap.host_no);
		return -ENODEV;
	}

	do {
		int actual_size;

		skbchap = alloc_skb(len, GFP_KERNEL);
		if (!skbchap) {
			printk(KERN_ERR "can not deliver chap: OOM\n");
			err = -ENOMEM;
			goto exit_get_chap;
		}

		nlhchap = __nlmsg_put(skbchap, 0, 0, 0,
				      (len - sizeof(*nlhchap)), 0);
		evchap = nlmsg_data(nlhchap);
		memset(evchap, 0, sizeof(*evchap));
		evchap->transport_handle = iscsi_handle(transport);
		evchap->type = nlh->nlmsg_type;
		evchap->u.get_chap.host_no = ev->u.get_chap.host_no;
		evchap->u.get_chap.chap_tbl_idx = ev->u.get_chap.chap_tbl_idx;
		evchap->u.get_chap.num_entries = ev->u.get_chap.num_entries;
		buf = (char *)evchap + sizeof(*evchap);
		memset(buf, 0, chap_buf_size);

		err = transport->get_chap(shost, ev->u.get_chap.chap_tbl_idx,
				    &evchap->u.get_chap.num_entries, buf);

		actual_size = nlmsg_total_size(sizeof(*ev) + chap_buf_size);
		skb_trim(skbchap, NLMSG_ALIGN(actual_size));
		nlhchap->nlmsg_len = actual_size;

		err = iscsi_multicast_skb(skbchap, ISCSI_NL_GRP_ISCSID,
					  GFP_KERNEL);
	} while (err < 0 && err != -ECONNREFUSED);

exit_get_chap: