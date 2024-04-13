static int irda_sendmsg_dgram(struct kiocb *iocb, struct socket *sock,
			      struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct irda_sock *self;
	struct sk_buff *skb;
	int err;

	IRDA_DEBUG(4, "%s(), len=%zd\n", __func__, len);

	if (msg->msg_flags & ~(MSG_DONTWAIT|MSG_CMSG_COMPAT))
		return -EINVAL;

	if (sk->sk_shutdown & SEND_SHUTDOWN) {
		send_sig(SIGPIPE, current, 0);
		return -EPIPE;
	}

	if (sk->sk_state != TCP_ESTABLISHED)
		return -ENOTCONN;

	self = irda_sk(sk);

	/*
	 * Check that we don't send out too big frames. This is an unreliable
	 * service, so we have no fragmentation and no coalescence
	 */
	if (len > self->max_data_size) {
		IRDA_DEBUG(0, "%s(), Warning to much data! "
			   "Chopping frame from %zd to %d bytes!\n",
			   __func__, len, self->max_data_size);
		len = self->max_data_size;
	}

	skb = sock_alloc_send_skb(sk, len + self->max_header_size,
				  msg->msg_flags & MSG_DONTWAIT, &err);
	if (!skb)
		return -ENOBUFS;

	skb_reserve(skb, self->max_header_size);
	skb_reset_transport_header(skb);

	IRDA_DEBUG(4, "%s(), appending user data\n", __func__);
	skb_put(skb, len);
	err = memcpy_fromiovec(skb_transport_header(skb), msg->msg_iov, len);
	if (err) {
		kfree_skb(skb);
		return err;
	}

	/*
	 * Just send the message to TinyTP, and let it deal with possible
	 * errors. No need to duplicate all that here
	 */
	err = irttp_udata_request(self->tsap, skb);
	if (err) {
		IRDA_DEBUG(0, "%s(), err=%d\n", __func__, err);
		return err;
	}
	return len;
}