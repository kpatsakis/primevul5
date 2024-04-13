static int sctp_getsockopt_peer_addrs(struct sock *sk, int len,
				      char __user *optval, int __user *optlen)
{
	struct sctp_association *asoc;
	int cnt = 0;
	struct sctp_getaddrs getaddrs;
	struct sctp_transport *from;
	void __user *to;
	union sctp_addr temp;
	struct sctp_sock *sp = sctp_sk(sk);
	int addrlen;
	size_t space_left;
	int bytes_copied;

	if (len < sizeof(struct sctp_getaddrs))
		return -EINVAL;

	if (copy_from_user(&getaddrs, optval, sizeof(struct sctp_getaddrs)))
		return -EFAULT;

	/* For UDP-style sockets, id specifies the association to query.  */
	asoc = sctp_id2assoc(sk, getaddrs.assoc_id);
	if (!asoc)
		return -EINVAL;

	to = optval + offsetof(struct sctp_getaddrs, addrs);
	space_left = len - offsetof(struct sctp_getaddrs, addrs);

	list_for_each_entry(from, &asoc->peer.transport_addr_list,
				transports) {
		memcpy(&temp, &from->ipaddr, sizeof(temp));
		addrlen = sctp_get_pf_specific(sk->sk_family)
			      ->addr_to_user(sp, &temp);
		if (space_left < addrlen)
			return -ENOMEM;
		if (copy_to_user(to, &temp, addrlen))
			return -EFAULT;
		to += addrlen;
		cnt++;
		space_left -= addrlen;
	}

	if (put_user(cnt, &((struct sctp_getaddrs __user *)optval)->addr_num))
		return -EFAULT;
	bytes_copied = ((char __user *)to) - optval;
	if (put_user(bytes_copied, optlen))
		return -EFAULT;

	return 0;
}