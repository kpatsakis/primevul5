static int sctp_setsockopt_auth_key(struct sock *sk,
				    struct sctp_authkey *authkey,
				    unsigned int optlen)
{
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;
	struct sctp_association *asoc;
	int ret = -EINVAL;

	if (optlen <= sizeof(struct sctp_authkey))
		return -EINVAL;
	/* authkey->sca_keylength is u16, so optlen can't be bigger than
	 * this.
	 */
	optlen = min_t(unsigned int, optlen, USHRT_MAX + sizeof(*authkey));

	if (authkey->sca_keylength > optlen - sizeof(*authkey))
		goto out;

	asoc = sctp_id2assoc(sk, authkey->sca_assoc_id);
	if (!asoc && authkey->sca_assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		goto out;

	if (asoc) {
		ret = sctp_auth_set_key(ep, asoc, authkey);
		goto out;
	}

	if (sctp_style(sk, TCP))
		authkey->sca_assoc_id = SCTP_FUTURE_ASSOC;

	if (authkey->sca_assoc_id == SCTP_FUTURE_ASSOC ||
	    authkey->sca_assoc_id == SCTP_ALL_ASSOC) {
		ret = sctp_auth_set_key(ep, asoc, authkey);
		if (ret)
			goto out;
	}

	ret = 0;

	if (authkey->sca_assoc_id == SCTP_CURRENT_ASSOC ||
	    authkey->sca_assoc_id == SCTP_ALL_ASSOC) {
		list_for_each_entry(asoc, &ep->asocs, asocs) {
			int res = sctp_auth_set_key(ep, asoc, authkey);

			if (res && !ret)
				ret = res;
		}
	}

out:
	memzero_explicit(authkey, optlen);
	return ret;
}