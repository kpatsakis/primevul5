int udp_recvmsg(struct sock *sk, struct msghdr *msg, size_t len, int noblock,
		int flags, int *addr_len)
{
	struct inet_sock *inet = inet_sk(sk);
	DECLARE_SOCKADDR(struct sockaddr_in *, sin, msg->msg_name);
	struct sk_buff *skb;
	unsigned int ulen, copied;
	int peeked, peeking, off;
	int err;
	int is_udplite = IS_UDPLITE(sk);
	bool checksum_valid = false;

	if (flags & MSG_ERRQUEUE)
		return ip_recv_error(sk, msg, len, addr_len);

try_again:
	peeking = off = sk_peek_offset(sk, flags);
	skb = __skb_recv_udp(sk, flags, noblock, &peeked, &off, &err);
	if (!skb)
		return err;

	ulen = udp_skb_len(skb);
	copied = len;
	if (copied > ulen - off)
		copied = ulen - off;
	else if (copied < ulen)
		msg->msg_flags |= MSG_TRUNC;

	/*
	 * If checksum is needed at all, try to do it while copying the
	 * data.  If the data is truncated, or if we only want a partial
	 * coverage checksum (UDP-Lite), do it before the copy.
	 */

	if (copied < ulen || peeking ||
	    (is_udplite && UDP_SKB_CB(skb)->partial_cov)) {
		checksum_valid = udp_skb_csum_unnecessary(skb) ||
				!__udp_lib_checksum_complete(skb);
		if (!checksum_valid)
			goto csum_copy_err;
	}

	if (checksum_valid || udp_skb_csum_unnecessary(skb)) {
		if (udp_skb_is_linear(skb))
			err = copy_linear_skb(skb, copied, off, &msg->msg_iter);
		else
			err = skb_copy_datagram_msg(skb, off, msg, copied);
	} else {
		err = skb_copy_and_csum_datagram_msg(skb, off, msg);

		if (err == -EINVAL)
			goto csum_copy_err;
	}

	if (unlikely(err)) {
		if (!peeked) {
			atomic_inc(&sk->sk_drops);
			UDP_INC_STATS(sock_net(sk),
				      UDP_MIB_INERRORS, is_udplite);
		}
		kfree_skb(skb);
		return err;
	}

	if (!peeked)
		UDP_INC_STATS(sock_net(sk),
			      UDP_MIB_INDATAGRAMS, is_udplite);

	sock_recv_ts_and_drops(msg, sk, skb);

	/* Copy the address. */
	if (sin) {
		sin->sin_family = AF_INET;
		sin->sin_port = udp_hdr(skb)->source;
		sin->sin_addr.s_addr = ip_hdr(skb)->saddr;
		memset(sin->sin_zero, 0, sizeof(sin->sin_zero));
		*addr_len = sizeof(*sin);
	}
	if (inet->cmsg_flags)
		ip_cmsg_recv_offset(msg, sk, skb, sizeof(struct udphdr), off);

	err = copied;
	if (flags & MSG_TRUNC)
		err = ulen;

	skb_consume_udp(sk, skb, peeking ? -err : err);
	return err;

csum_copy_err:
	if (!__sk_queue_drop_skb(sk, &udp_sk(sk)->reader_queue, skb, flags,
				 udp_skb_destructor)) {
		UDP_INC_STATS(sock_net(sk), UDP_MIB_CSUMERRORS, is_udplite);
		UDP_INC_STATS(sock_net(sk), UDP_MIB_INERRORS, is_udplite);
	}
	kfree_skb(skb);

	/* starting over for a new packet, but check if we need to yield */
	cond_resched();
	msg->msg_flags &= ~MSG_TRUNC;
	goto try_again;
}