struct sctp_transport *sctp_transport_get_idx(struct net *net,
					      struct rhashtable_iter *iter,
					      int pos)
{
	struct sctp_transport *t;

	if (!pos)
		return SEQ_START_TOKEN;

	while ((t = sctp_transport_get_next(net, iter)) && !IS_ERR(t)) {
		if (!--pos)
			break;
		sctp_transport_put(t);
	}

	return t;
}