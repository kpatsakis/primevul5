static int sctp_setsockopt_del_key(struct sock *sk,
				   struct sctp_authkeyid *val,
				   unsigned int optlen)
{
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;
	struct sctp_association *asoc;
	int ret = 0;

	if (optlen != sizeof(struct sctp_authkeyid))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, val->scact_assoc_id);
	if (!asoc && val->scact_assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc)
		return sctp_auth_del_key_id(ep, asoc, val->scact_keynumber);

	if (sctp_style(sk, TCP))
		val->scact_assoc_id = SCTP_FUTURE_ASSOC;

	if (val->scact_assoc_id == SCTP_FUTURE_ASSOC ||
	    val->scact_assoc_id == SCTP_ALL_ASSOC) {
		ret = sctp_auth_del_key_id(ep, asoc, val->scact_keynumber);
		if (ret)
			return ret;
	}

	if (val->scact_assoc_id == SCTP_CURRENT_ASSOC ||
	    val->scact_assoc_id == SCTP_ALL_ASSOC) {
		list_for_each_entry(asoc, &ep->asocs, asocs) {
			int res = sctp_auth_del_key_id(ep, asoc,
						       val->scact_keynumber);

			if (res && !ret)
				ret = res;
		}
	}

	return ret;
}