static union sctp_addr *sctp_sendmsg_get_daddr(struct sock *sk,
					       const struct msghdr *msg,
					       struct sctp_cmsgs *cmsgs)
{
	union sctp_addr *daddr = NULL;
	int err;

	if (!sctp_style(sk, UDP_HIGH_BANDWIDTH) && msg->msg_name) {
		int len = msg->msg_namelen;

		if (len > sizeof(*daddr))
			len = sizeof(*daddr);

		daddr = (union sctp_addr *)msg->msg_name;

		err = sctp_verify_addr(sk, daddr, len);
		if (err)
			return ERR_PTR(err);
	}

	return daddr;
}