struct sctp_transport *sctp_transport_get_next(struct net *net,
					       struct rhashtable_iter *iter)
{
	struct sctp_transport *t;

	t = rhashtable_walk_next(iter);
	for (; t; t = rhashtable_walk_next(iter)) {
		if (IS_ERR(t)) {
			if (PTR_ERR(t) == -EAGAIN)
				continue;
			break;
		}

		if (!sctp_transport_hold(t))
			continue;

		if (net_eq(t->asoc->base.net, net) &&
		    t->asoc->peer.primary_path == t)
			break;

		sctp_transport_put(t);
	}

	return t;
}