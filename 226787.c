static int sctp_getsockopt_active_key(struct sock *sk, int len,
				    char __user *optval, int __user *optlen)
{
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;
	struct sctp_authkeyid val;
	struct sctp_association *asoc;

	if (len < sizeof(struct sctp_authkeyid))
		return -EINVAL;

	len = sizeof(struct sctp_authkeyid);
	if (copy_from_user(&val, optval, len))
		return -EFAULT;

	asoc = sctp_id2assoc(sk, val.scact_assoc_id);
	if (!asoc && val.scact_assoc_id && sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		if (!asoc->peer.auth_capable)
			return -EACCES;
		val.scact_keynumber = asoc->active_key_id;
	} else {
		if (!ep->auth_enable)
			return -EACCES;
		val.scact_keynumber = ep->active_key_id;
	}

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &val, len))
		return -EFAULT;

	return 0;
}