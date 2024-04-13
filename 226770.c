static int sctp_setsockopt_events(struct sock *sk, __u8 *sn_type,
				  unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	int i;

	if (optlen > sizeof(struct sctp_event_subscribe))
		return -EINVAL;

	for (i = 0; i < optlen; i++)
		sctp_ulpevent_type_set(&sp->subscribe, SCTP_SN_TYPE_BASE + i,
				       sn_type[i]);

	list_for_each_entry(asoc, &sp->ep->asocs, asocs)
		asoc->subscribe = sctp_sk(sk)->subscribe;

	/* At the time when a user app subscribes to SCTP_SENDER_DRY_EVENT,
	 * if there is no data to be sent or retransmit, the stack will
	 * immediately send up this notification.
	 */
	if (sctp_ulpevent_type_enabled(sp->subscribe, SCTP_SENDER_DRY_EVENT)) {
		struct sctp_ulpevent *event;

		asoc = sctp_id2assoc(sk, 0);
		if (asoc && sctp_outq_is_empty(&asoc->outqueue)) {
			event = sctp_ulpevent_make_sender_dry_event(asoc,
					GFP_USER | __GFP_NOWARN);
			if (!event)
				return -ENOMEM;

			asoc->stream.si->enqueue_event(&asoc->ulpq, event);
		}
	}

	return 0;
}