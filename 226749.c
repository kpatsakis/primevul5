static int sctp_setsockopt(struct sock *sk, int level, int optname,
			   sockptr_t optval, unsigned int optlen)
{
	void *kopt = NULL;
	int retval = 0;

	pr_debug("%s: sk:%p, optname:%d\n", __func__, sk, optname);

	/* I can hardly begin to describe how wrong this is.  This is
	 * so broken as to be worse than useless.  The API draft
	 * REALLY is NOT helpful here...  I am not convinced that the
	 * semantics of setsockopt() with a level OTHER THAN SOL_SCTP
	 * are at all well-founded.
	 */
	if (level != SOL_SCTP) {
		struct sctp_af *af = sctp_sk(sk)->pf->af;

		return af->setsockopt(sk, level, optname, optval, optlen);
	}

	if (optlen > 0) {
		kopt = memdup_sockptr(optval, optlen);
		if (IS_ERR(kopt))
			return PTR_ERR(kopt);
	}

	lock_sock(sk);

	switch (optname) {
	case SCTP_SOCKOPT_BINDX_ADD:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_bindx(sk, kopt, optlen,
					       SCTP_BINDX_ADD_ADDR);
		break;

	case SCTP_SOCKOPT_BINDX_REM:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_bindx(sk, kopt, optlen,
					       SCTP_BINDX_REM_ADDR);
		break;

	case SCTP_SOCKOPT_CONNECTX_OLD:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_connectx_old(sk, kopt, optlen);
		break;

	case SCTP_SOCKOPT_CONNECTX:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_connectx(sk, kopt, optlen);
		break;

	case SCTP_DISABLE_FRAGMENTS:
		retval = sctp_setsockopt_disable_fragments(sk, kopt, optlen);
		break;

	case SCTP_EVENTS:
		retval = sctp_setsockopt_events(sk, kopt, optlen);
		break;

	case SCTP_AUTOCLOSE:
		retval = sctp_setsockopt_autoclose(sk, kopt, optlen);
		break;

	case SCTP_PEER_ADDR_PARAMS:
		retval = sctp_setsockopt_peer_addr_params(sk, kopt, optlen);
		break;

	case SCTP_DELAYED_SACK:
		retval = sctp_setsockopt_delayed_ack(sk, kopt, optlen);
		break;
	case SCTP_PARTIAL_DELIVERY_POINT:
		retval = sctp_setsockopt_partial_delivery_point(sk, kopt, optlen);
		break;

	case SCTP_INITMSG:
		retval = sctp_setsockopt_initmsg(sk, kopt, optlen);
		break;
	case SCTP_DEFAULT_SEND_PARAM:
		retval = sctp_setsockopt_default_send_param(sk, kopt, optlen);
		break;
	case SCTP_DEFAULT_SNDINFO:
		retval = sctp_setsockopt_default_sndinfo(sk, kopt, optlen);
		break;
	case SCTP_PRIMARY_ADDR:
		retval = sctp_setsockopt_primary_addr(sk, kopt, optlen);
		break;
	case SCTP_SET_PEER_PRIMARY_ADDR:
		retval = sctp_setsockopt_peer_primary_addr(sk, kopt, optlen);
		break;
	case SCTP_NODELAY:
		retval = sctp_setsockopt_nodelay(sk, kopt, optlen);
		break;
	case SCTP_RTOINFO:
		retval = sctp_setsockopt_rtoinfo(sk, kopt, optlen);
		break;
	case SCTP_ASSOCINFO:
		retval = sctp_setsockopt_associnfo(sk, kopt, optlen);
		break;
	case SCTP_I_WANT_MAPPED_V4_ADDR:
		retval = sctp_setsockopt_mappedv4(sk, kopt, optlen);
		break;
	case SCTP_MAXSEG:
		retval = sctp_setsockopt_maxseg(sk, kopt, optlen);
		break;
	case SCTP_ADAPTATION_LAYER:
		retval = sctp_setsockopt_adaptation_layer(sk, kopt, optlen);
		break;
	case SCTP_CONTEXT:
		retval = sctp_setsockopt_context(sk, kopt, optlen);
		break;
	case SCTP_FRAGMENT_INTERLEAVE:
		retval = sctp_setsockopt_fragment_interleave(sk, kopt, optlen);
		break;
	case SCTP_MAX_BURST:
		retval = sctp_setsockopt_maxburst(sk, kopt, optlen);
		break;
	case SCTP_AUTH_CHUNK:
		retval = sctp_setsockopt_auth_chunk(sk, kopt, optlen);
		break;
	case SCTP_HMAC_IDENT:
		retval = sctp_setsockopt_hmac_ident(sk, kopt, optlen);
		break;
	case SCTP_AUTH_KEY:
		retval = sctp_setsockopt_auth_key(sk, kopt, optlen);
		break;
	case SCTP_AUTH_ACTIVE_KEY:
		retval = sctp_setsockopt_active_key(sk, kopt, optlen);
		break;
	case SCTP_AUTH_DELETE_KEY:
		retval = sctp_setsockopt_del_key(sk, kopt, optlen);
		break;
	case SCTP_AUTH_DEACTIVATE_KEY:
		retval = sctp_setsockopt_deactivate_key(sk, kopt, optlen);
		break;
	case SCTP_AUTO_ASCONF:
		retval = sctp_setsockopt_auto_asconf(sk, kopt, optlen);
		break;
	case SCTP_PEER_ADDR_THLDS:
		retval = sctp_setsockopt_paddr_thresholds(sk, kopt, optlen,
							  false);
		break;
	case SCTP_PEER_ADDR_THLDS_V2:
		retval = sctp_setsockopt_paddr_thresholds(sk, kopt, optlen,
							  true);
		break;
	case SCTP_RECVRCVINFO:
		retval = sctp_setsockopt_recvrcvinfo(sk, kopt, optlen);
		break;
	case SCTP_RECVNXTINFO:
		retval = sctp_setsockopt_recvnxtinfo(sk, kopt, optlen);
		break;
	case SCTP_PR_SUPPORTED:
		retval = sctp_setsockopt_pr_supported(sk, kopt, optlen);
		break;
	case SCTP_DEFAULT_PRINFO:
		retval = sctp_setsockopt_default_prinfo(sk, kopt, optlen);
		break;
	case SCTP_RECONFIG_SUPPORTED:
		retval = sctp_setsockopt_reconfig_supported(sk, kopt, optlen);
		break;
	case SCTP_ENABLE_STREAM_RESET:
		retval = sctp_setsockopt_enable_strreset(sk, kopt, optlen);
		break;
	case SCTP_RESET_STREAMS:
		retval = sctp_setsockopt_reset_streams(sk, kopt, optlen);
		break;
	case SCTP_RESET_ASSOC:
		retval = sctp_setsockopt_reset_assoc(sk, kopt, optlen);
		break;
	case SCTP_ADD_STREAMS:
		retval = sctp_setsockopt_add_streams(sk, kopt, optlen);
		break;
	case SCTP_STREAM_SCHEDULER:
		retval = sctp_setsockopt_scheduler(sk, kopt, optlen);
		break;
	case SCTP_STREAM_SCHEDULER_VALUE:
		retval = sctp_setsockopt_scheduler_value(sk, kopt, optlen);
		break;
	case SCTP_INTERLEAVING_SUPPORTED:
		retval = sctp_setsockopt_interleaving_supported(sk, kopt,
								optlen);
		break;
	case SCTP_REUSE_PORT:
		retval = sctp_setsockopt_reuse_port(sk, kopt, optlen);
		break;
	case SCTP_EVENT:
		retval = sctp_setsockopt_event(sk, kopt, optlen);
		break;
	case SCTP_ASCONF_SUPPORTED:
		retval = sctp_setsockopt_asconf_supported(sk, kopt, optlen);
		break;
	case SCTP_AUTH_SUPPORTED:
		retval = sctp_setsockopt_auth_supported(sk, kopt, optlen);
		break;
	case SCTP_ECN_SUPPORTED:
		retval = sctp_setsockopt_ecn_supported(sk, kopt, optlen);
		break;
	case SCTP_EXPOSE_POTENTIALLY_FAILED_STATE:
		retval = sctp_setsockopt_pf_expose(sk, kopt, optlen);
		break;
	case SCTP_REMOTE_UDP_ENCAPS_PORT:
		retval = sctp_setsockopt_encap_port(sk, kopt, optlen);
		break;
	default:
		retval = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	kfree(kopt);
	return retval;
}