static int sctp_getsockopt_encap_port(struct sock *sk, int len,
				      char __user *optval, int __user *optlen)
{
	struct sctp_association *asoc;
	struct sctp_udpencaps encap;
	struct sctp_transport *t;
	__be16 encap_port;

	if (len < sizeof(encap))
		return -EINVAL;

	len = sizeof(encap);
	if (copy_from_user(&encap, optval, len))
		return -EFAULT;

	/* If an address other than INADDR_ANY is specified, and
	 * no transport is found, then the request is invalid.
	 */
	if (!sctp_is_any(sk, (union sctp_addr *)&encap.sue_address)) {
		t = sctp_addr_id2transport(sk, &encap.sue_address,
					   encap.sue_assoc_id);
		if (!t) {
			pr_debug("%s: failed no transport\n", __func__);
			return -EINVAL;
		}

		encap_port = t->encap_port;
		goto out;
	}

	/* Get association, if assoc_id != SCTP_FUTURE_ASSOC and the
	 * socket is a one to many style socket, and an association
	 * was not found, then the id was invalid.
	 */
	asoc = sctp_id2assoc(sk, encap.sue_assoc_id);
	if (!asoc && encap.sue_assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP)) {
		pr_debug("%s: failed no association\n", __func__);
		return -EINVAL;
	}

	if (asoc) {
		encap_port = asoc->encap_port;
		goto out;
	}

	encap_port = sctp_sk(sk)->encap_port;

out:
	encap.sue_port = (__force uint16_t)encap_port;
	if (copy_to_user(optval, &encap, len))
		return -EFAULT;

	if (put_user(len, optlen))
		return -EFAULT;

	return 0;
}