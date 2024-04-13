static int sctp_getsockopt_local_addrs(struct sock *sk, int len,
				       char __user *optval, int __user *optlen)
{
	struct sctp_bind_addr *bp;
	struct sctp_association *asoc;
	int cnt = 0;
	struct sctp_getaddrs getaddrs;
	struct sctp_sockaddr_entry *addr;
	void __user *to;
	union sctp_addr temp;
	struct sctp_sock *sp = sctp_sk(sk);
	int addrlen;
	int err = 0;
	size_t space_left;
	int bytes_copied = 0;
	void *addrs;
	void *buf;

	if (len < sizeof(struct sctp_getaddrs))
		return -EINVAL;

	if (copy_from_user(&getaddrs, optval, sizeof(struct sctp_getaddrs)))
		return -EFAULT;

	/*
	 *  For UDP-style sockets, id specifies the association to query.
	 *  If the id field is set to the value '0' then the locally bound
	 *  addresses are returned without regard to any particular
	 *  association.
	 */
	if (0 == getaddrs.assoc_id) {
		bp = &sctp_sk(sk)->ep->base.bind_addr;
	} else {
		asoc = sctp_id2assoc(sk, getaddrs.assoc_id);
		if (!asoc)
			return -EINVAL;
		bp = &asoc->base.bind_addr;
	}

	to = optval + offsetof(struct sctp_getaddrs, addrs);
	space_left = len - offsetof(struct sctp_getaddrs, addrs);

	addrs = kmalloc(space_left, GFP_USER | __GFP_NOWARN);
	if (!addrs)
		return -ENOMEM;

	/* If the endpoint is bound to 0.0.0.0 or ::0, get the valid
	 * addresses from the global local address list.
	 */
	if (sctp_list_single_entry(&bp->address_list)) {
		addr = list_entry(bp->address_list.next,
				  struct sctp_sockaddr_entry, list);
		if (sctp_is_any(sk, &addr->a)) {
			cnt = sctp_copy_laddrs(sk, bp->port, addrs,
						space_left, &bytes_copied);
			if (cnt < 0) {
				err = cnt;
				goto out;
			}
			goto copy_getaddrs;
		}
	}

	buf = addrs;
	/* Protection on the bound address list is not needed since
	 * in the socket option context we hold a socket lock and
	 * thus the bound address list can't change.
	 */
	list_for_each_entry(addr, &bp->address_list, list) {
		memcpy(&temp, &addr->a, sizeof(temp));
		addrlen = sctp_get_pf_specific(sk->sk_family)
			      ->addr_to_user(sp, &temp);
		if (space_left < addrlen) {
			err =  -ENOMEM; /*fixme: right error?*/
			goto out;
		}
		memcpy(buf, &temp, addrlen);
		buf += addrlen;
		bytes_copied += addrlen;
		cnt++;
		space_left -= addrlen;
	}

copy_getaddrs:
	if (copy_to_user(to, addrs, bytes_copied)) {
		err = -EFAULT;
		goto out;
	}
	if (put_user(cnt, &((struct sctp_getaddrs __user *)optval)->addr_num)) {
		err = -EFAULT;
		goto out;
	}
	/* XXX: We should have accounted for sizeof(struct sctp_getaddrs) too,
	 * but we can't change it anymore.
	 */
	if (put_user(bytes_copied, optlen))
		err = -EFAULT;
out:
	kfree(addrs);
	return err;
}