static int sctp_assoc_ulpevent_type_set(struct sctp_event *param,
					struct sctp_association *asoc)
{
	struct sctp_ulpevent *event;

	sctp_ulpevent_type_set(&asoc->subscribe, param->se_type, param->se_on);

	if (param->se_type == SCTP_SENDER_DRY_EVENT && param->se_on) {
		if (sctp_outq_is_empty(&asoc->outqueue)) {
			event = sctp_ulpevent_make_sender_dry_event(asoc,
					GFP_USER | __GFP_NOWARN);
			if (!event)
				return -ENOMEM;

			asoc->stream.si->enqueue_event(&asoc->ulpq, event);
		}
	}

	return 0;
}