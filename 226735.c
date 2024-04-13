static int sctp_getsockopt(struct sock *sk, int level, int optname,
			   char __user *optval, int __user *optlen)
{
	int retval = 0;
	int len;

	pr_debug("%s: sk:%p, optname:%d\n", __func__, sk, optname);

	/* I can hardly begin to describe how wrong this is.  This is
	 * so broken as to be worse than useless.  The API draft
	 * REALLY is NOT helpful here...  I am not convinced that the
	 * semantics of getsockopt() with a level OTHER THAN SOL_SCTP
	 * are at all well-founded.
	 */
	if (level != SOL_SCTP) {
		struct sctp_af *af = sctp_sk(sk)->pf->af;

		retval = af->getsockopt(sk, level, optname, optval, optlen);
		return retval;
	}

	if (get_user(len, optlen))
		return -EFAULT;

	if (len < 0)
		return -EINVAL;

	lock_sock(sk);

	switch (optname) {
	case SCTP_STATUS:
		retval = sctp_getsockopt_sctp_status(sk, len, optval, optlen);
		break;
	case SCTP_DISABLE_FRAGMENTS:
		retval = sctp_getsockopt_disable_fragments(sk, len, optval,
							   optlen);
		break;
	case SCTP_EVENTS:
		retval = sctp_getsockopt_events(sk, len, optval, optlen);
		break;
	case SCTP_AUTOCLOSE:
		retval = sctp_getsockopt_autoclose(sk, len, optval, optlen);
		break;
	case SCTP_SOCKOPT_PEELOFF:
		retval = sctp_getsockopt_peeloff(sk, len, optval, optlen);
		break;
	case SCTP_SOCKOPT_PEELOFF_FLAGS:
		retval = sctp_getsockopt_peeloff_flags(sk, len, optval, optlen);
		break;
	case SCTP_PEER_ADDR_PARAMS:
		retval = sctp_getsockopt_peer_addr_params(sk, len, optval,
							  optlen);
		break;
	case SCTP_DELAYED_SACK:
		retval = sctp_getsockopt_delayed_ack(sk, len, optval,
							  optlen);
		break;
	case SCTP_INITMSG:
		retval = sctp_getsockopt_initmsg(sk, len, optval, optlen);
		break;
	case SCTP_GET_PEER_ADDRS:
		retval = sctp_getsockopt_peer_addrs(sk, len, optval,
						    optlen);
		break;
	case SCTP_GET_LOCAL_ADDRS:
		retval = sctp_getsockopt_local_addrs(sk, len, optval,
						     optlen);
		break;
	case SCTP_SOCKOPT_CONNECTX3:
		retval = sctp_getsockopt_connectx3(sk, len, optval, optlen);
		break;
	case SCTP_DEFAULT_SEND_PARAM:
		retval = sctp_getsockopt_default_send_param(sk, len,
							    optval, optlen);
		break;
	case SCTP_DEFAULT_SNDINFO:
		retval = sctp_getsockopt_default_sndinfo(sk, len,
							 optval, optlen);
		break;
	case SCTP_PRIMARY_ADDR:
		retval = sctp_getsockopt_primary_addr(sk, len, optval, optlen);
		break;
	case SCTP_NODELAY:
		retval = sctp_getsockopt_nodelay(sk, len, optval, optlen);
		break;
	case SCTP_RTOINFO:
		retval = sctp_getsockopt_rtoinfo(sk, len, optval, optlen);
		break;
	case SCTP_ASSOCINFO:
		retval = sctp_getsockopt_associnfo(sk, len, optval, optlen);
		break;
	case SCTP_I_WANT_MAPPED_V4_ADDR:
		retval = sctp_getsockopt_mappedv4(sk, len, optval, optlen);
		break;
	case SCTP_MAXSEG:
		retval = sctp_getsockopt_maxseg(sk, len, optval, optlen);
		break;
	case SCTP_GET_PEER_ADDR_INFO:
		retval = sctp_getsockopt_peer_addr_info(sk, len, optval,
							optlen);
		break;
	case SCTP_ADAPTATION_LAYER:
		retval = sctp_getsockopt_adaptation_layer(sk, len, optval,
							optlen);
		break;
	case SCTP_CONTEXT:
		retval = sctp_getsockopt_context(sk, len, optval, optlen);
		break;
	case SCTP_FRAGMENT_INTERLEAVE:
		retval = sctp_getsockopt_fragment_interleave(sk, len, optval,
							     optlen);
		break;
	case SCTP_PARTIAL_DELIVERY_POINT:
		retval = sctp_getsockopt_partial_delivery_point(sk, len, optval,
								optlen);
		break;
	case SCTP_MAX_BURST:
		retval = sctp_getsockopt_maxburst(sk, len, optval, optlen);
		break;
	case SCTP_AUTH_KEY:
	case SCTP_AUTH_CHUNK:
	case SCTP_AUTH_DELETE_KEY:
	case SCTP_AUTH_DEACTIVATE_KEY:
		retval = -EOPNOTSUPP;
		break;
	case SCTP_HMAC_IDENT:
		retval = sctp_getsockopt_hmac_ident(sk, len, optval, optlen);
		break;
	case SCTP_AUTH_ACTIVE_KEY:
		retval = sctp_getsockopt_active_key(sk, len, optval, optlen);
		break;
	case SCTP_PEER_AUTH_CHUNKS:
		retval = sctp_getsockopt_peer_auth_chunks(sk, len, optval,
							optlen);
		break;
	case SCTP_LOCAL_AUTH_CHUNKS:
		retval = sctp_getsockopt_local_auth_chunks(sk, len, optval,
							optlen);
		break;
	case SCTP_GET_ASSOC_NUMBER:
		retval = sctp_getsockopt_assoc_number(sk, len, optval, optlen);
		break;
	case SCTP_GET_ASSOC_ID_LIST:
		retval = sctp_getsockopt_assoc_ids(sk, len, optval, optlen);
		break;
	case SCTP_AUTO_ASCONF:
		retval = sctp_getsockopt_auto_asconf(sk, len, optval, optlen);
		break;
	case SCTP_PEER_ADDR_THLDS:
		retval = sctp_getsockopt_paddr_thresholds(sk, optval, len,
							  optlen, false);
		break;
	case SCTP_PEER_ADDR_THLDS_V2:
		retval = sctp_getsockopt_paddr_thresholds(sk, optval, len,
							  optlen, true);
		break;
	case SCTP_GET_ASSOC_STATS:
		retval = sctp_getsockopt_assoc_stats(sk, len, optval, optlen);
		break;
	case SCTP_RECVRCVINFO:
		retval = sctp_getsockopt_recvrcvinfo(sk, len, optval, optlen);
		break;
	case SCTP_RECVNXTINFO:
		retval = sctp_getsockopt_recvnxtinfo(sk, len, optval, optlen);
		break;
	case SCTP_PR_SUPPORTED:
		retval = sctp_getsockopt_pr_supported(sk, len, optval, optlen);
		break;
	case SCTP_DEFAULT_PRINFO:
		retval = sctp_getsockopt_default_prinfo(sk, len, optval,
							optlen);
		break;
	case SCTP_PR_ASSOC_STATUS:
		retval = sctp_getsockopt_pr_assocstatus(sk, len, optval,
							optlen);
		break;
	case SCTP_PR_STREAM_STATUS:
		retval = sctp_getsockopt_pr_streamstatus(sk, len, optval,
							 optlen);
		break;
	case SCTP_RECONFIG_SUPPORTED:
		retval = sctp_getsockopt_reconfig_supported(sk, len, optval,
							    optlen);
		break;
	case SCTP_ENABLE_STREAM_RESET:
		retval = sctp_getsockopt_enable_strreset(sk, len, optval,
							 optlen);
		break;
	case SCTP_STREAM_SCHEDULER:
		retval = sctp_getsockopt_scheduler(sk, len, optval,
						   optlen);
		break;
	case SCTP_STREAM_SCHEDULER_VALUE:
		retval = sctp_getsockopt_scheduler_value(sk, len, optval,
							 optlen);
		break;
	case SCTP_INTERLEAVING_SUPPORTED:
		retval = sctp_getsockopt_interleaving_supported(sk, len, optval,
								optlen);
		break;
	case SCTP_REUSE_PORT:
		retval = sctp_getsockopt_reuse_port(sk, len, optval, optlen);
		break;
	case SCTP_EVENT:
		retval = sctp_getsockopt_event(sk, len, optval, optlen);
		break;
	case SCTP_ASCONF_SUPPORTED:
		retval = sctp_getsockopt_asconf_supported(sk, len, optval,
							  optlen);
		break;
	case SCTP_AUTH_SUPPORTED:
		retval = sctp_getsockopt_auth_supported(sk, len, optval,
							optlen);
		break;
	case SCTP_ECN_SUPPORTED:
		retval = sctp_getsockopt_ecn_supported(sk, len, optval, optlen);
		break;
	case SCTP_EXPOSE_POTENTIALLY_FAILED_STATE:
		retval = sctp_getsockopt_pf_expose(sk, len, optval, optlen);
		break;
	case SCTP_REMOTE_UDP_ENCAPS_PORT:
		retval = sctp_getsockopt_encap_port(sk, len, optval, optlen);
		break;
	default:
		retval = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return retval;
}