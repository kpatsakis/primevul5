static void sctp_apply_asoc_delayed_ack(struct sctp_sack_info *params,
					struct sctp_association *asoc)
{
	struct sctp_transport *trans;

	if (params->sack_delay) {
		asoc->sackdelay = msecs_to_jiffies(params->sack_delay);
		asoc->param_flags =
			sctp_spp_sackdelay_enable(asoc->param_flags);
	}
	if (params->sack_freq == 1) {
		asoc->param_flags =
			sctp_spp_sackdelay_disable(asoc->param_flags);
	} else if (params->sack_freq > 1) {
		asoc->sackfreq = params->sack_freq;
		asoc->param_flags =
			sctp_spp_sackdelay_enable(asoc->param_flags);
	}

	list_for_each_entry(trans, &asoc->peer.transport_addr_list,
			    transports) {
		if (params->sack_delay) {
			trans->sackdelay = msecs_to_jiffies(params->sack_delay);
			trans->param_flags =
				sctp_spp_sackdelay_enable(trans->param_flags);
		}
		if (params->sack_freq == 1) {
			trans->param_flags =
				sctp_spp_sackdelay_disable(trans->param_flags);
		} else if (params->sack_freq > 1) {
			trans->sackfreq = params->sack_freq;
			trans->param_flags =
				sctp_spp_sackdelay_enable(trans->param_flags);
		}
	}
}