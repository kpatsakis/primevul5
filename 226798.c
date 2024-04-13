static int sctp_sendmsg_check_sflags(struct sctp_association *asoc,
				     __u16 sflags, struct msghdr *msg,
				     size_t msg_len)
{
	struct sock *sk = asoc->base.sk;
	struct net *net = sock_net(sk);

	if (sctp_state(asoc, CLOSED) && sctp_style(sk, TCP))
		return -EPIPE;

	if ((sflags & SCTP_SENDALL) && sctp_style(sk, UDP) &&
	    !sctp_state(asoc, ESTABLISHED))
		return 0;

	if (sflags & SCTP_EOF) {
		pr_debug("%s: shutting down association:%p\n", __func__, asoc);
		sctp_primitive_SHUTDOWN(net, asoc, NULL);

		return 0;
	}

	if (sflags & SCTP_ABORT) {
		struct sctp_chunk *chunk;

		chunk = sctp_make_abort_user(asoc, msg, msg_len);
		if (!chunk)
			return -ENOMEM;

		pr_debug("%s: aborting association:%p\n", __func__, asoc);
		sctp_primitive_ABORT(net, asoc, chunk);
		iov_iter_revert(&msg->msg_iter, msg_len);

		return 0;
	}

	return 1;
}