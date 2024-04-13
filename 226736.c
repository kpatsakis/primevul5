static int sctp_apply_peer_addr_params(struct sctp_paddrparams *params,
				       struct sctp_transport   *trans,
				       struct sctp_association *asoc,
				       struct sctp_sock        *sp,
				       int                      hb_change,
				       int                      pmtud_change,
				       int                      sackdelay_change)
{
	int error;

	if (params->spp_flags & SPP_HB_DEMAND && trans) {
		error = sctp_primitive_REQUESTHEARTBEAT(trans->asoc->base.net,
							trans->asoc, trans);
		if (error)
			return error;
	}

	/* Note that unless the spp_flag is set to SPP_HB_ENABLE the value of
	 * this field is ignored.  Note also that a value of zero indicates
	 * the current setting should be left unchanged.
	 */
	if (params->spp_flags & SPP_HB_ENABLE) {

		/* Re-zero the interval if the SPP_HB_TIME_IS_ZERO is
		 * set.  This lets us use 0 value when this flag
		 * is set.
		 */
		if (params->spp_flags & SPP_HB_TIME_IS_ZERO)
			params->spp_hbinterval = 0;

		if (params->spp_hbinterval ||
		    (params->spp_flags & SPP_HB_TIME_IS_ZERO)) {
			if (trans) {
				trans->hbinterval =
				    msecs_to_jiffies(params->spp_hbinterval);
			} else if (asoc) {
				asoc->hbinterval =
				    msecs_to_jiffies(params->spp_hbinterval);
			} else {
				sp->hbinterval = params->spp_hbinterval;
			}
		}
	}

	if (hb_change) {
		if (trans) {
			trans->param_flags =
				(trans->param_flags & ~SPP_HB) | hb_change;
		} else if (asoc) {
			asoc->param_flags =
				(asoc->param_flags & ~SPP_HB) | hb_change;
		} else {
			sp->param_flags =
				(sp->param_flags & ~SPP_HB) | hb_change;
		}
	}

	/* When Path MTU discovery is disabled the value specified here will
	 * be the "fixed" path mtu (i.e. the value of the spp_flags field must
	 * include the flag SPP_PMTUD_DISABLE for this field to have any
	 * effect).
	 */
	if ((params->spp_flags & SPP_PMTUD_DISABLE) && params->spp_pathmtu) {
		if (trans) {
			trans->pathmtu = params->spp_pathmtu;
			sctp_assoc_sync_pmtu(asoc);
		} else if (asoc) {
			sctp_assoc_set_pmtu(asoc, params->spp_pathmtu);
		} else {
			sp->pathmtu = params->spp_pathmtu;
		}
	}

	if (pmtud_change) {
		if (trans) {
			int update = (trans->param_flags & SPP_PMTUD_DISABLE) &&
				(params->spp_flags & SPP_PMTUD_ENABLE);
			trans->param_flags =
				(trans->param_flags & ~SPP_PMTUD) | pmtud_change;
			if (update) {
				sctp_transport_pmtu(trans, sctp_opt2sk(sp));
				sctp_assoc_sync_pmtu(asoc);
			}
		} else if (asoc) {
			asoc->param_flags =
				(asoc->param_flags & ~SPP_PMTUD) | pmtud_change;
		} else {
			sp->param_flags =
				(sp->param_flags & ~SPP_PMTUD) | pmtud_change;
		}
	}

	/* Note that unless the spp_flag is set to SPP_SACKDELAY_ENABLE the
	 * value of this field is ignored.  Note also that a value of zero
	 * indicates the current setting should be left unchanged.
	 */
	if ((params->spp_flags & SPP_SACKDELAY_ENABLE) && params->spp_sackdelay) {
		if (trans) {
			trans->sackdelay =
				msecs_to_jiffies(params->spp_sackdelay);
		} else if (asoc) {
			asoc->sackdelay =
				msecs_to_jiffies(params->spp_sackdelay);
		} else {
			sp->sackdelay = params->spp_sackdelay;
		}
	}

	if (sackdelay_change) {
		if (trans) {
			trans->param_flags =
				(trans->param_flags & ~SPP_SACKDELAY) |
				sackdelay_change;
		} else if (asoc) {
			asoc->param_flags =
				(asoc->param_flags & ~SPP_SACKDELAY) |
				sackdelay_change;
		} else {
			sp->param_flags =
				(sp->param_flags & ~SPP_SACKDELAY) |
				sackdelay_change;
		}
	}

	/* Note that a value of zero indicates the current setting should be
	   left unchanged.
	 */
	if (params->spp_pathmaxrxt) {
		if (trans) {
			trans->pathmaxrxt = params->spp_pathmaxrxt;
		} else if (asoc) {
			asoc->pathmaxrxt = params->spp_pathmaxrxt;
		} else {
			sp->pathmaxrxt = params->spp_pathmaxrxt;
		}
	}

	if (params->spp_flags & SPP_IPV6_FLOWLABEL) {
		if (trans) {
			if (trans->ipaddr.sa.sa_family == AF_INET6) {
				trans->flowlabel = params->spp_ipv6_flowlabel &
						   SCTP_FLOWLABEL_VAL_MASK;
				trans->flowlabel |= SCTP_FLOWLABEL_SET_MASK;
			}
		} else if (asoc) {
			struct sctp_transport *t;

			list_for_each_entry(t, &asoc->peer.transport_addr_list,
					    transports) {
				if (t->ipaddr.sa.sa_family != AF_INET6)
					continue;
				t->flowlabel = params->spp_ipv6_flowlabel &
					       SCTP_FLOWLABEL_VAL_MASK;
				t->flowlabel |= SCTP_FLOWLABEL_SET_MASK;
			}
			asoc->flowlabel = params->spp_ipv6_flowlabel &
					  SCTP_FLOWLABEL_VAL_MASK;
			asoc->flowlabel |= SCTP_FLOWLABEL_SET_MASK;
		} else if (sctp_opt2sk(sp)->sk_family == AF_INET6) {
			sp->flowlabel = params->spp_ipv6_flowlabel &
					SCTP_FLOWLABEL_VAL_MASK;
			sp->flowlabel |= SCTP_FLOWLABEL_SET_MASK;
		}
	}

	if (params->spp_flags & SPP_DSCP) {
		if (trans) {
			trans->dscp = params->spp_dscp & SCTP_DSCP_VAL_MASK;
			trans->dscp |= SCTP_DSCP_SET_MASK;
		} else if (asoc) {
			struct sctp_transport *t;

			list_for_each_entry(t, &asoc->peer.transport_addr_list,
					    transports) {
				t->dscp = params->spp_dscp &
					  SCTP_DSCP_VAL_MASK;
				t->dscp |= SCTP_DSCP_SET_MASK;
			}
			asoc->dscp = params->spp_dscp & SCTP_DSCP_VAL_MASK;
			asoc->dscp |= SCTP_DSCP_SET_MASK;
		} else {
			sp->dscp = params->spp_dscp & SCTP_DSCP_VAL_MASK;
			sp->dscp |= SCTP_DSCP_SET_MASK;
		}
	}

	return 0;
}