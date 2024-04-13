void sctp_transport_walk_start(struct rhashtable_iter *iter) __acquires(RCU)
{
	rhltable_walk_enter(&sctp_transport_hashtable, iter);

	rhashtable_walk_start(iter);
}