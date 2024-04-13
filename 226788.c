static int sctp_setsockopt_associnfo(struct sock *sk,
				     struct sctp_assocparams *assocparams,
				     unsigned int optlen)
{

	struct sctp_association *asoc;

	if (optlen != sizeof(struct sctp_assocparams))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, assocparams->sasoc_assoc_id);

	if (!asoc && assocparams->sasoc_assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	/* Set the values to the specific association */
	if (asoc) {
		if (assocparams->sasoc_asocmaxrxt != 0) {
			__u32 path_sum = 0;
			int   paths = 0;
			struct sctp_transport *peer_addr;

			list_for_each_entry(peer_addr, &asoc->peer.transport_addr_list,
					transports) {
				path_sum += peer_addr->pathmaxrxt;
				paths++;
			}

			/* Only validate asocmaxrxt if we have more than
			 * one path/transport.  We do this because path
			 * retransmissions are only counted when we have more
			 * then one path.
			 */
			if (paths > 1 &&
			    assocparams->sasoc_asocmaxrxt > path_sum)
				return -EINVAL;

			asoc->max_retrans = assocparams->sasoc_asocmaxrxt;
		}

		if (assocparams->sasoc_cookie_life != 0)
			asoc->cookie_life =
				ms_to_ktime(assocparams->sasoc_cookie_life);
	} else {
		/* Set the values to the endpoint */
		struct sctp_sock *sp = sctp_sk(sk);

		if (assocparams->sasoc_asocmaxrxt != 0)
			sp->assocparams.sasoc_asocmaxrxt =
						assocparams->sasoc_asocmaxrxt;
		if (assocparams->sasoc_cookie_life != 0)
			sp->assocparams.sasoc_cookie_life =
						assocparams->sasoc_cookie_life;
	}
	return 0;
}